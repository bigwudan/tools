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


#include "../analysis_protocol.h"
#include "../analysis_protocol_tools.h"

struct analysis_protocol_base_tag *base_yingxue;
int client_fd_g = 0;

unsigned char test_buf[] = {
	//[0][0] //[0][1]                                                 //erno
	0xEA, 0x1B, 0x10, 0x4D, 0x00, 0x00, 0x00, 0x2D, 0x10, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x79, 0x53,
/*	0xEA, 0x1B, 0x11, 0x01, 0x00, 0x00, 0x00, 0x1E, 0x0A, 0x2A, 0x28, 0x26, 0x2A, 0x00, 0x00, 0x48, 0x35,
	0xEA, 0x1B, 0x12, 0x00, 0xCD, 0x80, 0x9E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x05, 0x4B,
	0xEA, 0x1B, 0x13, 0x00, 0x00, 0x05, 0x40, 0x50, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBE, 0x5F,*/
};

uint8_t ack_buf[] = {0xEB, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xD8, 0x2A};
uint8_t reply[] =  {0xAA, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xD8, 0xFF};
uint8_t on_buf[] = {0xEB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xD8, 0x2A};

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
        flag = is_empty_chain_list(chain);
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

    int flag = 1;
    unsigned char send_buff[11] = {0};
    //如果是第二帧加入回复缓存
    struct yingxue_frame_tag *yingxue_frame = (struct yingxue_frame_tag *)base->recv_frame; 
    /*printf("rev finish frame= ");
    for(int i=0; i < yingxue_frame->len; i++){
        printf(" 0x%02X ", yingxue_frame->data[i]);
    }
    printf("\r\n");*/

    memmove(send_buff, ack_buf, sizeof(send_buff));

    //判断当前帧的状态 是否在重复列表中，有的话去挑
    
    analysis_protocol_compare_recv_repeat(&base->send_frame_head, 0x02, 0x01, DATA);




    //发送命令
    if(yingxue_frame->data[2] == 0x11){

        //需要发送命令开机
        if(yingxue_frame->data[3] ==0x01 ){
            //去重在重复列表中
            analysis_protocol_recv_repeat_up(base, 0x01, 0x02, DATA, on_buf, sizeof(on_buf));
            //插入发送队列
            analysis_protocol_insert_send_list(base, 0x01, 0x02, DATA, on_buf, sizeof(on_buf), 0);
        }

    }    
    //接收帧长度为0
    yingxue_frame->len = 0;
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
        write(client_fd_g, send->data, send->data_len);
		SELF_FREE(send);
        //vPortFree(send);
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
        //vPortFree(tmp);
	}
    return 0;

}

//测试
void test_fun( struct analysis_protocol_base_tag *base )
{
    //读的数据写入数据库
    int8_t len = 0;
    uint8_t flag = 0;
    uint8_t recv_buf[100] = {0};

    while(1){
        //写入缓存
        gettimeofday(&base->curr_cache_time, NULL);
        sleep(1);
        len = read(client_fd_g, recv_buf, sizeof(recv_buf));

        if(len > 0){
            //写入缓存
            flag = analysis_protocol_write_chain_list(base->chain_list, recv_buf, len);
        }
        //查看环形缓存中是否为空
        flag = is_empty_chain_list(base->chain_list);
        if(flag != 0){
        
            flag = base->get_recv_frame_bc(base, NULL);
            if(flag == 1){
                base->process_recv_frame_bc(base, NULL);
                //检查是否需要重复
                //check_reply_func(base, NULL);	
            }
            //发送
            base->run_send_frame_bc(base, NULL);
            //查看是否需要超时
            analysis_protocol_overtime_send(base);
        
        }



    }

}


int
run_net()
{
    int client_sock = 0;
    int flag = 0;
    int on = 1;
    int clien_len = sizeof(struct sockaddr);    
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    flag = setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));
    if(flag !=0){
        printf("err reuseaddr\n");
        exit(1);

    }

    //创建sockaddr_in结构体变量
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    ////将套接字和IP、端口绑定
    flag  = bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(flag < 0 ){
        printf("err bind\n");
        exit(1);
    }

    flag = listen(serv_sock, 5);
    if(flag <0){
        printf("err listen\n");
        exit(1);
    }


    int len = 0;
    uint8_t test_buf1[100] = {0};

    while(1){

        client_sock = accept(serv_sock, (struct sockaddr*)&serv_addr, &clien_len);
        if(client_sock >0){
            printf("client=%d\n", client_sock);
            if (fcntl(client_sock, F_SETFL, fcntl(client_sock, F_GETFL) | O_NONBLOCK) < 0) {
                perror("setting O_NONBLOCK");
                close(client_sock);
                exit(1);
            }
            client_fd_g = client_sock;            

            test_fun(base_yingxue);
        }
    }

}




void
test_unit(struct analysis_protocol_base_tag *base )
{
    int flag = 0;
    uint8_t buf[] = {0xEA, 0x1b, 0x11, 0x4d};

    flag = analysis_protocol_write_chain_list(base->chain_list, buf, sizeof(buf));

    printf("rec=%d\n", flag);
    flag = base->get_recv_frame_bc(base, NULL);
    printf("finish frame state=%d\n", flag);
    printf("flag=%d\n", flag);
    
    uint8_t buf_1[] = {   0x00, 0x00, 0x00, 0x2d, 0x10, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x79, 0x53, 0x11, 0x22,0x55 };

    flag = analysis_protocol_write_chain_list(base->chain_list, buf_1, sizeof(buf_1));

    printf("rec=%d\n", flag);
    flag = base->get_recv_frame_bc(base, NULL);
    printf("finish frame state=%d\n", flag);
    printf("flag=%d\n", flag);

}

int main()
{
    base_yingxue = analysis_protocol_init((void *)0, yingxue_frame_recv_fun, yingxue_process_frame, send_func_bc, check_reply_func ); 
    //test_unit(base_yingxue);
    run_net();
    printf("wudan\n");

}
