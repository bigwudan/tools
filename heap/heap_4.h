#ifndef _HEAP_4_H
#define _HEAP_4_H

#include <stdlib.h>
#include <stdint.h>
#define portBYTE_ALIGNMENT  4

#define configTOTAL_HEAP_SIZE 10

#define portBYTE_ALIGNMENT_MASK ( 0x0007 )

#define vTaskSuspendAll()
#define	xTaskResumeAll()
#define mtCOVERAGE_TEST_MARKER()

#define	traceMALLOC( A, B )

#define vApplicationMallocFailedHook()

#define configASSERT(A)
#define traceFREE(A,B)


void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );


size_t xPortGetFreeHeapSize( void );

size_t xPortGetMinimumEverFreeHeapSize( void );
#endif

