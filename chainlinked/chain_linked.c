#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chain_linked.h"

int create_chain_list(ChainList *p_chain_list)
{
	p_chain_list->p_base = (ChainNode *)calloc(sizeof(ChainNode), CHAINMAX);	
	if(!p_chain_list->p_base){
		return -1;
	}
	p_chain_list->rear = p_chain_list->front = p_chain_list->count = 0;
	return 1;


}

int in_chain_list(ChainList *p_chain_list, char *p_val)
{
	int position = (p_chain_list->rear + 1)%CHAINMAX;
	if(position == p_chain_list->front){
		return 0;
	}
	char *p_cur = (char *)calloc(sizeof(char), strlen(p_val) + 1);
	if(!p_cur){
		return -1;
	}
	strncpy(p_cur, p_val, strlen(p_val));
	(p_chain_list->p_base + p_chain_list->rear)->p_val = p_cur;
	p_chain_list->rear = position;	
	return 1;



}

int out_chain_list(ChainList *p_chain_list, char **pp_val )
{
	if(p_chain_list->rear == p_chain_list->front){
		return 0;
	}
	char *p_val = (p_chain_list->p_base + p_chain_list->front)->p_val;
	*pp_val = (char *)calloc(sizeof(char), strlen(p_val)+1 );	
	if(!*pp_val){
		return -1;
	}
	
	strncpy(*pp_val, (p_chain_list->p_base + p_chain_list->front)->p_val, strlen(p_val));
	p_chain_list->front = (p_chain_list->front + 1)%CHAINMAX;
	free(p_val);
	return 1;
}
