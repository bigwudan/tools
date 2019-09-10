#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>  
#include <stddef.h>  
#include <setjmp.h>  
#include <google/cmockery.h>  


#include "array.h"


/* Ensure add() adds two integers correctly. */  
void test_add(void **state) 
{  
	size_t flag = 0;
	size_t ndx = 0;
	data_unset *tmp_data = NULL;
	//建立一个
	array *p = array_init();
	assert_true(p);
	
	//生成一个
	tmp_data = create_data_unset("1", "11");		
	assert_true(tmp_data);
	array_insert_unique(p, tmp_data);	
	flag = array_get_index(p, tmp_data->key, strlen(tmp_data->key), &ndx);
	assert_int_equal(flag, 0);
	assert_int_equal(ndx, 0);


	//生成第二个

	tmp_data = create_data_unset("2", "22");		
	assert_true(tmp_data);
	array_insert_unique(p, tmp_data);	
	flag = array_get_index(p, tmp_data->key, strlen(tmp_data->key), &ndx);
	assert_int_equal(flag, 1);
	assert_int_equal(ndx, 1);
	
	//生成第三个

	tmp_data = create_data_unset("5", "55");		
	assert_true(tmp_data);
	array_insert_unique(p, tmp_data);	
	flag = array_get_index(p, tmp_data->key, strlen(tmp_data->key), &ndx);
	assert_int_equal(flag, 2);
	assert_int_equal(ndx, 2);

	//生成第四个 ，插入到2得位置
	tmp_data = create_data_unset("4", "44");		
	assert_true(tmp_data);
	array_insert_unique(p, tmp_data);	
	flag = array_get_index(p, tmp_data->key, strlen(tmp_data->key), &ndx);
	assert_int_equal(flag, 3);
	assert_int_equal(ndx, 2);

	tmp_data = create_data_unset("4", "77");		
	assert_true(tmp_data);
	array_insert_unique(p, tmp_data);	
	free_array(p);	
}  


int main()
{

    const UnitTest tests[] = {  
        unit_test(test_add),  
    };  
    return run_tests(tests);  


    printf("wudan\n");

}
