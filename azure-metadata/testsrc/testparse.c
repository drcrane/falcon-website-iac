#include <stdio.h>
#include <stdlib.h>

#include "dynstring.h"
#include "filesystem.h"
#include "json.h"

int main(int argc, char *argv[]) {
	char * jsonbuffer;
	size_t jsonbuffer_sz;
	int res;
	JsonNode * rootNode, * valueNode, * tokenNode;
	res = filesystem_loadintoram("testdata/jsontoparse.json", (void **)&jsonbuffer, &jsonbuffer_sz);
	if (res) {
		fprintf(stderr, "file load failed\n");
		return 1;
	}
	rootNode = json_decode(jsonbuffer);
	if (rootNode == NULL) {
		fprintf(stderr, "failed to parse buffer\n");
		return 1;
	}
	free(jsonbuffer);
	//fprintf(stdout, "%d\n", (int)jsonbuffer_sz);
	valueNode = json_find_member(rootNode, "value");
	if (valueNode->tag == JSON_STRING) {
		fprintf(stdout, "%s\n", valueNode->string_);
	}
	json_delete(rootNode);

	res = filesystem_loadintoram("testdata/metadatatoken.json", (void **)&jsonbuffer, &jsonbuffer_sz);
	if (res) {
		fprintf(stderr, "file load failed\n");
		return 1;
	}
	rootNode = json_decode(jsonbuffer);
	free(jsonbuffer);
	tokenNode = json_find_member(rootNode, "access_token");
	if (valueNode->tag == JSON_STRING) {
		fprintf(stdout, "%s\n", tokenNode->string_);
	} else {
		fprintf(stderr, "No access token found!\n");
		return -1;
	}
	return 0;
}

