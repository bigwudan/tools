#include "parse_header_fsm.h"

enum LINE_STATE
parse_line(char *p_buf, int *p_checked, int *p_readed, int *p_begline){
	int checked = *p_checked;
	int readed = *p_readed;
	int begline = *p_begline;
	for(int i=checked; checked < readed; checked++){
		char c = p_buf[checked];	
		if( c == '\r' ){
			if( p_buf[checked+1] == '\n' ){
				p_buf[checked] = '\0';
				p_buf[checked+1] = '\0';
				*p_checked = checked+2;	
				return LINE_OK;			
			}else if(checked +1 >= readed ){
				return LINE_BAD;
			}
		}else if( c == '\n'){
			if( p_buf[checked - 1] == '\r' ){
				p_buf[checked - 1] = '\0';
				p_buf[checked] = '\0';
				*p_checked = checked+1;
				return LINE_OK;
			}else{
				return LINE_BAD;
			}
		}	
	}
	*p_checked = checked+1;
	return LINE_ING;
}
