#include "files.h"
#include <stdio.h>

int main()
{
	// file open
	char filepath[] = "txt";
	File* file = open_file(filepath,0,0);
	printf("== open file ==\n");
	printf("fd is %d\n", file->fd);
	printf("Absolute path is: %s\n", file->absolutePath);
	printf("Relative path is: %s\n", file->relativePath);
	printf("Mode is: %d\n", file->mode);
	printf("Seek is: %d\n", file->seek);
	printf("Filesize is: %lld\n", file->fileSize);

	// read next line
	printf("== read first line ==\n");
	printf("First line is: \n");
	char* line = read_next_line(file);
	printf("%s", line);

	// read second line
	printf("== read second line ==\n");
	printf("Second line is: \n");
	line = read_next_line(file);
        printf("%s", line);

	// no third line - check line == NULL

	printf("== read third line ==\n");
	printf("Third line is: \n");
	line = read_next_line(file);
        if ( line == 0 ) printf("EOF\n");
	else printf("%s", line);

	// no fourth line - condition is_eof

	printf("== read fourth line ==\n");
	printf("Fourth line is: \n");
	if ( !is_eof(file) )
	{
		// printf("in eof\n");
		line = read_next_line(file);
	        //printf("%s\n", line);
	}else{
		printf("EOF\n");
	}

	// file close
	printf("== close file ==\n");
	close_file(file);
	printf("File is closed\n");

	// open file again
	printf("== read all the lines with read_lines ==\n");
	printf("Openning file txt\n");
	file = open_file(filepath,0,0);
	// reading lines
	printf("reading lines\n");

	int size = 0;
	char** lines = read_lines(file, &size);
	printf("%d\n", size);
	for(int i = 0; i < size; i ++ ){
		printf("line %2d: %s", (i+1), lines[i]);
	}

	// closing file
	close_file(file);
        printf("File is closed\n");


	// read files is loop - using read_next_line and is_eof
	printf("== read lines with read_next_line and is_eof ==\n");
        // open file again
        printf("Openning file txt\n");
        file = open_file(filepath,0,0);
	int j = 1;
	while(!is_eof(file))
	{
		line = read_next_line(file);
		printf("line %2d: %s", j, line);
		j++;
	}

	// close file
        close_file(file);
        printf("File is closed\n");

	// write to file - no permissions
	printf("== write with read mode and no permissions ==\n");
	char str[] = "ghijk";
	file = open_file(filepath,0,0);
	int i = write_line(file, str);
	printf("number of chars %d\n", i);

	printf("== write with in rw mode and 666 perms ==\n");
	int read_write = O_RDWR | O_CREAT;
	int mode = 0666;
	file = open_file(filepath,read_write,mode);
	printf("file opened with rw+create, mode rw for e1\n");
	i = write_line(file, str);
	printf("number of chars %d\n", i);

	// close file
        close_file(file);
        printf("File is closed\n");


	// append to file
	printf("== write with in a mode and 666 perms ==\n");
	read_write = O_WRONLY | O_CREAT | O_APPEND;
	mode = 0666;
        file = open_file(filepath,read_write,mode);
        printf("file opened with flags append, mode rw for e1\n");
	char str2[] = "jklmn";
        i = write_line(file, str2);
        printf("number of chars %d\n", i);

	// close file
        close_file(file);
        printf("File is closed\n");

	// seek 5 forward
	printf("== seek 6 from the start ==\n");
	file = open_file(filepath, O_RDWR | O_CREAT, mode);
	int result = seek_file(file, 6, 1);
	printf("Seek result: %d\n", result);
	printf("File seek: %d\n", file->seek);
	line = read_next_line(file);
	printf("%s", line);
	 // close file
        close_file(file);
        printf("File is closed\n");

	// seek -6 from the end
	printf("== seek 6 from the end ==\n");
	file = open_file(filepath, O_RDWR | O_CREAT, mode);
        result = seek_file(file, -6, 2);
        printf("Seek result: %d\n", result);
        printf("File seek: %d\n", file->seek);
        line = read_next_line(file);
        printf("%s", line);
         // close file
        close_file(file);
        printf("File is closed\n");

	// seek 6 from the second line
        printf("== seek 6 after reading a line ==\n");
        file = open_file(filepath, O_RDWR | O_CREAT, mode);
	line = read_next_line(file);
        result = seek_file(file, 6, 1);
        printf("Seek result: %d\n", result);
        printf("File seek: %d\n", file->seek);
        line = read_next_line(file);
        printf("%s", line);
         // close file
        close_file(file);
        printf("File is closed\n");
	return 0;
}
