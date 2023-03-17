#include "files.h"
#include "history.h"
#include "lock.h"
#include <stdio.h>
#include <getopt.h>
#include "diff.h"
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

void show_help();
void process_arguments(int argc, char *argv[], char* rev, char* pass, int* lock_co, int* var_ind);
void login();
void checkin();
void checkout();
void diff2();
char* create_user_entry(char* username, char* groups[], int size);
int add_user(char* username, char* groups[], int size);
int update_user(char* username, char* groups[], int size);
int is_user_exists(char* username);
void remove_user(char* username);
int is_user_logged_in();
char* get_user();

const char* commands[] = {"login", "checkin", "checkout", "diff"};
void (*functions[4])() = {login, checkin, checkout, diff2};

static char* first_arg = (char*)malloc(0);
static char* rev = (char*)malloc(0);
static char* pass = (char*)malloc(0);
static char* lock_str = (char*)malloc(0);
static int lock_co = 0;


int main(int argc, char* argv[])
{
	if ( argc == 1 )
	{
		printf("The use of the command is incorrect, use: vcom [options <command>] <command>");
		show_help();
		return 0;
	}

	int var_ind = 0;

	process_arguments(argc, argv, rev, pass, &lock_co, &var_ind);
	if ( lock_co && strcmp(lock_str, "lock"))
	{
		printf("The use of the command is incorrect, use: vcom [option <command>] <command>");
		show_help();
                return 0;
	}
	if ( var_ind + 3 != argc )
	{
		printf("The use of the command is incorrect, use: vcom [option <command>] <command>");
                show_help();
                return 0;
	}
	char* command = argv[var_ind+1];
	first_arg = argv[var_ind + 2];

	int command_found = 0;
	for (int i = 0; i<(int)(sizeof(commands)/sizeof(char*)); i++)
	{
		if (!strcmp(commands[i], command))
		{
			functions[i]();
			command_found = 1;
			break;
		}
	}
	if (!command_found)
	{
		printf("Invalid command given\n");
		show_help();
	}
	/*
	// test add user
	char** groups = (char**)malloc(4*sizeof(char*));
	char group1[] = "user";
	char group2[] = "sw2";
	groups[0] = group1;
	groups[1] = group2;
	add_user(first_arg, groups, 2);
	*/
	return 0;
}

void process_arguments(int argc, char *argv[], char* str, char* pass, int* lock_co, int* var_ind) {
	int opt;

	while ((opt = getopt(argc, argv, "r:p:l:")) != -1) {
		switch (opt) {
		case 'r':
			strcpy(str, optarg);
			*(var_ind) = *(var_ind) + 2;
			break;
		case 'p':
			strcpy(pass, optarg);
			*(var_ind) = *(var_ind) + 2;
			break;
		case 'l':
			*(lock_co) = 1;
			strcpy(lock_str, optarg);
			strcpy(pass, optarg);
			*(var_ind) = *(var_ind) + 2;
			break;
		default:
			show_help();
			exit(EXIT_FAILURE);
		}
	}
}

void show_help()
{
	char filepath[] = "help";
	File* file = open_file(filepath, O_CREAT | O_RDONLY, 0666);
	while( !is_eof(file) )
		printf("%s", read_next_line(file));
	close_file(file);
}

void login()
{
	if (!strlen(first_arg))
	{
		show_help();
		return;
	}
	printf("Making login command:\n");
	printf("Username: %s\n", first_arg);
	if (!strlen(pass))
	{
		printf("Insert password: \n");
		pass = (char*)malloc(CHUNK*sizeof(char));
		fgets(pass, CHUNK, stdin);
	}
	printf("Password: %s\n", pass);
	// todo - check admin
	char users_path[] = "users/passwd";
	int perms = O_CREAT | O_RDWR;
	mode_t mode = 0666;
	File* file = open_file(users_path, perms, mode);
	int is_connected = 0;
	char* line;
	char* token;

	while(!is_eof(file))
	{
		line = read_next_line(file);
		char *linecpy = (char*)malloc(sizeof(char)*strlen(line));
		strcpy(linecpy, line);
		token = strtok(line, ":");
		if(!strcmp(token, first_arg))
		{
			printf("%s connected\n", token);
			is_connected = 1;
			linecpy[strlen(linecpy)-2] = 'y';
		}
		else
		{
			linecpy[strlen(linecpy)-2] = 'x';
		}
		seek_file(file, (-1)*strlen(linecpy), 1);
		write_data(file, linecpy);
		free(linecpy);
	}
	free(line);
	close_file(file);
	if (!is_connected)
	{
		printf("Username or password are incorrect\n");
	}
}

