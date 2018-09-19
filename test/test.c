#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };

char *p_str = "GET http://www.baidu.com/index.html HTTP/1.0";

int main(int argc, char* argv[])
{
    
    char *szTemp = calloc(strlen(p_str), 1);    

    strcpy(szTemp, p_str);



    char* szURL = strpbrk( szTemp, " \t" );
    if ( ! szURL )
    {
        return BAD_REQUEST;
    }
    *szURL++ = '\0';

    char* szMethod = szTemp;
    if ( strcasecmp( szMethod, "GET" ) == 0 )
    {
        printf( "The request method is GET\n" );
    }
    else
    {
        return BAD_REQUEST;
    }
    
    //szURL += strspn( szURL, " \t" );

    printf("len=%lu\n", strspn(szURL, " \t"));




    printf("test\n");


}



