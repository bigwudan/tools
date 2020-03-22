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
#include <errno.h>
#include <sys/time.h>

#define MAXLINE 100

unsigned char test_buf[] = {
	//[0][0] //[0][1]                                                 //erno
	/*0xEA, 0x1B, 0x10, 0x4D, 0x00, 0x00, 0x00, 0x2D, 0x10, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x79, 0x53,*/
	0xEA, 0x1B, 0x11, 0x01, 0x00, 0x00, 0x00, 0x1E, 0x0A, 0x2A, 0x28, 0x26, 0x2A, 0x00, 0x00, 0x48, 0x35,
	/*0xEA, 0x1B, 0x12, 0x00, 0xCD, 0x80, 0x9E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x05, 0x4B,
	0xEA, 0x1B, 0x13, 0x00, 0x00, 0x05, 0x40, 0x50, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBE, 0x5F,*/
};
unsigned char test_buf_1[] = {
	//[0][0] //[0][1]                                                 //erno
	0xEA, 0x1B, 0x11, 0x02, 0x00, 0x00, 0x00, 0x1E, 0x0A, 0x2A, 0x28, 0x26, 0x2A, 0x00, 0x00, 0x48, 0x35,
};
int
client_connect()
{
    int    sockfd, n,rec_len;  
    char    recvline[4096], sendline[4096];  
    char    buf[MAXLINE];  
    struct sockaddr_in    servaddr;  
    char *addr = "127.0.0.1";
    int len = 0;
    uint8_t read_buf[100] = {0};




    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);  
        exit(0);  
    }  


    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(1234);  
    if( inet_pton(AF_INET, addr, &servaddr.sin_addr) <= 0){  
        printf("inet_pton error for %s\n",addr);  
        exit(0);  
    }  
                      

    if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){  
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  

    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
    struct timeval old_tm;
    struct timeval new_tm;
    uint32_t dur_tm = 0;
    gettimeofday(&old_tm, NULL);
    while(1){
        gettimeofday(&new_tm, NULL);
        dur_tm = new_tm.tv_sec -  old_tm.tv_sec;


        if(dur_tm >=3){
            len = write(sockfd, test_buf_1, sizeof(test_buf_1));
            printf("***************over\n");
        }else{
            len = write(sockfd, test_buf, sizeof(test_buf));
        }
        



        printf("write =%d\n", len);
        sleep(1);
        len = read(sockfd, read_buf, sizeof(read_buf));
        if(len >0){
            printf("read len=%d, ", len);
            for(int i=0; i<len; i++){
                printf(" 0x%02X ", read_buf[i]);
            }
            printf("\r\n");
        }else{
            printf("read over\n");
        }
    
    }


    printf("send msg to server: \n");  



}

int main()
{
    client_connect();
    printf("wudan\n");
}


