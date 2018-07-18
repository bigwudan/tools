#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chain_linked.h"

int main()
{
	int flag = 0;
	char *buf1;
	char *buf2;
	char *buf3;
	char *buf4 = NULL;
	char *buf5;

	ChainList my_chain_list;
	flag = create_chain_list(&my_chain_list);
	printf("flag=%d\n", flag);
	flag = in_chain_list(&my_chain_list, "test1");
	printf("flag1=%d\n", flag);	

	flag = in_chain_list(&my_chain_list, "test2");
	printf("flag2=%d\n", flag);	

	flag = in_chain_list(&my_chain_list, "test3");
	printf("flag3=%d\n", flag);	
	
	flag = in_chain_list(&my_chain_list, "test4");
	printf("flag4=%d\n", flag);	
	flag = in_chain_list(&my_chain_list, "test5");
	printf("flag5=%d\n", flag);	
	
	flag = out_chain_list(&my_chain_list, &buf1);
	printf("flag=%d,buf1=%s\n", flag, buf1);
	free(buf1);
	
	flag = out_chain_list(&my_chain_list, &buf2);
	printf("flag=%d,buf2=%s\n", flag, buf2);

	free(buf2);
	flag = out_chain_list(&my_chain_list, &buf3);
	printf("flag=%d,buf3=%s\n", flag, buf3);

	free(buf3);

	flag = out_chain_list(&my_chain_list, &buf4);
	printf("flag=%d,buf4=%s\n", flag, buf4);
	free(my_chain_list.p_base);

}


