#include <stdio.h>
#include <string.h>
#include "analysis_protocol.h"
#include "analysis_protocol_tools.h"

extern struct analysis_protocol_base_tag analysis_protocol_base;



int main()
{
    struct analysis_protocol_base_tag *base_yingxue = analysis_protocol_init((void *)0); 
   // struct analysis_protocol_base_tag *base_wifi = analysis_protocol_init((void *)1);

    struct yingxue_frame_tag *yingxue =  (struct yingxue_frame_tag *)(base_yingxue->recv_frame);

    yingxue->data1 = 1;
    yingxue->data2 = 2;
    yingxue->data3 = 3;


    struct wifi_frame_tag *wifi = (struct wifi_frame_tag *)(base_yingxue->recv_frame);
    wifi->wifi_1 = 1;
    wifi->wifi_2 = 2;


    printf("wudan\n");


}
