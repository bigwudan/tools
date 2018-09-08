#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#define NGX_ALIGNMENT   sizeof(unsigned long)    /* platform word */

int main()
{

	printf("len=%d\n", NGX_ALIGNMENT);

}
