
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "parse_header_fsm.h"

struct head_param head_param_list[] = {
    { .p_name = "Date", .val_type = INT, .len=1, .val.i_val=0   },
    { .p_name = "Server", .val_type = STRING, .len=1, .val.p_c_val= (void *)0   },
    { .p_name = "X-Powered-By", .val_type = STRING, .len=1, .val.p_c_val= (void *)0   },
    { .p_name = "Content-Length", .val_type = INT, .len=1, .val.i_val=0   },
    { .p_name = "Content-Type", .val_type = STRING, .len=1, .val.p_c_val= (void *)0   },
    { .p_name = (void *)0 , .val_type = INT, .len=0, .val.i_val=0   },
};


enum HEAD_TYPE
parse_proto_head(char *p_buf){
    
    //开始的字符指针
    char *p_beg =   p_buf;
    char *p_end = NULL;
    char find_str[30] = {0};
    p_end = strpbrk(p_buf, ": \t");
    *p_end = '\0';
    *(p_end+1) = '\0';
    p_end = p_end+2;
    int i = 0;
    while(1){
        struct head_param tmp_head_param =   head_param_list[i];
        if(strcasecmp(p_beg, tmp_head_param.p_name) == 0){
            if(strcasecmp(p_beg, "date") == 0){

                printf("[date] i=%d\n",i);

                struct tm tm_;
                time_t t_;
                strptime(p_end, "%Y-%m-%d %H:%M:%S", &tm_); //将字符串转换为tm时间  
                tm_.tm_isdst = -1;  
                t_  = mktime(&tm_);  
                head_param_list[i].val.i_val = t_;
                return PROTOCOL_HEAD_TYPE;
            }else if(strcasecmp(p_beg, "Server") == 0 ){
                printf("[server] i=%d \n",i);
                head_param_list[i].val.p_c_val = p_end;
                head_param_list[i].len = strlen(p_end);
                return PROTOCOL_HEAD_TYPE;
            }else if(strcasecmp(p_beg, "X-Powered-By") == 0 ){
                printf("[x-powered-by] i=%d \n",i);
                head_param_list[i].val.p_c_val = p_end;
                head_param_list[i].len = strlen(p_end);
                return PROTOCOL_HEAD_TYPE;
            }else if(strcasecmp(p_beg, "Content-Length") == 0){
                printf("[content-length] i=%d \n",i);
                head_param_list[i].val.i_val = atoi(p_end);
                return PROTOCOL_HEAD_TYPE;
            }else if(strcasecmp(p_beg, "Content-Type") == 0){
                printf("[content-type] i=%d \n",i);
                head_param_list[i].val.p_c_val = p_end;
                head_param_list[i].len = strlen(p_end);
                return HTTP_BODY_TYPE;
            }
        }
        i++;
        if(tmp_head_param.p_name == NULL) break;
    }
    printf("p_buf=%s\n", p_beg);
}


enum HEAD_TYPE
parse_http_head(char *p_buf){
    //开始的字符指针
    char *p_beg =   p_buf;
    char *p_end = NULL;
    char find_str[30] = {0};
    p_end =  strpbrk(p_beg, " \t");
    *p_end = '\0';
    if(!p_end) return BAD_HEAD_TYPE; 
    sscanf(p_beg, "HTTP/%s", find_str);
    if(strcmp(find_str, "1.1") != 0){
        return BAD_HEAD_TYPE;
    }
    p_beg = p_end+1;
    p_end = strpbrk(p_beg, " \t");
    *p_end = '\0';
    if(strcmp(p_beg, "200") != 0){
        return BAD_HEAD_TYPE;
    }
    return HTTP_HEAD_TYPE;
};


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
				return LINE_ING;
			}else{
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
