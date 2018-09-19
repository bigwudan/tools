#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

typedef enum CHECK_STATE {CHECK_STATE_REQUEATLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT   }CHECK_STATE;//检查请求
typedef enum LINE_STATUS {LINE_OK=0, LINE_BAD, LINE_OPEN}LINE_STATUS;
typedef enum HTTP_CODE {NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION }HTTP_CODE;

char *p_head = "GET http://www.baidu.com/index.html HTTP/1.0\r\n\
User-Agent: Wget/1.12 (linux-gnu)\r\n\
Host: www.baidu.com\r\n\
Connection: close\r\n";





LINE_STATUS 
parse_line(char *p_buff, int *checked_index, int *read_index)
{
    char temp;
    for(; *checked_index < *read_index; ++(*checked_index)){
        temp = p_buff[*checked_index];  
        if(temp == '\r'){
            if((*checked_index + 1) == *read_index ){
                return LINE_OPEN;
            }else if(p_buff[(*checked_index) + 1] == '\n'    ){
                p_buff[(*checked_index)++] = '\0';
                p_buff[(*checked_index)++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }else if(temp == '\n'){
            if( ((*checked_index) > 1) && p_buff[*checked_index-1] == '\r'    ){
                p_buff[ *checked_index - 1  ] = '\0';
                p_buff[ (*checked_index)++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}


    HTTP_CODE 
parse_requestline(char *szTemp, CHECK_STATE* checkstate )
{


}




    HTTP_CODE 
parse_content(char* buffer, int* checked_index, CHECK_STATE* check_state, int* read_index, int* start_line  ) 
{
    LINE_STATUS linestatus = LINE_OK;
    HTTP_CODE retcode = NO_REQUEST;

    linestatus = parse_line(buffer, checked_index, read_index);

    while(  (linestatus = parse_line(buffer, checked_index, read_index))  == 0  ){
        char* szTemp = buffer + start_line;
        start_line = checked_index;
        switch(checkstate){
            case CHECK_STATE_REQUEATLINE:
                {
                    retcode = parse_req


                }




        }


    }




    printf("buf=%s\n", buffer);
    printf("linestatus = %d\n", linestatus);
    exit(1);








}




int main(int argc, char* argv[])
{
    char *buffer = (char *)calloc(BUFFER_SIZE, 1);
    if(buffer == NULL){
        perror("calloc\n");
    }   
    strcpy(buffer, p_head);
    int data_read = 0;
    int read_index = 0;
    int checked_index = 0;
    int start_line = 0;
    CHECK_STATE checkstate = CHECK_STATE_REQUEATLINE;

    read_index = strlen(p_head);


    parse_content(buffer, &checked_index, &checkstate, &read_index, &start_line);





    printf("line=%s\n", buffer);
    printf("test\n");

}




