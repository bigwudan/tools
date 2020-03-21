#include <stdio.h>
#include <stdlib.h>
#include "kfifo.h"


int main()
{
    struct kfifo *p_kfifo = NULL;
    p_kfifo = kfifo_alloc(1000); 
    int len = 0;
    if(p_kfifo == NULL){
        printf("p_kfifo alloc err\n");
        exit(1);
    }

    unsigned char put_buf[] = {0x11, 0x22, 0x33};
    unsigned char get_buf[10] = {0};
    
    printf("put len=%d\n", kfifo_put(p_kfifo, put_buf, sizeof(put_buf)));

    printf("get len=%d\n", len = kfifo_get(p_kfifo, get_buf, sizeof(get_buf)));

    for(int i=0; i < len; i++){

        printf("data=0x%02X ", get_buf[i]);

    }

    printf("get len=%d\n", len = kfifo_get(p_kfifo, get_buf, sizeof(get_buf)));


    printf("ok\n");

}
