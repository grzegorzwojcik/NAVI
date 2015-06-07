/*
 * BTM.c
 *
 *  Created on: Jun 7, 2015
 *      Author: Grzegorz
 */


#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "BTM.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
void BTM_initRCC(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
}

void BTM_initGPIO(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = BTM_USART1_TX | BTM_USART1_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BTM_PORT, &GPIO_InitStructure);

}
