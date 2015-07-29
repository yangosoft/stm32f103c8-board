/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>







static void delay(volatile uint32_t t)
{
     while(t > 0)
 {
 asm("nop");
 t--;
 }
    
// 	t *= 1000;
// 	while (t--) ;
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	/* This function will get called if a task overflows its stack.   If the
	parameters are corrupt then inspect pxCurrentTCB to find which was the
	offending task. */

	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; )
        {
            gpio_toggle(GPIOA, GPIO3);
           // gpio_toggle(GPIOA, GPIO2);
            delay(500000);
            gpio_toggle(GPIOA, GPIO3);
            //gpio_toggle(GPIOA, GPIO2);
            delay(500000);
        }
}

#if( configUSE_MALLOC_FAILED_HOOK == 1 )

void vApplicationMallocFailedHook( TaskHandle_t pxTask, char *pcTaskName )
{
    for( ;; )
        {
            //gpio_toggle(GPIOA, GPIO3);
            gpio_toggle(GPIOA, GPIO2);
            delay(500000);
            //gpio_toggle(GPIOA, GPIO3);
            gpio_toggle(GPIOA, GPIO2);
            delay(500000);
        }
}
#endif



void sv_call_handler(void)
{
      vPortSVCHandler();
}
 void pend_sv_handler(void)
{
      xPortPendSVHandler();
}
void sys_tick_handler(void)
{
      xPortSysTickHandler();
} 









