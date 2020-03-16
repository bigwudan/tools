#include "heap.h"
#include <stdio.h>
int main()
{

    void *p = pvPortMalloc(10);
    printf("p=%p\n", p);
    vPortFree(p);


}
