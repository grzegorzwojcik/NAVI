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
#include "BTM.h"
#include "CONTROLLER.h"
#include "FAULTS.h"

/* Private typedef */
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */


int main(void)
{
	/* TODO - Add your application code here */
	PLL_Configurattion();
	SYSTEM_ClockCheck();

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(2,2,0));
	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(2,1,2));


	BTM_initRCC();
	BTM_initGPIO();
	BTM_initUART();
	BTM_ClearBuffor();

	/* Enter infinite loop only when clock frequencies are OK */
	if( GV_SystemStatus == 1 ){
		FAULTS_initRCC();
		FAULTS_Servo_initGPIO();
		FAULTS_Servo_initTIM();		// 50 Hz PWM (together with control loop below

		CTRL_initNaviStruct();
		CTRL_initTIM();				// 50 Hz control loop
		CTRL_initNAVI_RCC();		// 50 Hz Autopilot PWMs
		CTRL_initNAVI_GPIO();		// 50 Hz Autopilot PWMs
		CTRL_initNAVI_PWM();		// 50 Hz Autopilot PWMs
		SysTick_Config(SystemCoreClock/1000);	// SysTick 1 kHZ (1ms interval)

		while (1)
		{

			if(GV_flag_BTMRX == 1 ){
				if( BTM_checkCRC(BTM_DF_CHAR, BTM_BUFFOR_LENGTH) == 1 ){
					CTRL_DataProcess();
					USART_puts(USART1, "Structure updated"+0x0D + 0x0A);
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				}
				else{
					USART_puts(USART1, "Data frame error"+0x0D + 0x0A);
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				}
				BTM_ClearBuffor();
				GV_flag_BTMRX = 0;
			}
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
