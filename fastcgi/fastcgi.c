#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fastcgi.h"




//组合抬头文件
int
fastcgi_assgin_header_tobuf(fcgi_header *header, unsigned char *buf)
{
    memmove(buf, header, sizeof(fcgi_header));
    return sizeof(fcgi_header);
}

int
fastcgi_assgin_beginrequestbody_tobuf(fcgi_beginrequestbody *body, unsigned char *buf)
{
    *buf++ = body->roleB1;
    *buf++ = body->roleB0;
    *buf++ = body->flags;
    memmove(buf, body->reserved, 5);
    return 8;
}






