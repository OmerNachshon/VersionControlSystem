#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
int main()
{
	printf("%d write group %d read group, or group perms %d\n", S_IWGRP, S_IRGRP, S_IRGRP | S_IWGRP);
	return 0;
}
