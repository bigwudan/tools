#define CHAIN_NODE_MAX 3
#define CHAIN_NODE_BUFFMAX 20


typedef struct _chain_node
{
	char buff[CHAIN_NODE_BUFFMAX];
	struct _chain_node *p_next_node;
}chain_node;

typedef struct _chain_linked
{
	chain_node *p_rear;
	chain_node *p_front;
	int node_count;
}chain_linked;


int create_chain_linked(chain_linked *);


int write_chain_linked(chain_linked *, char *);


int read_chain_linked(chain_linked *, char *);

int show_chain_linked(chain_linked *);
