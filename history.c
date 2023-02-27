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

char* create_history_path(char* abs_path)
{
	// some function to make a unique folder for history by name
	// this function takes the file absolute path and make a new path
	// therefore a filename can be transformed into the remote path of
	// the file's repository
	char* abs_path_cpy = (char*)malloc(strlen(abs_path)*sizeof(char));
	if (!abs_path_cpy)
	{
		perror("memory error create_history_path");
		return 0;
	}
	strcpy(abs_path_cpy, abs_path);
	char signs[] = "'./[]{}\\ -";
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
	return abs_path_cpy;
}

History* get_history(File* file)
{
	int perms = O_CREAT | O_RDWR;
	mode_t mode = 0666;
	char* history_path = create_history_path(file->absolutePath);

	File* historyFile = open_file(history_path, perms, mode);
	// if failed open
	if (!historyFile)
	{
		perror("file error");
		return 0;
	}
	History* history = (History*)malloc(sizeof(History));
	if (!history)
	{
		perror("memory error");
		close_file(historyFile);
                return 0;
	}
	history->file_absolute_path = (char*)malloc(strlen(file->absolutePath)*sizeof(char));
	if (!history->file_absolute_path)
        {
                perror("memory error");
                close_file(historyFile);
		free(history);
                return 0;
        }
	strcpy(history->file_absolute_path, file->absolutePath);

	history->history = (RevisionEntry**)malloc(0);
	history->totalEntries = 0;
	if (historyFile->fileSize <= 1)
	{
		// set file initital data
		write_line(historyFile, history_path);
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
			token = strtok(line, " ");
			entry->filename = (char*)malloc(strlen(token)*sizeof(char));
			strcpy(entry->filename, token);
			token = strtok(NULL, " ");
			entry->revision = strtod(token, NULL);
			history->history[history->totalEntries++] = entry;
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
	return history->history[history->totalEntries-1];
}
int add_revision_entry(History* history)
{
	double val = get_last_revision(history)->revision + 0.001;
	char* str = (char*)malloc(sizeof(6));
	sprintf(str, "%.4f", val);
	time_t t;
	t = time(NULL);
	// history path
	printf("%s\n", history->file_absolute_path);
	char* historyFilePath = create_history_path(history->file_absolute_path);
	File* historyFile = open_file(historyFilePath, O_RDWR | O_CREAT, 0666);
	struct tm *tm = localtime(&t);
	char str2[20] = {0};
	strftime(str2, sizeof(str2), "%Y-%m-%d %H:%M:%S", tm);
	printf("Value: %ld, String: %s\n", (long) t, str);
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
