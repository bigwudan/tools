#include <stdio.h>
#include "fastcgi.h"

#include <stdarg.h>  
#include <stddef.h>  
#include <setjmp.h>  
#include <google/cmockery.h>  


void test_head(void **state) 
{  
	unsigned char buf[80000] = {0};
	unsigned int position = 0;
	int ndata = 0;
	int npad = 0;
	struct wl_evbuffer *evbuf =  wl_evbuffer_new();
	//assert_int_equal(evbuf, 0);	
	assert_true(evbuf);
	
	fcgi_header head = {0x00, 0x01, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00};	
	
	ndata = fastcgi_assgin_header_tobuf(&head, buf);	
	position += ndata;	

	//得到body
	fcgi_beginrequestbody body = {
		0x00,
		0x01,
		0x01,
		{0x00, 0x00, 0x00,0x00,0x00}	
	};
	ndata = fastcgi_assgin_beginrequestbody_tobuf(&body, buf+position);
	position += ndata;	
	assert_true(ndata);	
	

	//组合key value
	struct kv_head kv_list_head;
	TAILQ_INIT(&kv_list_head);

	//1
	struct kv_list kv_1;
	kv_1.key = "key1";
	kv_1.value = "value1";

	TAILQ_INSERT_TAIL(&kv_list_head, &kv_1, next);		


	struct kv_list kv_2;
	kv_2.key = "key1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
	kv_2.value = "value122222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222";

	TAILQ_INSERT_TAIL(&kv_list_head, &kv_2, next);		


	struct kv_list kv_3;
	kv_3.key = "key111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111122222";
	kv_3.value = "value1222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222444";

	TAILQ_INSERT_TAIL(&kv_list_head, &kv_3, next);		

	ndata =  fastcgi_assgin_kvrequestbody_tobuf(&kv_list_head, buf+position, &npad);
	
	//抬头	
	fcgi_header head1 = {0x00, 0x01, 0x00, 0x01, ndata>>8 & 0xff, ndata & 0xff, npad, 0x00};	


} 


int main()
{
    const UnitTest tests[] = {  
        unit_test(test_head),  
    };  
    return run_tests(tests);  


}

