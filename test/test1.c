#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

	char *p_buf[] = {
		"111",
		"222"
	
	};

	char **pp_test = NULL;
	pp_test = p_buf;


	printf("test = %s , test1=%s\n", *pp_test, *pp_test+1);
}
