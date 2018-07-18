#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked.h"


int create_linked_list(struct linked_list * p_linked_list)
{
	linked_node *p_head = (linked_node *)malloc(sizeof(linked_node));
	if(!p_head){
		return -1;
	}
	p_head->p_value = NULL;
	p_head->p_next = NULL;
	p_linked_list->p_top = p_head;
	return 0;

}



int insert_linked_node(struct linked_list * p_linked_list, char *p_val )
{
	linked_node * p_old_top = p_linked_list->p_top;
	linked_node *p_node = (linked_node *)malloc(sizeof(linked_node));
	if(!p_node){
		return -1;
	}
	p_node->p_value = malloc(sizeof(char) * (strlen(p_val) + 1));
	memset(p_node->p_value, 0, strlen(p_val) + 1);
	
	strncpy(p_node->p_value, p_val, strlen(p_val));

	if(!p_node->p_value){
		return -1;
	}
	p_node->p_next = p_old_top->p_next;	
	p_linked_list->p_top->p_next = p_node;
	return 0;
}

void show_node_list(struct linked_list * p_list)
{
	linked_node *p_node = p_list->p_top->p_next;
	while(p_node){
		printf("value=%s\n", p_node->p_value);
		p_node = p_node->p_next;
	}	
}
