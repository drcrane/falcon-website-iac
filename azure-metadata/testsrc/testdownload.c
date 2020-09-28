#include <stdio.h>
#include <stdlib.h>

#include "dynstring.h"
#include "dynstringhttp.h"

// curl -H 'Metadata: true' 'http://169.254.169.254/metadata/identity/oauth2/token?api-version=2018-02-01&resource=https://vault.azure.net'

int main(int argc, char * argv[]) {
	CURL * curl;
	dynstring_context_t file_buffer;
	struct curl_slist *header_list = NULL;
	int res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (curl == NULL) {
		fprintf(stdout, "Failed to initialise CURL\n");
		return 1;
	}
	dynstring_initialise(&file_buffer, 8192);
	header_list = curl_slist_append(header_list, "Metadata: true");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
	res = dynstringhttp_appendfromurl(curl, &file_buffer, "https://bengreen.eu/");
	if (res) {
		fprintf(stderr, "failed to download file\n");
		return 1;
	}
	curl_slist_free_all(header_list);
	header_list = NULL;

	fprintf(stdout, "%s", dynstring_getcstring(&file_buffer));

	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return 0;
}

