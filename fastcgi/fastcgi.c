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

//组合键值对body
int
fastcgi_assgin_kvrequestbody_tobuf(struct kv_head *head, unsigned char *buf, int *npad)
{
	if(head == NULL) return 0;
	unsigned char *oldbuf = buf;
	int k_len = 0;	
	int v_len = 0;
	int tot = 0;
	int mod = 0;
	struct kv_list* tmp_kv;
	TAILQ_FOREACH(tmp_kv, head, next) {
		printf("k=%s,v=%s\n", tmp_kv->key, tmp_kv->value);
		//key 长度
		k_len = strlen(tmp_kv->key);
		if(k_len > 127){
			*buf++ = k_len >> 3*8 && 0xff;
			*buf++ = k_len >> 2*8 && 0xff;
			*buf++ = k_len >> 1*8 && 0xff;
			*buf++ = k_len >> 0*8 && 0xff;
		}else{
			*buf++ = k_len;
		} 
		
		//value 长度
		v_len = strlen(tmp_kv->value);
		if(v_len > 127){
			*buf++ = v_len >> 3*8 && 0xff;
			*buf++ = v_len >> 2*8 && 0xff;
			*buf++ = v_len >> 1*8 && 0xff;
			*buf++ = v_len >> 0*8 && 0xff;
		}else{
			*buf++ = v_len;
		} 
		//key 值
		strcpy(buf, tmp_kv->key);
		buf = buf + k_len;
		//value 值
		strcpy(buf, tmp_kv->value);
		buf = buf + v_len;
	}
	tot = buf - oldbuf;
	mod = tot%8;
	//需要8的倍数
	if(mod != 0 ){
		mod = 8 - mod;
		*npad = mod;
		memset(buf, 0, mod);
		tot = tot + mod;
	}
	return tot;
}




