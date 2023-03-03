#include "history.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
/*
struct typedef{
        char* filename;
        char* revision;
        time_t  timestamp;
}RevisionEntry;

struct typedef{
        RevisionHistory* history;
        char* file_absolute_path;
}History;
*/
char* get_history_path(char* abs_path)
{
	char* history_path = create_history_path(abs_path);
        history_path = (char*)realloc(history_path, strlen(history_path) + 9);
        char str[] = "/history";
        strcat(history_path, str);
	return history_path;
}
char* create_history_path(char* abs_path)
{
	// some function to make a unique folder for history by name
	// this function takes the file absolute path and make a new path
	// therefore a filename can be transformed into the remote path of
	// the file's repository
	char* abs_path_cpy = (char*)calloc(sizeof(char),strlen(abs_path)*sizeof(char));
	if (!abs_path_cpy)
	{
		perror("memory error create_history_path");
		return 0;
	}
	strcpy(abs_path_cpy, abs_path);
	char signs[] = "'.[]{}/\\ -";
	char c;
	int replaced = 0;
	for(int i=0; i< (int)strlen(abs_path_cpy); i++)
	{
		replaced = 0;
		c = abs_path_cpy[i];
		for(int j = 0; j < (int)strlen(signs) ; j++)
		{
			if (c == signs[j])
			{
				abs_path_cpy[i] = '_';
				replaced = 1;
				break;
			}
		}
		if (!replaced) // change to lower letter
		{
			if  (c <= 'Z' && c >= 'A') abs_path_cpy[i] = tolower(c);
		}
	}
	struct stat st = {0};
	if(stat(abs_path_cpy, &st) == -1)
        {
                printf("creating dir %s\n", abs_path_cpy);
                mkdir(abs_path_cpy, 0770);
        }
	return abs_path_cpy;
}

History* get_history(File* file)
{
	int perms = O_CREAT | O_RDWR;
	mode_t mode = 0666;
	char* history_path = get_history_path(file->absolutePath);

	//if (DEBUG)
		printf("opened history path\n");
	File* historyFile = open_file(history_path, perms, mode);
	// if failed open
	if (!historyFile)
	{
		perror("file error");
		return 0;
	}
	History* history = (History*)malloc(sizeof(History));
	history->fileName=NULL;
	history->absolutePath=NULL;
	history->history=NULL;
	if (!history)
	{
		perror("memory error");
		close_file(historyFile);
                return 0;
	}
	history->absolutePath = (char*)malloc(strlen(file->absolutePath)*sizeof(char));
	if (!history->absolutePath)
        {
                perror("memory error");
                close_file(historyFile);
		free(history);
                return 0;
        }
	history->fileName = (char*)malloc(strlen(file->relativePath)*sizeof(char));
	if (!history->fileName)
	{
		perror("memory error");
		free_history(history);
		close_file(historyFile);
		return 0;
	}
	strcpy(history->absolutePath, file->absolutePath);
	strcpy(history->fileName, file->relativePath);

	history->history = (RevisionEntry**)malloc(0);
	history->totalEntries = 0;
	if (historyFile->fileSize <= 1)
	{
		// set file initital data
		write_line(historyFile, file->absolutePath);
	}
	else
	{
		char* line = read_next_line(historyFile);
		char *token;
		while(!is_eof(historyFile))
		{
			line = read_next_line(historyFile);
			history->history = (RevisionEntry**)realloc(history->history, (history->totalEntries + 1)*sizeof(RevisionEntry*));
			RevisionEntry* entry = (RevisionEntry*)malloc(sizeof(RevisionEntry));
			if(!entry)
			{
				free(line);
				free_history(history);
				close_file(historyFile);
				return NULL;
			}
			token = strtok(line, " ");
			entry->revision = atof(token);
			token = strtok(NULL, "\n");
			tm tm;
			strptime(token, "%Y-%m-%d %H:%M:%S", &tm);
			entry->timestamp = mktime(&tm);
			history->history[history->totalEntries++] = entry;
			// strftime(str2, sizeof(str2), "%Y-%m-%d %H:%M:%S", tm);
		}
		free(line);
	}
	close_file(historyFile);
	return history;
}
RevisionEntry* get_revision(History* history, time_t timestamp)
{
	for(int i =0;i<history->totalEntries; i++)
	{
		if ( timestamp == history->history[i]->timestamp)
		{
			return history->history[i];
		}
	}
	return 0;
}
RevisionEntry* get_last_revision(History* history)
{
	if(!history->totalEntries)return NULL;
	return history->history[history->totalEntries-1];
}
int add_revision_entry(History* history)
{

	double val;
	RevisionEntry* entry = get_last_revision(history);
	if(entry)
	{
		val = entry->revision + 0.001;
	}
	else
	{
		val = 0.001;
	}
	char* str = (char*)malloc(sizeof(char)*6);
	sprintf(str, "%.4f", val);
	time_t t;
	t = time(NULL);

	// history path
	printf("%s\n", history->absolutePath);
	char* historyFilePath = get_history_path(history->absolutePath);
	File* historyFile = open_file(historyFilePath, O_RDWR | O_CREAT, 0666);

	// create line
	struct tm *tm = localtime(&t);
	char str2[20] = {0};
	strftime(str2, sizeof(str2), "%Y-%m-%d %H:%M:%S", tm);
	seek_file(historyFile,0,2);
	write_data(historyFile, str);
	char space[] = " ";
	write_data(historyFile, space);
	write_line(historyFile, str2);


	/*
	char filepath[] = "users/passwd";
        int perms = O_CREAT | O_RDWR;
        int mode = 0666;
        File* file = open_file(filepath, perms, mode);
        seek_file(file, 0, 2);
        printf("File opened \n");

        write_line(file, line);
        close_file(file);
	*/
	return 1;
}

void free_history(History* history)
{
	if(!history)return;
	if(history->history)
	{
		for(int i = 0; i<history->totalEntries; i++)
		{
			free(history->history[i]);
		}
	}
	if(history->fileName)
	{
		free(history->fileName);
	}
	if(history->absolutePath)
	{
		free(history->absolutePath);
	}
}
