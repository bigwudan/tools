#include <string.h>
#include "analysis_protocol.h"
#include "analysis_protocol_tools.h"

struct chain_list_tag chain_list;

struct analysis_protocol_base_tag analysis_protocol_base;

//初始化
void 
analysis_protocol_init()
{
    //初始化
    memset(&analysis_protocol_base, 0, sizeof(struct analysis_protocol_base_tag));    
    //赋值给链表
    analysis_protocol_base.chain_list = &chain_list;
    //初始化发送命令
    TAILQ_INIT(&analysis_protocol_base.send_frame_head);  
    TAILQ_INIT(&analysis_protocol_base.send_frame_dest_head);  
}


//写入的数据，返回已经写入字节数
uint8_t
analysis_protocol_write_chain_list(struct chain_list_tag *chain_list, uint8_t *src, uint8_t len )
{
    uint8_t send_len = 0;
    uint8_t flag = 0;
    for(uint8_t i=0; i< len; i++){
        //判断是否满
        is_full_chain_list(chain_list , flag );
        if(flag != 0){
            in_chain_list(chain_list, src[i] ); 
            send_len++;
        //已经写满    
        }else{
            break;
        }
    }

    return send_len;

}

//读出的数据，返回已经写入字节数
uint8_t
analysis_protocol_read_chain_list(struct chain_list_tag *chain_list, uint8_t *src, uint8_t len )
{
    uint8_t send_len = 0;
    uint8_t flag = 0;

    for(int i=0; i< len; i++){
        //是否有数据
        is_empty_chain_list(chain_list, flag);
        if( flag !=0 ){
            out_chain_list(chain_list, (src[i]));
            send_len++;
        }else{
            break;
        }
    }
    return send_len;
}
