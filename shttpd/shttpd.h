#ifndef _SHTTPD_H_
#define _SHTTPD_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef enum SHTTPD_METHOD_TYPE{
	METHOD_GET, 		/*GET     ����*/
	METHOD_POST, 		/*POST   ����*/
	METHOD_PUT, 		/*PUT     ����*/
	METHOD_DELETE, 	/*DELETE����*/
	METHOD_HEAD,		/*HEAD   ����*/
	METHOD_CGI,		/**CGI����*/
	METHOD_NOTSUPPORT
}SHTTPD_METHOD_TYPE;


typedef struct vec 
{
	char	*ptr;
	int			len;
	SHTTPD_METHOD_TYPE type;
}vec;

union variant {
	char	*v_str;
	int		v_int;
	struct vec	v_vec;
};


struct headers {
	union variant	cl;		/* Content-Length:		*/
	union variant	ct;		/* Content-Type:		*/
	union variant	connection;	/* Connection:			*/
	union variant	ims;		/* If-Modified-Since:		*/
	union variant	user;		/* Remote user name		*/
	union variant	auth;		/* Authorization		*/
	union variant	useragent;	/* User-Agent:			*/
	union variant	referer;	/* Referer:			*/
	union variant	cookie;		/* Cookie:			*/
	union variant	location;	/* Location:			*/
	union variant	range;		/* Range:			*/
	union variant	status;		/* Status:			*/
	union variant	transenc;	/* Transfer-Encoding:		*/
};

enum{WORKER_INITED, WORKER_RUNNING,WORKER_DETACHING, WORKER_DETACHED,WORKER_IDEL};

struct conf_opts{
	char CGIRoot[128];		/*CGI��·��*/
	char DefaultFile[128];		/*Ĭ���ļ�����*/
	char DocumentRoot[128];	/*���ļ�·��*/
	char ConfigFile[128];		/*�����ļ�·��������*/
	int ListenPort;			/*�����˿�*/
	int MaxClient;			/*���ͻ�������*/
	int TimeOut;				/*��ʱʱ��*/
	int InitClient;				/*��ʼ���߳�����*/
};

extern struct conf_opts my_conf_opts;


struct worker_ctl;
struct worker_conn;

struct worker_opts
{
	pthread_t th;
	int flag;
	pthread_mutex_t mutex;
	struct worker_ctl *ctl;
};


struct conn_request{
	
	struct vec	req;		/*��������*/
	char *head;			/*����ͷ��\0'��β*/
	char *uri;			/*����URI,'\0'��β*/
	char rpath[16384];	/*�����ļ�����ʵ��ַ\0'��β*/
	int 	method;			/*��������*/
	unsigned long major;	/*���汾*/
	unsigned long minor;	/*���汾*/
	struct headers ch;	/*ͷ���ṹ*/
	struct worker_conn *conn;	/*���ӽṹָ��*/
	int err;
};


struct worker_conn
{
#define K 1024
	char drep[16*K];	
	char dres[16*K];
	int cs;
	int to;
	struct conn_request con_req;


	struct worker_ctl *ctl;


};


struct worker_ctl
{
	struct worker_opts opts;
	struct worker_conn conn;
};

extern int Worker_ScheduleRun(int);
















#endif

