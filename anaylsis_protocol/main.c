#include <stdio.h>
#include <string.h>
#include "analysis_protocol_tools.h"

struct chain_list_tag chain_list;

int main()
{
    create_chain_list((&chain_list));
    int max = 10;
    int flag = 0;
    uint8_t data = 0x00;

    for(int i =0; i <10; i++){
        //判断是否满
        is_full_chain_list( (&chain_list) , flag );
        if(flag != 0){
            in_chain_list( (&chain_list), i ); 
            printf("add i=%d \n", i);
        }else{
            printf("just full i=%d \n", i);
        }
    }

    
    for(int i=0; i< 10; i++){
        //是否有数据
        is_empty_chain_list( (&chain_list), flag);
        if( flag !=0 ){
            out_chain_list((&chain_list), data);
            printf("data=0x%02X\n", data);

        
        }else{
        
            printf("over\n");
        }



    
    
    }





    printf("wudan\n");


}
