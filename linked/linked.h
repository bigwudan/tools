#ifndef _LINKED_H
#define _LINKED_H

typedef struct linked
{
	char *p_value;
	struct linked_node *p_next;
}linked_node;

struct linked_list
{
	linked_node *p_top;
};

int insert_linked_node(struct linked_list *, char *p_val );

void pop_linked_node(struct linked_list *, char *p_val);

void clean_linked_node(struct linked_list *);

int create_linked_list(struct linked_list *);

void show_node_list(struct linked_list *);

#endif
