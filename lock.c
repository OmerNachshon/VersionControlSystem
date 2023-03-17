#include "lock.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
	File* stub = open_file(lock_path, O_CREAT | O_RDWR, 0666);

	if(!stub)
	{
		free(lock_path);
		perror("file openning error");
		return;
	}
	char str2[10] = {0};
	sprintf(str2, "%f", revision->revision);
	write_data(stub, str2);
	char space[] = " ";
	write_data(stub, space);
	write_data(stub, user);
	write_data(stub, space);
	char time[20];
	strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&revision->timestamp));
	write_line(stub, time);

	free(history_path);
	free(lock_path);
	close_file(stub);
	return;
}

void create_local_key(File* file, RevisionEntry* revision, char* user)
{
        char* lock_path = (char*)calloc(sizeof(char), 7*sizeof(char));
        char str[] = "./lock";
        strcat(lock_path, str);
        if(!lock_path)
        {
                perror("memory error");
        }
        File* stub = open_file(lock_path, O_CREAT | O_RDWR, 0666);

        if(!stub)
        {
                free(lock_path);
                perror("file openning error");
                return;
        }
	char str2[10] = {0};
        sprintf(str2, "%f", revision->revision);
        write_data(stub, str2);
        char space[] = " ";
        write_data(stub, space);
        write_data(stub, user);
        write_data(stub, space);
        char time[20];
        strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&revision->timestamp));
        write_line(stub, time);

	free(lock_path);
	close_file(stub);

        return;
}

int is_locked(File* file)
{
	char* abs_path = file->absolutePath;
	char* history_path = create_history_path(abs_path);
	char* lock_path = (char*)calloc(sizeof(char), sizeof(char)*strlen(history_path)+6);
        if(!lock_path)
        {
                perror("memory error");
        }
	strcpy(lock_path, history_path);
        char str[] = "/lock";
        strcat(lock_path, str);

	// File* lockFile = open_file(lock_path, O_RDWR , 0666);	// needs to be deleted , access below solves the error
	//  if (!lockFile)
	//  {
	//  	free(history_path);
	//  	free(lock_path);
	//  	return 0;
	//  }
	if (access(lock_path, F_OK) == 0) 
	{
		free(history_path);
		free(lock_path);
		return 1;
	}
	free(lock_path);
	free(history_path);
	//close_file(lockFile);
	return 0;
}
int unlock_file(File* file, char* user)
{
	if(!is_locked(file)) return 0;

        char* abs_path = file->absolutePath;
        char* history_path = create_history_path(abs_path);
        char* lock_path = (char*)calloc(sizeof(char), sizeof(char)*strlen(history_path)+6);
        if(!lock_path)
        {
                perror("memory error");
        }
        strcpy(lock_path, history_path);
        char str[] = "/lock";
        strcat(lock_path, str);
        File* lockFile = open_file(lock_path, O_RDWR , 0666);

	if(!lockFile)
	{
		perror("error open file");
	}
	Lock lock;
	char* line = read_next_line(lockFile);
	char* token;
	token = strtok(line, " ");
	lock.revision = atof(token);
	token = strtok(NULL, " ");
	lock.user = (char*)calloc(sizeof(char), strlen(token));
	strcpy(lock.user, token);
	token = strtok(NULL, "\n");
	tm tm;
	strptime(token, "%Y-%m-%d %H:%M:%S", &tm);
	lock.timestamp = mktime(&tm);
	printf("lock data: %f, %s, %ld\n", lock.revision, lock.user, lock.timestamp);
	char str2[] = "./lock";
	File* keyFile = open_file(str2, O_RDWR, 0666);
	Lock key;
        line = read_next_line(keyFile);
        token = strtok(line, " ");
        key.revision = atof(token);

        token = strtok(NULL, " ");
        key.user = (char*)calloc(sizeof(char), strlen(token));
        strcpy(key.user, token);
        token = strtok(NULL, "\n");
        strptime(token, "%Y-%m-%d %H:%M:%S", &tm);
        key.timestamp = mktime(&tm);
        printf("key data: %f, %s, %ld\n", key.revision, key.user, key.timestamp);

	close_file(keyFile);
	close_file(lockFile);
	free(line);
	if ( !(key.revision == lock.revision && !strcmp(key.user, lock.user) && key.timestamp == lock.timestamp) )
	{
		free(lock_path);
		return 0;
	}
	// files matches
	printf("matching lock and key\n");
	if( remove(lock_path) == 0 )printf("remove success\n"); else printf("remove failed\n");
	if( remove(str2) == 0 )printf("remove success\n"); else printf("remove failed\n");
	free(lock_path);
	return 1;
}
int has_local_key(File* file)
{
	return 0;
}
