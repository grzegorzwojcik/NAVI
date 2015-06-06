/*
 * functions.c
 *
 *  Created on: Jun 6, 2015
 *      Author: Grzegorz
 */


/*
 * functionc.c
 *
 *  Created on: Sep 2, 2014
 *      Author: Grzegorz W�jcik
 */

#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "functions.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
void SYSTEM_ClockCheck(void){

	GV_SystemStatus = 0;
	RCC_ClocksTypeDef ClksFreq;
	RCC_GetClocksFreq(&ClksFreq);

	if( ClksFreq.SYSCLK_Frequency == 72000000 ){
		if( ClksFreq.HCLK_Frequency == 72000000 ){
			GV_SystemStatus = 1;
		}
	}
	else
		GV_SystemStatus = 0;
}

/*-- Interrupts --------------------------------------------------------------*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	GV_SystemCounter++;
	if( GV_SystemCounter >= 1000 )
		GV_SystemCounter = 0;
}





