/*
    FreeRTOS V8.2.1 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks
 * (which just exist to test the kernel port and provide an example of how to use
 * each FreeRTOS API function).
 *
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Check" task - This only executes every five seconds but has the highest
 * priority so is guaranteed to get processor time.  Its main function is to
 * check that all the standard demo tasks are still operational. The check task
 * will toggle LED 3 (PB11) every five seconds so long as no errors have been
 * detected.  The toggle rate will increase to half a second if an error has
 * been found in any task.
 *
 * "Echo" task - This is a very basic task that simply echoes any characters
 * received on COM0 (USART1).  This can be tested by transmitting a text file
 * from a dumb terminal to the STM32 USART then observing or capturing the text
 * that is echoed back.  Missing characters will be all the more obvious if the
 * file contains a simple repeating string of fixed width.
 *
 * Currently this demo does not include interrupt nesting examples.  High
 * frequency timer and simpler nesting examples can be found in most Cortex-M3
 * demo applications.
 *
 * The functions used to initialise, set and clear LED outputs are normally
 * defined in partest.c.  This demo includes two partest files, one that is
 * configured for use with the Keil MCBSTM32 evaluation board (called
 * ParTest_MCBSTM32.c) and one that is configured for use with the official
 * ST Eval board (called ParTest_ST_Eval.c).  One one of these files should be
 * included in the build at any one time, as appropriate for the hardware
 * actually being used.
 */


/* Standard includes. */
#include <string.h>
#include <stdio.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/usart.h>



/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Library includes. */
// #include "stm32f10x_it.h"





/* Demo app includes. */
// #include "BlockQ.h"
// #include "integer.h"
// #include "flash.h"
// #include "partest.h"
// #include "semtest.h"
// #include "GenQTest.h"
// #include "QPeek.h"
// #include "recmutex.h"

/* Driver includes. */
#include <a.h>


/* The time between cycles of the 'check' task - which depends on whether the
check task has detected an error or not. */
#define mainCHECK_DELAY_NO_ERROR			( ( TickType_t ) 5000 / portTICK_PERIOD_MS )
#define mainCHECK_DELAY_ERROR				( ( TickType_t ) 500 / portTICK_PERIOD_MS )

/* The LED controlled by the 'check' task. */
#define mainCHECK_LED						( 3 )

/* Task priorities. */
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainECHO_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY )

/* COM port and baud rate used by the echo task. */
#define mainCOM0							( 0 )
#define mainBAUD_RATE						( 115200 )

/*-----------------------------------------------------------*/








/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );

/* The 'check' task as described at the top of this file. */
static void prvCheckTask( void *pvParameters );

/* A simple task that echoes all the characters that are received on COM0
(USART1). */
static void prvUSARTEchoTask( void *pvParameters );





static void usart_setup(void)
{
   
    rcc_periph_clock_enable(RCC_USART1);
    
    /* Setup GPIO pin GPIO_USART1_TX/GPIO9 on GPIO port A for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,     GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
        
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	/* Setup UART parameters. */
	// usart_set_baudrate(USART1, 38400);
	/* TODO usart_set_baudrate() doesn't support 24MHz clock (yet). */
	/* This is the equivalent: */
// 	USART_BRR(USART1) = (uint16_t)((72000000 << 4) / (9600 * 16));
        usart_set_baudrate(USART1, 9600);
        
        
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
        nvic_enable_irq(NVIC_USART1_IRQ);
	usart_enable_rx_interrupt(USART1);
        
        
	usart_enable(USART1);
}








void delay(volatile uint32_t t)
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

static volatile uint16_t charReaded = 'H';

extern "C" int _write(int file, char *ptr, int len);


