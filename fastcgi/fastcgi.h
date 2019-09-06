#ifndef _FASTCGI_H_
#define _FASTCGI_H_
#include <sys/queue.h>


#include "wl_buffer.h"


typedef enum _fcgi_request_type {
	FCGI_BEGIN_REQUEST      =  1, /* [in]                              */
	FCGI_ABORT_REQUEST      =  2, /* [in]  (not supported)             */
	FCGI_END_REQUEST        =  3, /* [out]                             */
	FCGI_PARAMS             =  4, /* [in]  environment variables       */
	FCGI_STDIN              =  5, /* [in]  post data                   */
	FCGI_STDOUT             =  6, /* [out] response                    */
	FCGI_STDERR             =  7, /* [out] errors                      */
	FCGI_DATA               =  8, /* [in]  filter data (not supported) */
	FCGI_GET_VALUES         =  9, /* [in]                              */
	FCGI_GET_VALUES_RESULT  = 10  /* [out]                             */
} fcgi_request_type;


typedef struct _fcgi_header {
	unsigned char version; // 标识FastCGI协议版本
	unsigned char type; // 标识FastCGI记录类型，也就是记录执行的一般职能
	unsigned char requestIdB1; // 标识记录所属的FastCGI请求
	unsigned char requestIdB0; // 标识记录所属的FastCGI请求
	unsigned char contentLengthB1; // 记录的contentData组件的字节数
	unsigned char contentLengthB0; // 记录的contentData组件的字节数
	unsigned char paddingLength;
	unsigned char reserved;
} fcgi_header;


typedef enum _fcgi_role {
	FCGI_RESPONDER  = 1,
	FCGI_AUTHORIZER = 2,
	FCGI_FILTER     = 3
} fcgi_role;


typedef struct _fcgi_end_request {
	unsigned char appStatusB3; // 组件是应用级别的状态码
	unsigned char appStatusB2; // 组件是应用级别的状态码
	unsigned char appStatusB1; // 组件是应用级别的状态码
	unsigned char appStatusB0; // 组件是应用级别的状态码
	unsigned char protocolStatus; // 组件是协议级别的状态码
	unsigned char reserved[3];
} fcgi_end_request;


typedef struct _fcgi_beginrequestbody
{
	unsigned char roleB1;       //web服务器所期望php-fpm扮演的角色，具体取值下面有
	unsigned char roleB0;
	unsigned char flags;        //确定php-fpm处理完一次请求之后是否关闭
	unsigned char reserved[5];  //保留字段
}fcgi_beginrequestbody;

typedef struct 
{
	unsigned char appStatusB3;      //结束状态，0为正常
	unsigned char appStatusB2;
	unsigned char appStatusB1;
	unsigned char appStatusB0;
	unsigned char protocolStatus;   //协议状态
	unsigned char reserved[3];
}FCGI_EndRequestBody;

//keyvalue节点
struct kv_list{
	char *key;
	char *value;

	TAILQ_ENTRY(kv_list) next;
};

TAILQ_HEAD(kv_head, kv_list);



//组合抬头文件
int fastcgi_assgin_header_tobuf(fcgi_header *header, unsigned char *buf);

int fastcgi_assgin_beginrequestbody_tobuf(fcgi_beginrequestbody *body, unsigned char *buf);

//组合键值对body
int fastcgi_assgin_kvrequestbody_tobuf(struct kv_head *head, unsigned char *buf, int *npad);

//组合stdin body
int fastcgi_assgin_stdinbody_tobuf(unsigned char *dst, int *npad, const unsigned char *src, const int ndata);




#endif
