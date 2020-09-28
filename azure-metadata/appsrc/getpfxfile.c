#include <stdio.h>
#include <stdlib.h>

#include "dynstring.h"
#include "dynstringhttp.h"
#include "json.h"
#include "filesystem.h"

char * getmetadatatoken(CURL * curl) {
	dynstring_context_t buf, hdr;
	int res;
	struct curl_slist *header_list = NULL;
	char * auth_hdr = NULL;
	JsonNode * rootNode = NULL, * accessToken = NULL;
	buf.buf = NULL;
	hdr.buf = NULL;
	res = dynstring_initialise(&buf, 8192);
	if (res) { goto finish; }
	res = dynstring_initialise(&hdr, 4096);
	if (res) { goto finish; }
	header_list = curl_slist_append(header_list, "Metadata: true");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
	res = dynstringhttp_appendfromurl(curl, &buf, "http://169.254.169.254/metadata/identity/oauth2/token?api-version=2018-02-01&resource=https://vault.azure.net");
	if (res) { goto finish; }
	rootNode = json_decode(dynstring_getcstring(&buf));
	if (rootNode == NULL) { goto finish; }
	accessToken = json_find_member(rootNode, "access_token");
	if (accessToken == NULL || accessToken->tag != JSON_STRING) { goto finish; }
	res = dynstring_appendstringz(&hdr, "Authorization: Bearer ", accessToken->string_, NULL);
	if (res) { goto finish; }
	auth_hdr = dynstring_detachcstring(&hdr);
finish:
	if (rootNode) {
		json_delete(rootNode);
	}
	if (header_list) {
		curl_slist_free_all(header_list);
	}
	dynstring_free(&hdr);
	dynstring_free(&buf);
	return auth_hdr;
}

char * getpfxfile(CURL * curl, char * auth_header) {
	dynstring_context_t buf, hdr;
	int res;
	struct curl_slist *header_list = NULL;
	char * b64_str = NULL;
	JsonNode * rootNode = NULL, * valueNode = NULL;
	buf.buf = NULL;
	hdr.buf = NULL;
	res = dynstring_initialise(&buf, 8192);
	if (res) { goto finish; }
	res = dynstring_initialise(&hdr, 4096);
	if (res) { goto finish; }
	header_list = curl_slist_append(header_list, auth_header);
	header_list = curl_slist_append(header_list, "Accept: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
	res = dynstringhttp_appendfromurl(curl, &buf, "https://falcon-website-keyvault.vault.azure.net/secrets/bengreen-eu-ssl?api-version=2016-10-01");
	if (res) { goto finish; }
	rootNode = json_decode(dynstring_getcstring(&buf));
	if (rootNode == NULL) { goto finish; }
	valueNode = json_find_member(rootNode, "value");
	if (valueNode == NULL || valueNode->tag != JSON_STRING) { goto finish; }
	dynstring_appendstringz(&hdr, valueNode->string_, NULL);
	if (res) { goto finish; }
	b64_str = dynstring_detachcstring(&hdr);
finish:
	if (rootNode) {
		json_delete(rootNode);
	}
	if (header_list) {
		curl_slist_free_all(header_list);
	}
	dynstring_free(&hdr);
	dynstring_free(&buf);
	return b64_str;
}

int main(int argc, char * argv[]) {
	CURL * curl;
	char * auth_hdr = NULL;
	char * pfx_b64 = NULL;
	int res = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (curl == NULL) {
		fprintf(stdout, "Failed to initialise CURL\n");
		return 1;
	}
	auth_hdr = getmetadatatoken(curl);
	if (auth_hdr == NULL) {
		res = 1;
		goto finish;
	}
	//fprintf(stdout, "%s\n", auth_hdr);
	pfx_b64 = getpfxfile(curl, auth_hdr);
	free(auth_hdr);
	auth_hdr = NULL;

	if (pfx_b64 == NULL) {
		res = 1;
		goto finish;
	}

	fprintf(stdout, "%s\n", pfx_b64);
	free(pfx_b64);

finish:
	if (auth_hdr) {
		free(auth_hdr);
	}
	if (curl) {
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return res;
}