void checkin()
{
	if (!is_user_logged_in())	//check if a user is logged
        {
                printf("No user logged in\n");
                return;
        }

	// check if .vcom exists, otherwise ,create it
	DIR* hiddenFile= opendir(".vcom");
	if(!hiddenFile)
		mkdir(".vcom",0777);

	if (strlen(first_arg))
        {
		File* file = open_file(first_arg, O_RDONLY, 0660);
		if(!file)
		{
			perror("error openning");
			return;
		}
		// check lock // remove later, code arrived here!
		if (is_locked(file))
		{
			if (!has_local_key(file))
			{
				// delete lock from folder
				// delete local file lock
				printf("unlocking file and checking in\n");
				char* username = get_user();
				int is_unlocked = unlock_file(file, username);
				if (is_unlocked)
				{
					printf("file is unlocked for write");
				} 
				else
				{
					printf("Could not perform checkin, the file is locked under other user's name\n");
				}
				
				free(username);
			}
			else
			{
				printf("Could not perform checkin, file is locked for checkin\n");
				return;
			}
		}
		
		History* history = get_history(file);	
		RevisionEntry* last_revision=get_last_revision(history);
		
		if(!last_revision)	//case first revision
		{
			add_revision_entry(history);
			char new_dir[127];
			char * folder_path=get_path_folder(history->absolutePath);
			strcpy(new_dir,folder_path);
			mkdir(new_dir,0777);
			strcat(new_dir,"/1");
			rev=(char*)realloc(rev,sizeof(char)*sizeof("1"));
			strcpy(rev,"1");
			copyFile(first_arg,new_dir);		//save revision as file copy
			free(folder_path);
		}
		else	//needs to check if changes were made , using diff and add new revision
		{
			//prep for getting last revision file
			printf("last revision: %f\n", last_revision->revision);
			char path_last_revision_file[127];
			char * folder_path=get_path_folder(history->absolutePath);
			char last_revision_string[100];
    		sprintf(last_revision_string, "%d", (int)last_revision->revision);
			strcpy(path_last_revision_file,folder_path);
			rev=(char*)realloc(rev,sizeof(char)*sizeof(last_revision_string));
			strcpy(rev,last_revision_string);
			strcat(path_last_revision_file,"/");
			strcat(path_last_revision_file,last_revision_string);
			File* file_last_revision = open_file(path_last_revision_file, O_RDONLY, 0660);	//open last revision file
			if(!file_last_revision)
			{
				perror("error openning");
				return;
			}
			int num_rows_different=diff(file,file_last_revision,0);	// check if changes were made
			if(num_rows_different==0)
				printf("No changes were made since last revision\n");
			else	//save changes as new revision
			{
				add_revision_entry(history);
				history = get_history(file);
				last_revision=get_last_revision(history);
				sprintf(last_revision_string, "%d", (int)last_revision->revision);
				rev=(char*)realloc(rev,sizeof(char)*sizeof(last_revision_string));
				strcpy(rev,last_revision_string);
				char new_rev[255];
				char * folder_path_1=get_path_folder(history->absolutePath);
				strcpy(new_rev,folder_path_1);
				strcat(new_rev,"/");
				strcat(new_rev,last_revision_string);
				copyFile(first_arg,new_rev);		//save revision as file copy
				free(folder_path_1);
			}
			free(folder_path);
			close_file(file_last_revision);
		}
		free_history(history);
		
	}
        else
        {
                printf("No string specified\n");
                show_help();
                return;
        }
	if (strlen(rev))
        {
                printf("Revision: %s\n", rev);
        }
	else
        {
                printf("Revision: Last known\n");
		// rev = fetch_last_revision(first_arg);
        }
	free(rev);
}
void checkout()
{
	if (!is_user_logged_in()) // authorize
        {
                printf("No user logged in\n");
                return;
        }
	if (strlen(rev))
        {
                printf("Revision: %s\n", rev);
        }
	else
        {
                printf("Revision: Last known\n");
        }
        if (strlen(first_arg))
        {

                printf("Filepath: %s\n", first_arg);
		if (access(first_arg, F_OK) != 0) // check perms
		{
			printf("The files has no permissions for the user\n");
			return;
		}
		// 48 for group rw
		File* file = open_file(first_arg, O_RDONLY, 0660);
		History* history = get_history(file);
		RevisionEntry* revision = (RevisionEntry*)malloc(sizeof(RevisionEntry));
		int found = 0;
		int locked = is_locked(file);
		if (!locked && lock_co) // check lock exists, add check request to lock
		{
			printf("locking with: %s, %s, %f, %ld\n", get_user(), history->fileName, revision->revision, revision->timestamp);
        	        create_lock(file, revision, get_user());
                	create_local_key(file, revision, get_user());
		}
		else if (locked)
		{
			printf("file is locked, and cannot be checked out\n");
			close_file(file);
			return;
		}

		if(strlen(rev))
		{
			double revision_d = atof(rev);
			for(int i = 0; i<history->totalEntries; i++)
			{
				if (history->history[i]->revision == revision_d)
				{
					revision = history->history[i];
					found = 1;
					break;
				}
			}
			if (!found)
			{
				printf("revision not found.");
				return;
			}
		}
		else
		{
			revision = get_last_revision(history);
		}
		// make checkout with the revision
		char revision_num[10];
		char dot[2] = ".";
		sprintf(revision_num, "%d", (int)revision->revision);
		char* new_name = (char*)calloc(sizeof(char), CHUNK);
		char* token;
		token = strtok(first_arg, ".");
		strcat(new_name, token);
		strcat(new_name, revision_num);

		if (strcmp(token, first_arg))
		{
			strcat(new_name, dot);
			strcat(new_name, first_arg);
		}
		printf("new file name is: %s\n", new_name);
		File* newFile = open_file(new_name, O_CREAT | O_RDWR, 0660);
		char archived_path[255];
                char * folder_path_1=get_path_folder(history->absolutePath);
		strcpy(archived_path,folder_path_1);
                strcat(archived_path,"/");
                strcat(archived_path, revision_num);
		File* archivedFile = open_file(archived_path, O_RDONLY, 0660);
		char* line;
		while(!is_eof(archivedFile))
		{
			line = read_next_line(archivedFile);
			write_data(newFile, line);
		}
		close_file(newFile);
		close_file(archivedFile);
		close_file(file);
		free_history(history);
		free(line);
	}
        else
        {
        	printf("No string specified\n");
		show_help();
		return;
        }
}
void diff2()
{
	if (!is_user_logged_in())
        {
                printf("No user logged in\n");
                return;
        }
	if (strlen(rev))
        {
                printf("Revision: %s\n", rev);
        }
	else
	{
		printf("Revision: Last known\n");
	}

	if (strlen(first_arg))
        {
                printf("Filepath: %s\n", first_arg);
        }
        else
        {
                show_help();
                return;
        }
	File* file = open_file(first_arg, O_RDONLY, 0660);
	History* history = get_history(file);
	RevisionEntry* revision = (RevisionEntry*)malloc(sizeof(RevisionEntry));
	if(strlen(rev))
	{
		int found = 0;
		double revision_d = atof(rev);
		for(int i = 0; i<history->totalEntries; i++)
		{
			if (history->history[i]->revision == revision_d)
			{
				revision = history->history[i];
				found = 1;
				break;
			}
		}
		if (!found)
		{
			printf("revision not found.");
			return;
		}
	}
	else
        {
		revision = get_last_revision(history);
	}
	char revision_num[10];
        sprintf(revision_num, "%d", (int)revision->revision);
	char archived_path[255];

        char * folder_path_1=get_path_folder(history->absolutePath);
        strcpy(archived_path,folder_path_1);
        strcat(archived_path,"/");
        strcat(archived_path, revision_num);
        File* archivedFile = open_file(archived_path, O_RDONLY, 0660);
	diff(file, archivedFile, 1);
	return;
}
char* create_user_entry(char* username, char* groups[], int size)
{
	int len = 0;
        len += strlen(username) + 1;
        for(int i = 0; i < size; i++)
        {
                len += strlen(groups[i]) + 1;
        }
        len += 2;
        char* line = (char*)calloc(len*sizeof(char), sizeof(char));
        char seperator[] = ":";
        strcat(line, username);
        strcat(line, seperator);
        for(int i = 0; i < size ; i++ )
        {
                strcat(line, groups[i]);
                strcat(line, seperator);
        }
        char online[] = "x";
        strcat(line, online);
	return line;
}

