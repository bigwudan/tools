#define CHAINMAX 4


typedef struct chain_node
{
	char *p_val;

}ChainNode;

typedef struct chain_list
{
	int rear;
	int front;
	int count;
	ChainNode *p_base;
}ChainList;


int create_chain_list(ChainList *);

int in_chain_list(ChainList *, char *);

int out_chain_list(ChainList *, char **);





