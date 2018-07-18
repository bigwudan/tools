#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked.h"


int main()
{
	char buf[2] = {0};

	sprintf(buf, "%d", 1000);

	printf("buf=%d\n",sizeof(buf));

}




