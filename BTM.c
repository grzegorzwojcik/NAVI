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
	/* STM32F103RBT6
	 * USART1:
	 * TX: PA9
	 * RX: PA10 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = BTM_USART1_TX | BTM_USART1_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BTM_PORT, &GPIO_InitStructure);
}

void BTM_initUART(void){

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 			// Enable the USART2 receive and transmit interrupt

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	USART_Cmd(USART2, ENABLE);
}
