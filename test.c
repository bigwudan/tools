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
	char html[1024] = {0};
	snprintf(
			html,
			sizeof(html),
					"HTTP/1.1 %d %s\r\n" 
		"Date: %s\r\n"
		"Last-Modified: %s\r\n"
		"Etag: \"%s\"\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %d\r\n"
		"Accept-Ranges: bytes\r\n\r\nwudan",
		200,
		"ok",
		"Fri, 03 Mar 2019 06:34:03 GMT",
		"Fri, 03 Mar 2019 06:34:03 GMT",
		"5ca4f75b8c3ec61:9ee",	
		"text/html",
		37	
			);
	
	printf("%s\n", html);



}
