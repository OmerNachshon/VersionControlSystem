#ifndef __LOCK__
#define __LOCK__

#include "files.h"
#include "history.h"

typedef struct{
	char* user;
	time_t timestamp;
}Lock;

void create_lock(File* file, RevisionEntry* revision, char* user);
void create_local_key(File* file, RevisionEntry* revision, char* user);
int is_locked(File* file);
int unlock_file(File* file, time_t time, char* user);

#endif
