/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : Quadcopter navigation board main function.
**
**  Author		: Grzegorz WÓJCIK
**
**  Environment : Atollic TrueSTUDIO/STM32
**
*****************************************************************************
*/

/* Includes */
#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "functions.h"
#include "FAULTS.h"

/* Private typedef */
 USART_InitTypeDef USART_InitStructure;
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */


int main(void)
{


	/* TODO - Add your application code here */
	//HSEStartUpStatus = RCC_WaitForHSEStartUp();
	SYSTEM_ClockCheck();
	SysTick_Config(SystemCoreClock/1000);	// SysTick 1 kHZ (1ms interval)



	/* Enter infinite loop only when clock frequencies are OK */
	if( GV_SystemStatus == 1 ){
	FAULTS_Servo_initGPIO();
	FAULTS_Servo_initTIM();
	TIM2->CCR3 = 1500;

		while (1)
		{

		}
	}

	else
		while(1);
}



/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}
