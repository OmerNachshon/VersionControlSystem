#include "files.h"
#include <stdio.h>
#include <string.h>

#define DEBUG 0

File* open_file(char *path, int flags, mode_t mode)
{

        File* file = (File*)malloc(sizeof(File));
        file->fd = open(path, flags, mode);           
        if (file->fd == -1) {
                perror("error openning file");
                return 0;
        }

        file->absolutePath = (char*)malloc(MAX_PATH*2*(sizeof(char)));
        if ( !realpath(path, file->absolutePath) ) { perror("realpath error"); }

        file->relativePath = (char*)malloc(MAX_PATH*(sizeof(char)));

	if(!file->relativePath)
	{
		perror("memory error");
	}
        strcpy(file->relativePath,path);

        file->mode = mode;
        file->seek = 0;
        struct stat st;
        if (fstat(file->fd, &st) == -1) {    //here is the error
                perror("open file fstat");
                return 0;
        }
        file->fileSize = st.st_size;
        return file;
}

char** read_lines(File* file, int* size)  // check read_lines
{
        int current_size = 0;
        char** lines = (char**)malloc(current_size*sizeof(char*));
        if ( lines == 0 ) { perror("read lines malloc"); }
        if ( DEBUG == 1 )
                printf("On enter\n");
        while( !is_eof(file) )
        {
                char* line = read_next_line(file);
                if ( DEBUG == 1 )
                        printf("On iter %d: ", current_size+1);
                lines = (char**)realloc(lines, (current_size+1)*sizeof(char*));
                if ( DEBUG == 1 )
                        printf("line: %s\n", line);
                lines[current_size++] = line;
        }

        if ( DEBUG == 1 )
                printf("On assign size\n");
        *size = current_size;
        if ( DEBUG == 1 )
                printf("On exit\n");

        if (current_size == 0) { return NULL; }
        return lines;
}
char* read_next_line(File* file)
{
        // todo - check permissions
        char* line = (char*)malloc(CHUNK*sizeof(char));
        if ( line == 0 )
                perror("read_next_line malloc");
        if ( DEBUG )
                printf("in read_next_line\n");
        int total = CHUNK;
        int size = 0;
        char c;
        if ( DEBUG )
                printf("before loop\n");
        while( !is_eof(file))
        {
		if (read(file->fd, &c, 1) == 0)
			perror("read char\n");
                if ( DEBUG == 1 )
                        printf("in loop %d iter, char %d \n", size+1, c);
                file->seek += 1;
                if ( (size + 1) % (CHUNK + 1)  == 0 )
                {
                        line = (char*) realloc(line, total + 1 + CHUNK );
                        total += 1 + CHUNK;
                }
                line[size++] = c;
                if ( c == '\n' ) break;
        }
        if (size == 0)
        {
                return 0;
        }
        return line;
}

int write_line(File* file, char* string)
{
        int size = strlen(string);
        char* new_str = (char*)calloc(sizeof(char),size+1);
        strcpy(new_str, string);
        new_str[size] = '\n';
        int total = write(file->fd, new_str, size+1);
        free(new_str);
	file->seek += size + 1;
        return total;
}
int write_data(File* file, char* string)
{
	file->seek += strlen(string);
        return write(file->fd, string, strlen(string));
}

int seek_file(File* file, int offset, int whence)
{
        int result = -1;
        int fd = file->fd;
        if ( whence < 0 || whence > 2 )
        {
                perror("seek whence = {0,1,2}");
                return result;
        }
        result = lseek(fd, offset, whence);
        if ( result < 0 || result > file->fileSize )
        {
                perror("seek overflow");
                return -1;
        }
        file->seek = result;
        return result;
}
int close_file(File* file)
{
        int result = close(file->fd);
        if (result == -1) perror("close file");
        free(file);
        return 0;
}
int is_eof(File* file)
{
	if (DEBUG)
		printf("fileSize: %lld, seek: %d\n",  file->fileSize, file->seek);
        return file->fileSize <= file->seek;
}
