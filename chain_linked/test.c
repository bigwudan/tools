#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chain_linked.h"


int main()
{
	int flag = 0;
	char buf[30];
	chain_linked my_chain_linked;

	flag = create_chain_linked(&my_chain_linked);


	printf("create flag=%d\n", flag);

	flag = write_chain_linked(&my_chain_linked, "111");
	printf("w flag=%d\n", flag);


	flag = write_chain_linked(&my_chain_linked, "222");
	printf("w flag=%d\n", flag);

	flag = write_chain_linked(&my_chain_linked, "333");
	printf("w flag=%d\n", flag);



	flag = read_chain_linked(&my_chain_linked, buf);
	printf("r flag=%d buff=%s\n", flag, buf);

	flag = read_chain_linked(&my_chain_linked, buf);
	printf("r flag=%d buff=%s\n", flag, buf);

	flag = read_chain_linked(&my_chain_linked, buf);
	printf("r flag=%d buff=%s\n", flag, buf);

	flag = read_chain_linked(&my_chain_linked, buf);
	printf("r flag=%d buff=%s\n", flag, buf);
}

