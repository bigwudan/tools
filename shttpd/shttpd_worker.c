#include "shttpd.h"
static struct worker_ctl *ctls = NULL;
pthread_mutex_t thread_init = PTHREAD_MUTEX_INITIALIZER;



static int worker_isstatus(int status)
{
	int n = 0;
	for(n = 0; n < my_conf_opts.MaxClient; n++){
		if(ctls[n].opts.flag == status   ){
			return n;
		}
	}
	return -1;
}


static void do_work( struct worker_ctl *ctl )
{
	int fd = ctl->conn.cs;
	int retval = 1;
	int flag = 0;
	struct timeval tv;
	
//	printf("do_wrok-------- \n");
//
//	flag =  read( fd, ctl->conn.drep, sizeof(ctl->conn.drep)  );
//
//	if(flag > 0){
//		printf("drep=%s\n", ctl->conn.drep);
//	}else{
//		printf("error\n");
//	}
//
//
//	
//	getchar();

	fd_set rfds;


	for(; retval>0; ){
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		tv.tv_sec = 300;
		tv.tv_usec = 0;
		retval = select(fd+1, &rfds, NULL, NULL, &tv);
		
		switch(retval){
			case -1:
				close(fd);
				break;
			case 0:
				close(fd);
				break;
			default:
				if( FD_ISSET(fd, &rfds) ){
					memset(ctl->conn.drep, 0, sizeof(ctl->conn.drep));
					ctl->conn.con_req.req.len \
						= \
					read( fd, ctl->conn.drep, sizeof(ctl->conn.drep)  );
					if( ctl->conn.con_req.req.len > 0 ){
						printf("buff=%s\n", ctl->conn.drep);
					}else{
						close(fd);
						retval = -1;
					}
				}
		}
	}
	
}



static void *worker(void *arg)
{
	printf("thread start\n");
	struct worker_ctl *p_ctl = NULL;
	int flag = 0;
	p_ctl = (struct worker_ctl *)arg;
	pthread_mutex_unlock(&thread_init);
	p_ctl->opts.flag = WORKER_IDEL;
	for(; p_ctl->opts.flag != WORKER_DETACHING; ){
		flag = pthread_mutex_trylock(&p_ctl->opts.mutex);
		if(flag){
			sleep(1);
			continue;
		}else{
			p_ctl->opts.flag == WORKER_RUNNING;
			printf("do work!\n");
			do_work( p_ctl );

			if( p_ctl->opts.flag == WORKER_DETACHING  ){
				break;
			}else{
				p_ctl->opts.flag = WORKER_IDEL;
				continue;
			}
		}
	}
	p_ctl->opts.flag = WORKER_DETACHED;
	return ;
}


static void Worker_add(int i)
{
	pthread_t th;
	pthread_mutex_lock(&thread_init);
	int err = pthread_create(&th, NULL, worker, (void*)&ctls[i]);/*�����߳�*/
	pthread_mutex_unlock(&thread_init);
}


static Worker_Init()
{
	ctls = (struct worker_ctl *)malloc(  my_conf_opts.MaxClient * sizeof( struct worker_ctl ) );

	if(!ctls){
		perror("malloc error\n");
		exit(1);
	}
	memset(ctls, 0 , sizeof(ctls)*my_conf_opts.MaxClient  );
	int n = 0;
	for(n=0; n < my_conf_opts.MaxClient; n++){
		ctls[n].opts.ctl = &ctls[n];
		ctls[n].conn.ctl = &ctls[n];
		ctls[n].opts.flag = WORKER_IDEL;
		pthread_mutex_init(&ctls[n].opts.mutex, NULL);
		pthread_mutex_lock(&ctls[n].opts.mutex);
	}
	
	for(n=0; n < my_conf_opts.MaxClient; n ++){
		Worker_add(n);
	}
}


int Worker_ScheduleRun(int ss)
{
	Worker_Init();
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	int i = 0;
	while(1){
		printf("select start\n");
		struct timeval tv;
		fd_set rfds;
		int retval = -1;
		FD_ZERO(&rfds);
		FD_SET(ss, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 500000;
		retval = select(ss+1, &rfds, NULL, NULL, &tv);
		switch(retval){

			case -1:
				perror("select error");
				exit;
			case 0:
				continue;
			default:
				if(FD_ISSET( ss, &rfds ) ){
					printf("request arrive\n");
					int sc = accept(ss, (struct sockaddr*)&client, &len  );	
					i = worker_isstatus(WORKER_IDEL);
//					if(i == -1){
//						i = worker_isstatus(WORKER_DETACHED);
//						if(i != 1){
//							Worker_add(i);				
//						}
//					}
					if(i != -1){
						ctls[i].conn.cs = sc;	
						pthread_mutex_unlock(&ctls[i].opts.mutex);
					}else{
						printf("pthread error\n");
						exit;
					}
				}
		}	
	}
}





