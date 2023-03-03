#include "lock.h"
#include <stdio.h>
#include <string.h>
/*
typedef struct{
        char* user;
        time_t timestamp;
}Lock;
*/
void create_lock(File* file, RevisionEntry* revision, char* user)
{
	char* abs_path = file->absolutePath;
	char* history_path = create_history_path(abs_path);
	char* lock_path = (char*)calloc(sizeof(char), sizeof(char)*strlen(history_path)+6);
	strcpy(lock_path, history_path);
	char str[] = "/lock";
	strcat(lock_path, str);
	if(!lock_path)
	{
		perror("memory error");
	}
	printf("path: %s\n",lock_path);
	File* stub = open_file(lock_path, O_CREAT | O_RDWR, 0666);

	if(!stub)
	{
		free(lock_path);
		perror("file openning error");
		return;
	}
	write_data(stub, file->relativePath);
	char space[] = " ";
	write_data(stub, space);
	write_data(stub, user);
	write_data(stub, space);
	char time[20];
	strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&revision->timestamp));
	write_line(stub, time);
	return;
}
void create_local_key(File* file, RevisionEntry* revision, char* user)
{

}

int is_locked(File* file)
{
	return 0;
}

int unlock_file(File* file, time_t time, char* user)
{
	return 0;
}
