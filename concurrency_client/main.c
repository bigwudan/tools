#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse_header_fsm.h"

int main(){
	int a=0;
	int b=0;
	int c = 0;
	int flag = parse_line(NULL,&a, &b, &c );
	printf("flag=%d\n", flag);

	printf("eee\n");

}
