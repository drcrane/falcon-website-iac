#ifndef DYNSTRINGHTTP_H
#define DYNSTRINGHTTP_H

#include <curl/curl.h>
#include "dynstring.h"

int dynstringhttp_appendfromurl(CURL * curl, dynstring_context_t * dynstring, char * url);

#endif /* DYNSTRINGHTTP_H */

