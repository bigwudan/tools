#include <stdio.h>
#include <string.h>
#include "fastcgi.h"

#include <stdarg.h>  
#include <stddef.h>  
#include <setjmp.h>  
#include <google/cmockery.h>  


void test_head(void **state) 
{  
	unsigned char buf[80000] = {0};
	unsigned char head_buf[3000] = {0};
	unsigned int position = 0;
	int ndata = 0;
	int npad = 0;
	int nhead = 0;

    
    
	struct wl_evbuffer *evbuf =  wl_evbuffer_new();
	//assert_int_equal(evbuf, 0);	
	assert_true(evbuf);
	
	fcgi_header f_head;

	
	// beg request
	f_head.version = 0x00;
	f_head.type = 0x01;
	f_head.requestIdB1 = 0x00;
	f_head.requestIdB0 = 0x01;
	f_head.contentLengthB1 = 0x00;
	f_head.contentLengthB0 = 0x08;
	f_head.paddingLength = 0x00;
	f_head.reserved = 0x00;
	ndata = fastcgi_assgin_header_tobuf(&f_head, buf);	
	wl_evbuffer_add(evbuf, buf, ndata);
	memset(buf, 0, sizeof(buf));
	//得到body
	fcgi_beginrequestbody body = {
		0x00,
		0x01,
		0x01,
		{0x00, 0x00, 0x00,0x00,0x00}	
	};
	ndata = fastcgi_assgin_beginrequestbody_tobuf(&body, buf);
	wl_evbuffer_add(evbuf, buf, ndata);
	memset(buf, 0, sizeof(buf));
	assert_true(ndata);	
	// end beg request

	//beg params
	struct kv_head kv_list_head;
	TAILQ_INIT(&kv_list_head);

	//1
	struct kv_list kv_1;
	kv_1.key = "k1";
	kv_1.value = "v1";

	TAILQ_INSERT_TAIL(&kv_list_head, &kv_1, next);		


	struct kv_list kv_2;
	kv_2.key = "k2";
	kv_2.value = "v2";

	TAILQ_INSERT_TAIL(&kv_list_head, &kv_2, next);		



	ndata =  fastcgi_assgin_kvrequestbody_tobuf(&kv_list_head, buf, &npad);
	

	f_head.version = 0x00;
	f_head.type = 0x04;
	f_head.requestIdB1 = 0x00;
	f_head.requestIdB0 = 0x01;
	f_head.contentLengthB1 = ndata>>8 & 0xff;
	f_head.contentLengthB0 = ndata & 0xff;
	f_head.paddingLength = npad;
	f_head.reserved = 0x00;
	
	nhead =  fastcgi_assgin_header_tobuf(&f_head, head_buf);	
	
	wl_evbuffer_add(evbuf, head_buf, nhead);
	wl_evbuffer_add(evbuf, buf, ndata);

	f_head.version = 0x00;
	f_head.type = 0x04;
	f_head.requestIdB1 = 0x00;
	f_head.requestIdB0 = 0x01;
	f_head.contentLengthB1 = 0x00;
	f_head.contentLengthB0 = 0x00;
	f_head.paddingLength = 0x00;
	f_head.reserved = 0x00;
    //加入00

	nhead =  fastcgi_assgin_header_tobuf(&f_head, head_buf);	
	wl_evbuffer_add(evbuf, head_buf, nhead);
	//end params
    
    //beg stdin
    char *tmp_str = "this is post";
    ndata = fastcgi_assgin_stdinbody_tobuf(buf, &npad, tmp_str, strlen(tmp_str));    

	f_head.version = 0x00;
	f_head.type = 0x05;
	f_head.requestIdB1 = 0x00;
	f_head.requestIdB0 = 0x01;
	f_head.contentLengthB1 = ndata >> 8 & 0xff;
	f_head.contentLengthB0 = ndata & 0xff;
	f_head.paddingLength = npad;
	f_head.reserved = 0x00;

    //head
	nhead =  fastcgi_assgin_header_tobuf(&f_head, head_buf);	
	wl_evbuffer_add(evbuf, head_buf, nhead);
    //body
	wl_evbuffer_add(evbuf, buf, ndata);
    // 00 head
	f_head.version = 0x00;
	f_head.type = 0x05;
	f_head.requestIdB1 = 0x00;
	f_head.requestIdB0 = 0x00;
	f_head.contentLengthB1 = 0x00;
	f_head.contentLengthB0 = 0x00;
	f_head.paddingLength = 0x00;
	f_head.reserved = 0x00;
    //head
	nhead =  fastcgi_assgin_header_tobuf(&f_head, head_buf);	
	wl_evbuffer_add(evbuf, head_buf, nhead);
} 


int main()
{
    const UnitTest tests[] = {  
        unit_test(test_head),  
    };  
    return run_tests(tests);  


}

