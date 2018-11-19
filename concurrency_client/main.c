#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse_header_fsm.h"

int main(){
	char p_buf[] = "1234567\r\nabcd\r\ncd";
	int checked = 0;
	int readed = strlen(p_buf);
	int begline = 0;
	int start = 0;
	int flag = 0;
	flag = parse_line(p_buf,&checked, &readed, &begline );
	printf("flag=%d p_buf=%s checked=%d readed=%d begline=%d\n",flag, &p_buf[start], checked, readed, begline);	

	start = checked;
	flag = parse_line(p_buf,&checked, &readed, &begline );
	printf("flag=%d p_buf=%s checked=%d readed=%d begline=%d\n",flag, &p_buf[start], checked, readed, begline);	


	start = checked;
	flag = parse_line(p_buf,&checked, &readed, &begline );
	printf("flag=%d\n", flag);
	printf("flag=%d p_buf=%s checked=%d readed=%d begline=%d\n",flag, &p_buf[start], checked, readed, begline);	

}
