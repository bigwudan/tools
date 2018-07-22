#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chain_linked.h"



int create_chain_linked(chain_linked *p_chain_linked )
{
	p_chain_linked->node_count = CHAIN_NODE_MAX;
	int i = 0;
	chain_node *p_node = NULL;	 
	chain_node *p_old_node = NULL;
	chain_node *p_frist_node = NULL;
	for(i = 0; i < CHAIN_NODE_MAX; i++){
		p_node = (chain_node *)calloc(1, sizeof(chain_node));	
			
		if(!p_node){
			return -1;
		}
	

		p_node->p_next_node = NULL;

		if(i == 0){
			p_frist_node = p_node;
		}

		if(p_old_node){
			p_old_node->p_next_node = p_node;
		
		}
		p_old_node = p_node;
	
	}
	p_node->p_next_node = p_frist_node;
	p_chain_linked->p_rear = p_frist_node;
	p_chain_linked->p_front = p_frist_node;
	return 1;
}

int show_chain_linked(chain_linked *p_chain_linked)
{
	chain_node *p_node = p_chain_linked->p_rear;
	int i=0;
	for(i=0; i< p_chain_linked->node_count; i++){
		printf("i=%d, p_node=%p, p_next=%p\n",i , p_node, p_node->p_next_node);
		p_node = p_node->p_next_node;
	}
	return 1;
}

int read_chain_linked(chain_linked *p_chain_linked, char *p_node)
{
	if(p_chain_linked->p_rear == p_chain_linked->p_front){
		return 0;
	}
	strcpy(p_node, p_chain_linked->p_rear->p_next_node->buff);
	p_chain_linked->p_rear = p_chain_linked->p_rear->p_next_node;
	return 1;
}

int write_chain_linked(chain_linked *p_chain_linked, char *p_node)
{
	if(p_chain_linked->p_front->p_next_node == p_chain_linked->p_rear){
		return 0;
	}
	strcpy(p_chain_linked->p_front->p_next_node->buff, p_node);
	p_chain_linked->p_front = p_chain_linked->p_front->p_next_node; 
	return 1;

}
















