#ifndef _HEAP_H
#define _HEAP_H
#include <stdint.h>
#define configTOTAL_HEAP_SIZE 4096
#define portBYTE_ALIGNMENT 8
#define portBYTE_ALIGNMENT_MASK ( 0x0007 )

#define portPOINTER_SIZE_TYPE uint64_t

#define vTaskSuspendAll()

#define	xTaskResumeAll()

void *pvPortMalloc(uint32_t xWantedSize);

void vPortFree(void *pv);


#endif
