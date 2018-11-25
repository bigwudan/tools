#ifndef CONFIG_H
#define CONFIG_H

#define PROCESS_NUM 1
#define THREAD_NUM 2
#define EXPIRED_TIME 30
#define THREAD_TASK_NUM 30
#define CONCURRENCY_NUM 10
#define IP "114.215.85.234"
#define PORT 80

const char* request = "GET http://www.bigwudan.com/index.html HTTP/1.1\r\nConnection: keep-alive\r\n\r\nxxxxxxxxxxxx";

char *p_request = "GET /index.html HTTP/1.1\r\nHost: www.bigwudan.com\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.87 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9\r\nIf-None-Match: \"10-57a9977776192\"\r\nIf-Modified-Since: Wed, 14 Nov 2018 05:47:16 GMT\r\n\r\nxxxxxxxxxxx";

char *p_1_request = "GET /index.html HTTP/1.1\r\nHost: www.bigwudan.com\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.87 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9\r\n\r\nxxxxxxxxxxx";


char *p_2_request = "GET /index.php?id=%d HTTP/1.1\r\nHost: www.bigwudan.com\r\n\r\nxxxxxxxxxxx";


#endif
