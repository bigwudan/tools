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
struct worker_opts
{
	pthread_t th;
	int flag;
	pthread_mutex_t mutex;
	struct worker_ctl *ctl;
};


struct worker_conn
{
#define K 1024
	char drep[16*K];	
	char dres[16*K];
	int cs;
	int to;
	struct worker_ctl *ctl;


};


struct worker_ctl
{
	struct worker_opts opts;
	struct worker_conn conn;
};

extern int Worker_ScheduleRun(int);
















#endif

