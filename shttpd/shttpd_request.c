#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "shttpd.h"
void header_parse( char *head_str, int len, struct headers *ch)
{
	char *p = NULL;
	char *pos = head_str;
	char *end = head_str + len ;
	union variant *v = NULL;
	while( p <= end ){
		for(p = pos; *p != '\n'; p++){
		}	
		struct http_header *p_http_header = NULL;
		for(p_http_header = http_headers; p_http_header->len !=0; p_http_header++){
			if( strncmp(p_http_header->name, pos, p_http_header->len   ) == 0  ){
				break;
			}	
		}
		if(p_http_header->len != 0){
			pos += p_http_header->len; 
			v = (union variant *)((char *)ch + p_http_header->offset  );
			if( p_http_header->type == HDR_INT  ){
				v->v_int =  strtoul( pos, NULL, 10 );
			}else if(  p_http_header->type == HDR_STRING ){
				v->v_vec.len = p - pos;
				v->v_vec.ptr = pos;	
				if(*(p-1) == '\r'){
					v->v_vec.len--;
				}
			}
		}
		pos = p + 1;	
	}
	return ;
}


void request_parse(struct conn_request *p_request)
{
	char *str = p_request->rqs;
	int len = p_request->len;
	int clen = 0;
	char *pos = NULL;
	pos = memchr(str, '\n', len);
	clen = pos - str + 1 ;
	if(  *(pos - 1) == '\r'  ){
		*(pos - 1) = '\0';
	}	
	*pos = '\0';
	pos = str;
	JUMPTO_CHAR(pos, ' ');
	*pos = '\0';
	struct vec *v = NULL;
	for(v=_shttpd_methods; v->ptr != NULL; v++ ){
		if( strncmp(v->ptr, str, v->len) == 0 ){
			p_request->method = v->type;
			break;
		}
	}
	str = pos+ 1;
	JUMPTO_CHAR(pos, ' ');
	*pos = '\0';
	p_request->uri = str;
	str = pos + 1;
	sscanf(str, "HTTP/%lu.%lu", &p_request->major, &p_request->minor);
	str = p_request->rqs + clen  ;
	len = len - clen;
	header_parse(str, len, &p_request->ch);
	return ;
}







int main(int argn, char **argv)
{
	char headstr[1024] = "GET /root/default.html HTTP/1.1\r\nContent-Length: 1111\r\nContent-Type: 2222\r\nUser-Agent: 323232\r\n\r\nhttpsaeqrwerwerwer";
	struct conn_request my_request;
	strcpy(my_request.rqs, headstr);

	my_request.len = strlen(headstr);


	request_parse(&my_request);









}
