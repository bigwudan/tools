#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#define SERVICE "192.168.1.109"
#define PORT 8111
unsigned int ELFHash(char *str)
{
	int hash = 0;
	int x=0;
	while(*str){
		hash = (hash <<4 ) + (*str++);
		if((x = hash & 0xF000000L)!=0){
			hash ^=(x >> 24);
			hash &=~x;
		
		}






	}
	return (hash & 0x7FFFFFFF);


}



int main()
{

	char *p_cbuff = NULL;

	char buf[2] = {0};

	p_cbuff = calloc(1000 , sizeof(char));
	
	buf[1] = 'x';


	


	unsigned int p = ELFHash("wudan");
	sleep(2);

	printf("p=%d\n", p);


}