int add_user(char* username, char* groups[], int size)
{
	if (is_user_exists(username)) return 0;
	char* line = create_user_entry(username, groups, size);
	char filepath[] = "users/passwd";
	int perms = O_CREAT | O_RDWR;
	int mode = 0666;
	File* file = open_file(filepath, perms, mode);
	seek_file(file, 0, 2);
	printf("File opened \n");
	write_line(file, line);
	close_file(file);
	return 1;
}

int update_user(char* username, char* groups[], int size)
{
	if (!is_user_exists(username)) return 0;
	char users_path[] = "users/passwd";
        File* file = open_file(users_path, O_CREAT | O_RDWR, 0666);
        int is_updated = 0;
        char* token;
	char* entry = create_user_entry(username, groups, size);
	char** lines = read_lines(file, &size);

	for(int i = 0 ; i < size; i++)
	{
		char* linecpy = (char*)malloc(sizeof(char)*strlen(lines[i]));
		strcpy(lines[i], linecpy);
		token = strtok(linecpy, ":");
		if(!strcmp(token, username))
		{
			write_line(file, entry);
			is_updated = 1;
		}
		else
		{
			write_data(file, lines[i]);
		}
		free(linecpy);
	}
        free(lines);
        close_file(file);
	return is_updated;
}

int is_user_exists(char* username)
{
	char users_path[] = "users/passwd";
        File* file = open_file(users_path, O_CREAT | O_RDONLY, 0666);
        int is_found = 0;
        char* line;
        char* token;
        while(!is_eof(file) && !is_found)
        {
                line = read_next_line(file);
                token = strtok(line, ":");
                if(!strcmp(token, username))
                {
                        is_found = 1;
                }
        }
        free(line);
	close_file(file);
	return is_found;
}

int is_user_logged_in()
{
	char users_path[] = "users/passwd";
	File* file = open_file(users_path, O_CREAT | O_RDONLY, 0666);
	char* line;
	while(!is_eof(file))
	{
		line = read_next_line(file);
		if ( strlen(line) >= 2 && line[strlen(line)-2] == 'y')
		{
			free(line);
			close_file(file);
			return 1;
		}
	}
	free(line);
	close_file(file);
	return 0;
}
char* get_user()
{
	char users_path[] = "users/passwd";
        File* file = open_file(users_path, O_CREAT | O_RDONLY, 0666);
        char* line;
	char* token;
        while(!is_eof(file))
        {
                line = read_next_line(file);
                if ( strlen(line) >= 2 && line[strlen(line)-2] == 'y')
                {
			token = strtok(line, ":");
			char* username = (char*)malloc(sizeof(char)*strlen(token));
                        strcpy(username,token);
			free(line);
                        close_file(file);
                        return username;
                }
        }
        free(line);
        close_file(file);
        return 0;
}
