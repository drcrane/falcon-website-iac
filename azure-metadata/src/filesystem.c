#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"
//#include "timefn.h"

#define LINUX

#ifdef LINUX
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

int filesystem_fileexists(char * path) {
	return 0;
}

int filesystem_isfileolderthan(char * path, int seconds) {
	struct stat fstat;
	int rc;
	rc = lstat(path, &fstat);
	if (rc == -1) {
		return -1;
	}
	if (fstat.st_mtime < (timefn_getcurrentunixtime() - seconds)) {
		return 1;
	}
	return 0;
}

int filesystem_saveramtofile(char * path, void * data, size_t size) {
	int fd;
	ssize_t wrsz;
	fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		return -1;
	}
	wrsz = write(fd, data, size);
	close(fd);
	if (wrsz != size) {
		unlink(path);
		return -1;
	}
	return 0;
}

int filesystem_loadintoram(char * path, void ** data_ptr, size_t * data_sz_ptr) {
	void * data;
	ssize_t data_sz;
	struct stat file_stat;
	int rc;
	int fd = -1;
	rc = lstat(path, &file_stat);
	if (rc == -1 || S_ISDIR(file_stat.st_mode)) {
		return -1;
	}
	data = malloc(file_stat.st_size + 1);
	if (data == NULL) {
		return -1;
	}
	fd = open(path, O_RDONLY);
	if (fd == -1) {
		goto error;
	}
	data_sz = read(fd, data, file_stat.st_size);
	if (data_sz == -1) {
		goto error;
	}
	*(((char *)data) + data_sz) = '\0';
	close(fd);
	*data_ptr = data;
	*data_sz_ptr = (size_t)data_sz;
	return 0;
error:
	if (data) {
		free(data);
	}
	if (fd != -1) {
		close(fd);
	}
	return -1;
}

char * filesystem_sanitisedir(char * path) {
	size_t path_len;
	char * sane_path;
	path_len = strlen(path);
	if (path_len == 0) {
		return strdup(path);
	}
	if (path[path_len - 1] == '/') {
		return strdup(path);
	}
	sane_path = malloc(path_len + 2);
	if (sane_path == NULL) {
		return NULL;
	}
	memcpy(sane_path, path, path_len);
	sane_path[path_len] = '/';
	path_len ++;
	sane_path[path_len] = '\0';
	return sane_path;
}

int filesystem_recursedirectories(char * path, filesystem_callback fscb, void * user_data) {
	DIR * dir;
	struct dirent * entry;
	struct stat status;
	int rc;
	int res = -1;
	size_t path_len;
	char * path_end;

	path_len = strlen(path);
	path_end = path + (path_len - 1);
	if (*path_end == '/') {
		*path_end = '\0';
	}
	if (!(dir = opendir(path))) {
		return res;
	}
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_DIR) {
			size_t path_len;
			char * path_pos;
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}
			path_len = strlen(path);
			path_pos = path + path_len;
			*path_pos = '/';
			strcpy(path_pos + 1, entry->d_name);
			rc = lstat(path, &status);
			if (rc == -1) {
				continue;
			}
			// not following directory symbolic links for now.
			if (S_ISLNK(status.st_mode)) {
				continue;
			}
			rc = filesystem_recursedirectories(path, fscb, user_data);
			*path_pos = '\0';
			if (rc) {
				res = rc;
				goto fs_rd_end;
			}
		} else {
			path_len = strlen(path);
			size_t filename_len = strlen(entry->d_name);
			char * end_of_path;
			end_of_path = path + path_len;
			*end_of_path = '/';
			if ((path_len + 1 + filename_len + 1) >= FILESYSTEM_PATH_MAXLEN) {
				res = -1;
				goto fs_rd_end;
			}
			strcpy(end_of_path + 1, entry->d_name);
			//fprintf(stdout, "%s\n", path);
			rc = fscb(user_data, path);
			*end_of_path = '\0';
			if (rc) {
				res = rc;
				goto fs_rd_end;
			}
		}
	}
	res = 0;
fs_rd_end:
	closedir(dir);
	return res;
}
#endif

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
/*
 * StackOverflow:
 * https://stackoverflow.com/questions/2038912/how-to-recursively-traverse-directories-in-c-on-windows
*/

static void FindFilesRecursively(LPCTSTR lpFolder, LPCTSTR lpFilePattern) {
	TCHAR szFullPattern[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;
	// first we are going to process any subdirectories
	PathCombine(szFullPattern, lpFolder, _T("*"));
	hFindFile = FindFirstFile(szFullPattern, &FindFileData);
	if(hFindFile != INVALID_HANDLE_VALUE) {
		do {
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// found a subdirectory; recurse into it
				PathCombine(szFullPattern, lpFolder, FindFileData.cFileName);
				FindFilesRecursively(szFullPattern, lpFilePattern);
			}
		} while(FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}

	// Now we are going to look for the matching files
	PathCombine(szFullPattern, lpFolder, lpFilePattern);
	hFindFile = FindFirstFile(szFullPattern, &FindFileData);
	if(hFindFile != INVALID_HANDLE_VALUE) {
		do {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				// found a file; do something with it
				PathCombine(szFullPattern, lpFolder, FindFileData.cFileName);
				_tprintf_s(_T("%s\n"), szFullPattern);
			}
		} while(FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}
}


void filesystem_recursedirectories(char * name) {
	HANDLE directory;
}
#endif
