#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse_header_fsm.h"

char p_m_buf[] = "HTTP/1.1 200 OK\r\nDate: Mon, 19 Nov 2018 01:01:37 GM\r\nServer: Apache/2.4.6 (CentOS) PHP/5.4.16\r\nX-Powered-By: PHP/5.4.16\r\nContent-Length: 30\r\nContent-Type: text/html; charset=UTF-8\r\n\r\nint(1542589297)string(1)";


int main(){



	int checked = 0;
	int readed = strlen(p_m_buf);
	int begline = 0;
	int start = 0;
	int flag = 0;
    
    enum HEAD_TYPE m_head_type;
    m_head_type = HTTP_HEAD_TYPE;

	while(parse_line(p_m_buf,&checked, &readed, &begline ) == LINE_OK){
        switch(m_head_type){
        
            case HTTP_HEAD_TYPE:
                parse_http_head( &p_m_buf[checked] );
                
                break;


        
        
        }
    




    
    } 


    return 1;
//	flag = parse_line(p_buf,&checked, &readed, &begline );
//	printf("flag=%d p_buf=%s checked=%d readed=%d begline=%d\n",flag, &p_buf[start], checked, readed, begline);	
//
//	start = checked;
//	flag = parse_line(p_buf,&checked, &readed, &begline );
//	printf("flag=%d p_buf=%s checked=%d readed=%d begline=%d\n",flag, &p_buf[start], checked, readed, begline);	
//
//
//	start = checked;
//	flag = parse_line(p_buf,&checked, &readed, &begline );
//	printf("flag=%d\n", flag);
//	printf("flag=%d p_buf=%s checked=%d readed=%d begline=%d\n",flag, &p_buf[start], checked, readed, begline);	

}
