#ifndef __FILES__
#define __FILES__

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <stdlib.h>

#define MAX_PATH 63
#define CHUNK 63

typedef struct
{
	int fd;
	char* absolutePath;
	char* relativePath;
	mode_t mode;
	int seek;
	long long fileSize;
}File;

File* open_file(char *path, int flags, mode_t mode);
char** read_lines(File* file, int* size);
char* read_next_line(File* file);
int write_line(File* file, char* string);
int write_data(File* file, char* string);
int seek_file(File* file, int offset, int whence);
int close_file(File* file);
int is_eof(File* file);

#endif
