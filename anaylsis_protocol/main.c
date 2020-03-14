#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/tcp.h> // for TCP_NODELAY
#include <arpa/inet.h>


#include "analysis_protocol.h"
#include "analysis_protocol_tools.h"



unsigned char test_buf[] = {
	//[0][0] //[0][1]                                                 //erno
	0xEA, 0x1B, 0x10, 0x4D, 0x00, 0x00, 0x00, 0x2D, 0x10, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x79, 0x53,
/*	0xEA, 0x1B, 0x11, 0x01, 0x00, 0x00, 0x00, 0x1E, 0x0A, 0x2A, 0x28, 0x26, 0x2A, 0x00, 0x00, 0x48, 0x35,
	0xEA, 0x1B, 0x12, 0x00, 0xCD, 0x80, 0x9E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x05, 0x4B,
	0xEA, 0x1B, 0x13, 0x00, 0x00, 0x05, 0x40, 0x50, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBE, 0x5F,*/
};

uint8_t ack_buf[] = {0xEB, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xD8, 0x2A};


uint8_t 
yingxue_frame_recv_fun(struct analysis_protocol_base_tag *base, void *arg)
{
    uint8_t flag = 0; //是否有数据
    uint8_t data = 0x00;//保存的数据
    uint8_t len =0;//长度
	uint8_t res = 0;
    struct chain_list_tag *chain = base->chain_list;
    struct yingxue_frame_tag *yingxue = (struct yingxue_frame_tag *)base->recv_frame;
    do{
        is_empty_chain_list(chain, flag);
        if(flag == 0) break;
        out_chain_list(chain, data);
		//
		if(yingxue->len == 0){
			if(data != 0xEA){
				continue;
			}			
			yingxue->data[len++] = data;
			yingxue->len++;
		}else{
			yingxue->data[len++] = data;
			yingxue->len++;
			if(yingxue->len == 17){
				res = 1;
				break;
			}
	
		}

    }while(flag);
    return res;
}

//得到一个完整的数据，然后分析，最后判断是否写入缓存
uint8_t
yingxue_process_frame(struct analysis_protocol_base_tag *base, void *arg)
{
   //写入写入缓存
    //是否加入检查重试的队列
    struct analysis_protocol_send_frame_list_tag *send_dest = SELF_MALLOC(sizeof(struct analysis_protocol_send_frame_list_tag));
    send_dest->repeat_max = 2;
    send_dest->repeat_num = 1;

	memmove(send_dest->data, ack_buf, sizeof(ack_buf) );

    send_dest->data_len = sizeof(ack_buf);
    gettimeofday(&send_dest->last_send_time,NULL);
    send_dest->repeat_during = 2;
    TAILQ_INSERT_TAIL(&base->send_frame_head, send_dest, next);
    //加入发送列表

    struct analysis_protocol_send_frame_to_dest_tag *send_frame_dest = 
        SELF_MALLOC(sizeof(struct analysis_protocol_send_frame_to_dest_tag));

    memmove(send_frame_dest, send_dest->data, send_dest->data_len);
    send_frame_dest->data_len = send_dest->data_len;
    TAILQ_INSERT_TAIL(&base->send_frame_dest_head, send_frame_dest, next );
    return 0;    

}


//底层发送数据
uint8_t
send_func_bc(struct analysis_protocol_base_tag *base, void *arg)
{
    struct analysis_protocol_send_frame_to_dest_tag *send;
    send = TAILQ_FIRST(&base->send_frame_dest_head);
	if(send){
		TAILQ_REMOVE(&base->send_frame_dest_head, send, next); 
		printf("send:");
		for(int i=0; i < send->data_len; i++){
			printf("0x%02X ", send->data[i]);
		}
		printf("\r\n");
		SELF_FREE(send);
	}
    return 0;
}


uint8_t check_reply_func(struct analysis_protocol_base_tag *base, void *arg)
{
	//移除一个
	struct analysis_protocol_send_frame_list_tag *tmp = TAILQ_FIRST(&base->send_frame_head);	
	if(tmp){
		TAILQ_REMOVE(&base->send_frame_head, tmp, next);		
		SELF_FREE(tmp);
	}
    return 0;

}

//测试
void test_fun( struct analysis_protocol_base_tag *base )
{
    //读的数据写入数据库
    uint8_t len = 0;
    //写入缓存
    len = analysis_protocol_write_chain_list(base->chain_list, test_buf, sizeof(test_buf));
    printf("len=0x%02X\n", len);
    
    //运行分析数据看是否得到一个完整的数据
    len = base->get_recv_frame_bc(base, NULL);
	printf("len=%d\n", len);

    if(len == 1){
		printf("rec finish\n");
        base->process_recv_frame_bc(base, NULL);
        //检查是否需要重复
		check_reply_func(base, NULL);	
    }
	//发送
	base->run_send_frame_bc(base, NULL);
    //查看是否需要超时
    analysis_protocol_overtime_send(base);

}




int main()
{
	struct sockaddr_in serv;
	int flag = 0;

	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(1885);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	int socketfd = 0;

	socketfd =  socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0);

	flag = bind(socketfd, &serv, sizeof(serv));
	if(flag != 0){
		printf("flag = %d\n", flag);
		exit(1);
	}


	listen(socketfd, 5);
	if(flag != 0){
		printf("flag = %d\n", flag);
		exit(1);
	}

	int sfd = 0;

	socklen_t addrlen = 0;
	struct sockaddr_storage addr;
	unsigned int socklen = sizeof(addr);
	memset(&addr, 0, sizeof(struct sockaddr_storage));

	sfd = accept(socketfd, (struct sockaddr *)&addr, &addrlen);
	while(1){
		printf("sfd=%d\n", sfd);
	}		


    struct analysis_protocol_base_tag *base_yingxue = analysis_protocol_init((void *)0, yingxue_frame_recv_fun, 
                                                                             yingxue_process_frame, send_func_bc, check_reply_func ); 

    test_fun(base_yingxue);




    printf("wudan\n");


}
