#include "array.h"

//初始化
array *
array_init(void) {
    array *a;
    a = calloc(1, sizeof(*a));
    return a;
}

//插入数据
void 
array_insert_unique(array *a, data_unset *entry) 
{
    data_unset **old;
    //如果存在就复制
    if (NULL != (old = array_find_or_insert(a, entry))) {
        if((*old)->value){
            free((*old)->value);
        }        
        (*old)->value = strdup(entry->value);
        free(entry->key);
        free(entry->value);
        free(entry);
    }
    return ;
}

//查找数据,并且插入数据
static 
data_unset **array_find_or_insert(array *a, data_unset *entry) 
{
    size_t ndx, pos, j;
    /* try to find the entry */
    if (-1 != (ndx = array_get_index(a, entry->key, strlen(entry->key), &pos))) {
        /* found collision, return it */
        return &a->data[ndx];
    }

    /* insert */

    /* there couldn't possibly be enough memory to store so many entries */

    if (a->size == a->used) {
        a->size  += 16;
        a->data   = realloc(a->data,   sizeof(*a->data)   * a->size);
        a->sorted = realloc(a->sorted, sizeof(*a->sorted) * a->size);
        for (j = a->used; j < a->size; j++) a->data[j] = NULL;
    }

    ndx = a->used;

    /* make sure there is nothing here */
    if (a->data[ndx]){
        free(a->data[ndx]);
    } 

    a->data[a->used++] = entry;

    /* move everything one step to the right */
    if (pos != ndx) {
        memmove(a->sorted + (pos + 1), a->sorted + (pos), (ndx - pos) * sizeof(*a->sorted));
    }
    /* insert */
    a->sorted[pos] = ndx;
    return NULL;
}


//查找数据
static size_t 
array_get_index(const array *a, const char *key, size_t keylen, size_t *rndx) {
    size_t lower = 0, upper = a->used;

    while (lower != upper) {
        size_t probe = (lower + upper) / 2;
        const char *b = a->data[a->sorted[probe]]->key;
        //int cmp = array_keycmp(key, keylen, CONST_BUF_LEN(b));

        int cmp = strcmp(key, b);
        if (cmp == 0) {
            /* found */
            if (rndx) *rndx = probe;
            return a->sorted[probe];
        } else if (cmp < 0) {
            /* key < [probe] */
            upper = probe; /* still: lower <= upper */
        } else {
            /* key > [probe] */
            lower = probe + 1; /* still: lower <= upper */
        }
    }
    /* not found: [lower-1] < key < [upper] = [lower] ==> insert at [lower] */
    if (rndx) *rndx = lower;
    return -1;
}


//生成data_unset
data_unset *
create_data_unset(char *key, char *value)
{
    data_unset *p = calloc(1, sizeof(data_unset));    
    if(p == NULL) return NULL;
    p->key = strdup(key);
    p->value = strdup(value);
    return p;
}
//释放data_unset
void 
free_data_unset(data_unset *p)
{
    if(p->key) free(p->key);
    if(p->value) free(p->value);
    free(p);
}






