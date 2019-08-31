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
} 


int main()
{
    const UnitTest tests[] = {  
        unit_test(test_head),  
    };  
    return run_tests(tests);  


}

