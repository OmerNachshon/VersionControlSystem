#include "files.h"
#include "history.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

void show_help();
void process_arguments(int argc, char *argv[], char* rev, char* pass, int* var_ind);
void login();
void checkin();
void checkout();
void diff();
char* create_user_entry(char* username, char* groups[], int size);
int add_user(char* username, char* groups[], int size);
int update_user(char* username, char* groups[], int size);
int is_user_exists(char* username);
void remove_user(char* username);
int is_user_logged_in();
char* get_user();

const char* commands[] = {"login", "checkin", "checkout", "diff"};
void (*functions[4])() = {login, checkin, checkout, diff};

static char* first_arg = (char*)malloc(0);
static char* rev = (char*)malloc(0);
static char* pass = (char*)malloc(0);


int main(int argc, char* argv[])
{
	if ( argc == 1 )
	{
		printf("The use of the command is incorrect, use: vcom [options <command>] <command>");
		show_help();
		return 0;
	}

	int var_ind = 0;

	process_arguments(argc, argv, rev, pass, &var_ind);

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

void process_arguments(int argc, char *argv[], char* str, char* pass, int* var_ind) {
	int opt;

	while ((opt = getopt(argc, argv, "r:p:")) != -1) {
		switch (opt) {
		case 'r':
			strcpy(str, optarg);
			*(var_ind) = *(var_ind) + 2;
			break;
		case 'p':
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
	if (!is_user_logged_in())
        {
                printf("No user logged in\n");
                return;
        }

	if (strlen(first_arg))
        {
                File* file = open_file(first_arg, O_RDONLY, 0666);
		printf("Filepath: %s\n", create_history_path(first_arg));
		History* history = get_history(file);
		printf("History history_file: %s\n", history->absolutePath);
		for(int i = 0; i< history->totalEntries; i++)
		{
			printf("Filename: %s, Revision: %f\n", history->fileName, history->history[i]->revision);
		}
		add_revision_entry(history);
		// (mode_t)48
		// add backup with perms and mode
		// add diff
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
	// todo - lock check
	// todo - check login
}
void checkout()
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
		File* file = open_file(first_arg, O_RDONLY, 48);
        	History* history = get_history(file);
		RevisionEntry* revision = (RevisionEntry*)malloc(sizeof(RevisionEntry));
		int found = 0;
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
		printf("locking with: %s, %s, %f, %ld\n", get_user(), history->fileName, revision->revision, revision->timestamp);
		printf("%s\n", history->absolutePath);
		printf("%d\n", history->totalEntries);
	}
        else
        {
                printf("No string specified\n");
		show_help();
		return;
        }
}
void diff()
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
                printf("No string specified\n");
		show_help();
		return;
	}
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
