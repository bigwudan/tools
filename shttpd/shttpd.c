#include "shttpd.h"


struct conf_opts my_conf_opts = 
{
	"/1212",
	"/www",
	"/3434",
	"12121",
	8000,
	4,
	3,
	2

};


static void sig_int(int num)
{
	printf("sig_int\n");
}

static int do_listen()
{
	struct sockaddr_in server;
	int ss;
	int reuse;
	int err;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(my_conf_opts.ListenPort);
	ss = socket(AF_INET, SOCK_STREAM, 0);
	if(ss == -1){
		perror("socket create error\n");
		exit;
	}
	err = setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	if(err == -1){
		perror("setsockopts error\n");
		exit;
	}
	err = bind(ss, (struct sockaddr*) &server, sizeof(server));
	if(err == -1){
		perror("bind error\n");
		exit;
	}
	err = listen(ss, my_conf_opts.MaxClient*2);
	if(err){
		perror("listen error\n");
		exit;
	}
	return ss;
}





int main(int argc,  char **argv)
{
	//signal(SIGINT, sig_int);
	int ss = do_listen();
	
	printf("ss=%d\n", ss);
	
	Worker_ScheduleRun(ss);


	printf("test\n");



}
