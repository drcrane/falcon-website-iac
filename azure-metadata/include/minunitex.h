#ifndef MINUNITEX_H
#define MINUNITEX_H

#ifdef SHA256HELPER_H

static int minunitex_sha256hashmatch(char * path, char * match_hash) {
	char file_hash[65];
	sha256_file(path, file_hash);
	if (memcmp(file_hash, match_hash, 64) == 0) {
		return 1;
	}
	return 0;
}

#endif /* SHA256HELPER_H */

#endif /* MINUNITEX_H */