int main( void )
{
    
    
#ifdef DEBUG
  debug();
#endif

	/* Set up the clocks and memory interface. */
	prvSetupHardware();

        
        
        
        
        
        
        
        

        printf("Setup ok\n");


	/* Create the 'echo' task, which is also defined within this file. */
	xTaskCreate( prvUSARTEchoTask, "Echo", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

        
        
	/* Create the 'check' task, which is also defined within this file. */
	xTaskCreate( prvCheckTask, "Check", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
        
        
        printf("Start scheduler\n");

    /* Start the scheduler. */
	vTaskStartScheduler();

        
        
        
    /* Will only get here if there was insufficient memory to create the idle
    task.  The idle task is created within vTaskStartScheduler(). */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Described at the top of this file. */
static void prvCheckTask( void *pvParameters )
{
TickType_t xLastExecutionTime;
unsigned long ulTicksToWait = mainCHECK_DELAY_NO_ERROR;

	/* Just to remove the compiler warning about the unused parameter. */
	( void ) pvParameters;

	/* Initialise the variable used to control our iteration rate prior to
	its first use. */
	xLastExecutionTime = xTaskGetTickCount();

        
        
        
        
        
        
 while(1)
 {
     
     
     
     
        //usart_send(USART1,charReaded);
        printf("HOla mundo: %c\n", charReaded);
     
     
        gpio_toggle(GPIOA, GPIO2);
        delay(1000000);
        gpio_toggle(GPIOA, GPIO2);
        delay(1000000);
        
 	vTaskDelayUntil( &xLastExecutionTime, ulTicksToWait );
 }
        
        
}
/*-----------------------------------------------------------*/

/* Described at the top of this file. */
static void prvUSARTEchoTask( void *pvParameters )
{
       
    
	/* Just to avoid compiler warnings. */
	//( void ) pvParameters;
        TickType_t xLastExecutionTime;
        unsigned long ulTicksToWait = mainCHECK_DELAY_NO_ERROR;
        xLastExecutionTime = xTaskGetTickCount();
        
 while(1)
 {
        gpio_toggle(GPIOA, GPIO3);
        delay(1000000);
        gpio_toggle(GPIOA, GPIO3);
        delay(1000000);
        
        
 	vTaskDelayUntil( &xLastExecutionTime, ulTicksToWait );
 }
        

	/* Initialise COM0, which is USART1 according to the STM32 libraries. */
// 	lCOMPortInit( mainCOM0, mainBAUD_RATE );
// 
// 	/* Try sending out a string all in one go, as a very basic test of the
//     lSerialPutString() function. */
//     lSerialPutString( mainCOM0, pcLongishString, strlen( pcLongishString ) );
// 
// 	for( ;; )
// 	{
// 		/* Block to wait for a character to be received on COM0. */
// 		xSerialGetChar( mainCOM0, &cChar, portMAX_DELAY );
// 
// 		/* Write the received character back to COM0. */
// 		xSerialPutChar( mainCOM0, cChar, 0 );
// 	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    
    
    /* Set STM32 to 24 MHz. */

    
    
    //rcc_clock_setup_in_hse_8mhz_out_24mhz();

    rcc_clock_setup_in_hse_8mhz_out_72mhz();
	/* Enable GPIOC clock. */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
        
        /*Enable GPIOA clock */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	/*set GPIOA0 as input open-drain */
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO13);
        
        
        rcc_periph_clock_enable(RCC_GPIOA);
        
    
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO3);
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
        
    
        usart_setup();
}

/*-----------------------------------------------------------*/

void assert_failed( unsigned char *pucFile, unsigned long ulLine )
{
	( void ) pucFile;
	( void ) ulLine;

	for( ;; )
        {
 
 
        gpio_toggle(GPIOA, GPIO3);
        delay(1000000);
        gpio_toggle(GPIOA, GPIO3);
        delay(1000000);
        gpio_toggle(GPIOA, GPIO3);
        delay(1000000);
        }
            
            
}



extern "C" int _write(int file, char *ptr, int len) {
    

    
    int n;
        for (n = 0; n < len; n++) 
        {
               //gpio_toggle(GPIOA, GPIO2);
            
            
            //usart_send_blocking(USART1,  (ptr[n] & 0x00FF) );
            usart_send_blocking(USART1, *ptr);
            
            
            ptr++;
        }
    
    return len;
}







extern "C" void usart1_isr(void)
{
	


	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		


		charReaded = usart_recv(USART1);
                usart_send(USART1, charReaded);

		
	}

	
}



