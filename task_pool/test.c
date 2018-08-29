#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{

	int fd = open("test.txt", O_RDWR|O_APPEND|O_CREAT);
	int a = 300000;
	char buf[10];
	sprintf( buf, "%d\r\n",a );


	write(fd,buf,10);


	printf("buf=%s\n", buf);

	printf("fd=%d\n",fd);	
	printf("wudan\n");

}
