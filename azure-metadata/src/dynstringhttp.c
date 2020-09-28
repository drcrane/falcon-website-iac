#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <curl/curl.h>
#include <pthread.h>

#include "dynstring.h"
#include "timefn.h"
#include "dbg.h"
#include "dynstringhttp.h"

static int dynstringhttp_writedatatodynstring(void * ptr, size_t size, size_t nmemb, void * userdata) {
	int res;
	// This filemanager is for small files.
	if ((dynstring_length((dynstring_context_t *)userdata) + (size * nmemb)) > 8192 * 4) {
		return 0;
	}
	res = dynstring_appendstring((dynstring_context_t *)userdata, (const char *)ptr, size * nmemb);
	if (res == 0) {
		return size * nmemb;
	}
	return 0;
}

int dynstringhttp_appendfromurl(CURL * curl, dynstring_context_t * dynstring, char * url) {
	CURLcode c_res;
	size_t written;
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dynstringhttp_writedatatodynstring);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, dynstring);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	c_res = curl_easy_perform(curl);
	if (c_res != CURLE_OK) {
		return -1;
	}
	return 0;
}


