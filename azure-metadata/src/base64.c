/*
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include <string.h>

#include "base64.h"
#include "dbg.h"

static const unsigned char base64_table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int base64_line_length = 72;

/* URL Safe Modifications...
 * 0x2B = +  <-->  0x2D = -   (62)
 * 0x2F = /  <-->  0x5F = _   (63)
 */

static const char base64_reverse_table[128] = {
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, /* 0x */
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 62, 64, 63, /* 2x */
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* 4x */
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 63,
   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 6x */
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

/* https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl */

/**
 * function to allow easier decoding of urlsafe base64 encoded data.
 */
int base64_decode_ex(char * src, size_t src_len, char * dst, size_t * dst_len) {
	char * src_end;
	char * src_ptr;
	char * dst_ptr;
	int c;
	int accumulator;
	int bits_collected;
	if (src_len == 0) {
		src_len = strlen(src);
	}
	src_ptr = src;
	src_end = src + src_len;
	dst_ptr = dst;
	accumulator = 0;
	bits_collected = 0;
	do {
		c = *src_ptr;
		if (c == '=' || c == ' ' || c == '\r' || c == '\n') {
			goto skip;
		}
		if (base64_reverse_table[c] > 63) {
			goto error;
		}
		accumulator = (accumulator << 6) | base64_reverse_table[c];
		bits_collected += 6;
		if (bits_collected >= 8) {
			bits_collected -= 8;
			*dst_ptr = (accumulator >> bits_collected) & 0xff;
			dst_ptr++;
		}
skip:
		src_ptr ++;
	} while (src_ptr < src_end);
	*dst_len = dst_ptr - dst;
	return 0;
error:
	return -1;
}

/**
 * base64_reqbufsz - Calculate destination buffer size required for given input
 * length.
 * 1, 3-byte blocks to 4-byte
 * 2, newlines
 * 3, trailing newlinw and null-terminator
 */
size_t base64_reqbufsz(size_t lentoencode) {
	size_t olen;
	olen = lentoencode * 4 / 3 + 4; 
	olen += olen / base64_line_length;
	olen += 2;
	return olen;
}

/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out: Destination buffer
 * @out_len: Pointer to output length variable, or %NULL if not used
 * Returns: Allocated buffer of out_len bytes of encoded data,
 * or %NULL on failure
 *
 * Caller is responsible for ensuring that the destination buffer is big enough. 
 * nul terminated to make it easier to use as a C string. The nul terminator is
 * not included in out_len.
 */
unsigned char * base64_encode(const unsigned char *src, size_t len,
			      unsigned char *out, size_t *out_len) {
	unsigned char *pos;
	const unsigned char *end, *in;
	size_t olen;
	int line_len;

	olen = base64_reqbufsz(len);
	if (olen > *out_len) {
		*out_len = olen;
		return NULL;
	}
	if (olen < len)
		return NULL; /* integer overflow */
	if (out == NULL)
		return NULL;

	end = src + len;
	in = src;
	pos = out;
	line_len = 0;
	while (end - in >= 3) {
		*pos++ = base64_table[(in[0] >> 2) & 0x3f];
		*pos++ = base64_table[(((in[0] & 0x03) << 4) |
				       (in[1] >> 4)) & 0x3f];
		*pos++ = base64_table[(((in[1] & 0x0f) << 2) |
				       (in[2] >> 6)) & 0x3f];
		*pos++ = base64_table[in[2] & 0x3f];
		in += 3;
		line_len += 4;
		if (line_len >= 72) {
			*pos++ = '\n';
			line_len = 0;
		}
	}

	if (end - in) {
		*pos++ = base64_table[(in[0] >> 2) & 0x3f];
		if (end - in == 1) {
			*pos++ = base64_table[((in[0] & 0x03) << 4) & 0x3f];
			*pos++ = '=';
		} else {
			*pos++ = base64_table[(((in[0] & 0x03) << 4) |
					       (in[1] >> 4)) & 0x3f];
			*pos++ = base64_table[((in[1] & 0x0f) << 2) & 0x3f];
		}
		*pos++ = '=';
		line_len += 4;
	}

	if (line_len)
		*pos++ = '\n';

	*pos = '\0';
	if (out_len)
		*out_len = pos - out;
	return out;
}

/* 
 * Rather in-efficient but gets the job done.
 * In the case of a mis-match decoding the partial portion should NOT
 * be simply passed to the base64_decode function.
 * TODO: Fix this to make it better
 */
int base64_decode_forgiving(const unsigned char * src, size_t src_len,
		unsigned char * out, size_t * out_len) {
	if (src_len & 3) {
		size_t remainder = (src_len & 3);
		size_t out_size = *out_len;
		unsigned char buf[4];
		base64_decode(src, src_len - remainder, out, out_len);
		memset(buf, '=', 4);
		memcpy(buf, src + src_len - remainder, remainder);
		remainder = out_size - *out_len;
		base64_decode(buf, 4, out + *out_len, &remainder);
		*out_len = *out_len + remainder;
	} else {
		base64_decode(src, src_len, out, out_len);
	}
	return 0;
}

/**
 * base64_decode - Base64 decode
 * @src: Data to be decoded
 * @len: Length of the data to be decoded
 * @out: Destination buffer
 * @out_len: Pointer to output length variable
 * Returns: Allocated buffer of out_len bytes of decoded data,
 * or %NULL on failure
 *
 * Caller is responsible for size of destination buffer.
 */
unsigned char * base64_decode(const unsigned char *src, size_t len,
			      unsigned char *out, size_t *out_len)
{
	unsigned char dtable[256], *pos, block[4], tmp;
	size_t i, count, olen;
	int pad = 0;

	memset(dtable, 0x80, 256);
	for (i = 0; i < sizeof(base64_table) - 1; i++)
		dtable[base64_table[i]] = (unsigned char) i;
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

	if (count == 0 || count % 4) {
		return NULL;
	}

	olen = count / 4 * 3;
	if (olen > *out_len) {
		return NULL;
	}
	pos = out;
	if (out == NULL)
		return NULL;

	count = 0;
	for (i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		if (tmp == 0x80)
			continue;

		if (src[i] == '=')
			pad++;
		block[count] = tmp;
		count++;
		if (count == 4) {
			*pos++ = (block[0] << 2) | (block[1] >> 4);
			*pos++ = (block[1] << 4) | (block[2] >> 2);
			*pos++ = (block[2] << 6) | block[3];
			count = 0;
			if (pad) {
				if (pad == 1)
					pos--;
				else if (pad == 2)
					pos -= 2;
				else {
					/* Invalid padding */
					return NULL;
				}
				break;
			}
		}
	}
	//debug("i: %d len: %d", (int)i, (int)len);

	*out_len = pos - out;
	return out;
}

