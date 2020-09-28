#ifndef DYNSTRING_H
#define DYNSTRING_H

#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

#include <malloc.h>
#include <string.h>
#include "dbg.h"
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
};
#endif
typedef struct dynstring_context dynstring_context_t;

struct dynstring_context {
	char * buf;
	size_t size;
	size_t pos;
};

static int dynstring_initialisezero(dynstring_context_t * ctx) {
	ctx->buf = NULL;
	ctx->size = 0;
	ctx->pos = 0;
	return 0;
}

static int dynstring_initialise(dynstring_context_t * ctx, size_t size) {
	ctx->buf = (char *)malloc(size);
	if (ctx->buf == NULL) {
		return -1;
	}
	ctx->size = size;
	ctx->pos = 0;
	return 0;
}

static int dynstring_appendchar(dynstring_context_t * ctx, int c) {
	if (ctx->pos >= ctx->size) {
		char * newbuf = (char *)realloc(ctx->buf, ctx->size + 64);
		if (newbuf == NULL) {
			return -1;
		}
		ctx->buf = newbuf;
		ctx->size = ctx->size + 64;
	}
	ctx->buf[ctx->pos++] = c;
	return 0;
}

static int dynstring_appendstring(dynstring_context_t * ctx, const char * str, size_t str_len) {
	if (str_len == 0) {
		return 0;
	}
	if (str_len + ctx->pos >= ctx->size) {
		char * newbuf = (char *)realloc(ctx->buf, ctx->size + str_len);
		if (newbuf == NULL) {
			return -1;
		}
		ctx->buf = newbuf;
		ctx->size = ctx->size + str_len;
	}
	memcpy(ctx->buf + ctx->pos, str, str_len);
	ctx->pos += str_len;
	return 0;
}

static int dynstring_appendstringz_va(dynstring_context_t * ctx, va_list args) {
	char * str;
	size_t str_len;
	int rc;
	str = va_arg(args, char *);
	while (str != NULL) {
		str_len = strlen(str);
		rc = dynstring_appendstring(ctx, str, str_len);
		if (rc) { break; }
		str = va_arg(args, char *);
	}
	return rc;
}

static int dynstring_appendstringz(dynstring_context_t * ctx, ...) {
	int rc;
	va_list args;
	va_start(args, ctx);
	rc = dynstring_appendstringz_va(ctx, args);
	va_end(args);
	return rc;
}

static int dynstring_endswithchar(dynstring_context_t * ctx, int c) {
	return ctx->buf[ctx->pos - 1] == c;
}

static size_t dynstring_length(dynstring_context_t * ctx) {
	return ctx->pos;
}

static void dynstring_empty(dynstring_context_t * ctx) {
	ctx->pos = 0;
	if (ctx->buf) {
		ctx->buf[ctx->pos] = '\0';
	}
}

static void dynstring_free(dynstring_context_t * ctx) {
	if (ctx->buf) {
		free(ctx->buf);
		ctx->buf = NULL;
	}
	ctx->pos = 0;
	ctx->size = 0;
}

static int dynstring_compact(dynstring_context_t * ctx) {
	char * newbuf;
	size_t newsize;
	newsize = ctx->pos + 1;
	newbuf = (char *)realloc(ctx->buf, newsize);
	if (newbuf) {
		ctx->buf = newbuf;
		ctx->size = newsize;
		return 0;
	} else {
		return -1;
	}
}

static char * dynstring_getcstring(dynstring_context_t * ctx) {
	char * rawstring;
	rawstring = ctx->buf;
	if (ctx->size > ctx->pos && ctx->buf[ctx->pos] == '\0') {
		return rawstring;
	}
	dynstring_appendchar(ctx, '\0');
	ctx->pos --;
	return rawstring;
}

static char * dynstring_detachcstring(dynstring_context_t * ctx) {
	char * rawstring;
	dynstring_appendchar(ctx, '\0');
	dynstring_compact(ctx);
	rawstring = ctx->buf;
	ctx->buf = NULL;
	dynstring_free(ctx);
	return rawstring;
}

#endif /* DYNSTRING_H */


