/*
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>

int base64_decode_ex(char * src, size_t src_len, char * dst, size_t * dst_len);

size_t base64_reqbufsz(size_t lentoencode);
unsigned char * base64_encode(const unsigned char *src, size_t len,
			      unsigned char *out, size_t *out_len);
int base64_decode_forgiving(const unsigned char * src, size_t src_len,
		unsigned char *out, size_t *out_len);
unsigned char * base64_decode(const unsigned char *src, size_t len,
			      unsigned char *out, size_t *out_len);

#endif /* BASE64_H */

