#ifndef __MYDIFF__
#define __MYDIFF__

#include <stdio.h>
#include <stdlib.h>

int diff(File *f1, File *f2,bool printFlag);
bool compareRows(char *row1, int size1, char *row2, int size2);
void copyFile(char* source,char* destination);

#endif
