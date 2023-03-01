#ifndef __HISTORY__
#define __HISTORY__

#include <time.h>
#include "files.h"

typedef struct{
	double revision;
	time_t  timestamp;
}RevisionEntry;

typedef struct{
	RevisionEntry** history;
	char* fileName;
	char* absolutePath;
	int totalEntries;
}History;

char* create_history_path(char* abs_path);
History* get_history(File* file);

RevisionEntry* get_revision(History* history, time_t timestamp);
RevisionEntry* get_last_revision(History* history);
int add_revision_entry(History* history);
void free_history(History* history);
#endif
