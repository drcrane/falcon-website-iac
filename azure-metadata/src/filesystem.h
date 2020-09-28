#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define FILESYSTEM_PATH_MAXLEN 1024

int filesystem_fileexists(char * path);
/*
 * If the specified file is not found return value is -1
 * If the specified file is older than `seconds` return value is 1
 * If the specified file is younger than `seconds` return value is 0
 */
int filesystem_isfileolderthan(char * path, int seconds);

int filesystem_saveramtofile(char * path, void * data, size_t size);
int filesystem_loadintoram(char * path, void ** data_ptr, size_t * data_sz_ptr);

/*
 * Create a path we can append to
 * if the path is an empty string, a new empty string will be returned.
 * if the path has a trailing / then a copy will be returned
 * if the path does not have a trailing / then a copy with a trailing / will be returned.
 * in the case of error NULL will be returned.
 */
char * filesystem_sanitisedir(char * path);

/*
 * Callback function for filesystem_recursedirectories
 * return value must be 0 to move on to next file any other
 * value will cause filesystem_recursedirectories to stop
 * processing and return the value returned from here.
 */
typedef int (*filesystem_callback)(void * user_data, char * path);

int filesystem_recursedirectories(char * name, filesystem_callback fscb, void * user_data);

#endif // FILESYSTEM_H

