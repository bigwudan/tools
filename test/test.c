#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main()
{
    char *url = "http://www.192.167.1.103:83421103333/";

    char *p_beg = strstr(url+7, ":");
    char *p_end = strstr(url+7, "/");
    p_beg = p_beg+1;
    int i = p_end - p_beg;
        
    char buf[100] = {0};

    strncpy(buf, p_beg, i);
    buf[i] = '\0';
        
    int host = 0;
    host = atoi(buf);


    printf("host=%d\n", host);

    printf("i=%d\n",i);

    printf("test\n");

}



