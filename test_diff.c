#include "files.h"
#include "diff.h"
#include <stdio.h>

int main()
{
	// diff files
	printf("== diff 2 files ==\n");
	char filepath[] = "txt";
	File* file = open_file(filepath,O_CREAT | O_RDONLY, 0666);
	char filepath2[] = "sec";
	File* sec = open_file(filepath2,O_CREAT | O_RDONLY,0666);
	printf("File 1 fd: %d\n", file->fd);
	printf("File 2 fd: %d\n", sec->fd);

	diff(file, sec);
	return 0;

}
