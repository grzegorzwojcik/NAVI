/*
 * BTM.c
 *
 *  Created on: Jun 7, 2015
 *      Author: Grzegorz WÓJCIK
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
}

void BTM_initGPIO(void){
	/* STM32F103RBT6
	 * USART1:
	 * TX: PA9
	 * RX: PA10 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = BTM_USART1_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BTM_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BTM_USART1_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(BTM_PORT, &GPIO_InitStructure);
}

void BTM_initUART(void){

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 			// Enable the USART1 receive and transmit interrupt
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 			// Enable the USART1 receive and transmit interrupt

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);

	//USART_ClearFlag(USART1, USART_FLAG_RXNE);
	//USART_ClearFlag(USART1, USART_FLAG_TXE);
	USART_Cmd(USART1, ENABLE);
	GV_flag_BTMRX = 0;
	GV_flag_BTMTX = 0;
}

/* Clearing BTM buffor */
void BTM_ClearBuffor(){
	uint8_t tmp = 0;
	for (tmp = 0; tmp < BTM_BUFFOR_LENGTH; tmp++){
		GV_bufforBTM[tmp] = 0;
	}
}

/* Sending a string */
void USART_puts(USART_TypeDef* USARTx, volatile char *s){
	while(*s){
		// wait until data register is empty
		while( !(USARTx->SR & 0x00000040) );
		USART_SendData(USARTx, *s);
		*s++;
	}
}

/*
* @brief Function Name  : Checksum function
* @brief Description    : This function generates CRC of the GV_bufforBTM string. After detecting 'StartChar' it starts
* 							generating the CRC and stops when '*' is detected. The CRC does not consider
* 							'StartCHar', nor '*' sign.
* @param StartChar		: Char, after which CRC generation is started
* @param Length			: String length
* @return				: uint8_t CRC
*
* @example				: CRC of the: "#,2,0,0,1,600,* is 53 (decimal)"
* @INFO						: this function uses global variable GV_bufforBTM[]
*/
uint8_t BTM_calculateCRC(char StartChar, uint8_t Length){
	uint8_t XOR = 0;	// set CRC to 0
	uint8_t i = 0;

	for( XOR = 0, i = 0; i < Length; i++ ){
		if(GV_bufforBTM[i] == '*')
			break;
		if(GV_bufforBTM[i] != StartChar)
			XOR ^= GV_bufforBTM[i];
	}

	return XOR;
}

/*
* @brief Function Name  	: Checksum check
* @brief Description    	: This function compares CRC included in received data frame with the generated one.
* @param GeneratedChecksum	: Checksum generated for example by BTM_calculateCRC
* @return 					: 0 = CRCs do not match, 1 = CRCs match
*
* @INFO						: this function uses global variable GV_bufforBTM[]
*/

uint8_t BTM_checkCRC(char StartChar, uint8_t Length){
	static uint8_t XOR = 0;					// calculated checksum
	static uint8_t i = 0, j = 0, tmp = 0;
	char checksum[4] = {0};					// received checksum
	static uint8_t status = 0;				// 0 - CRCs do not match, 1 - CRCs match

	for( XOR = 0, i = 0, j = 0, tmp = 0, status = 0; i < Length; i++ ){
		if( tmp == 1 ){
			checksum[j] = GV_bufforBTM[i];
			j++;
			if( j >= 3 )
				break;
		}
		if(GV_bufforBTM[i] == '*')
			tmp = 1;
		if( (GV_bufforBTM[i] != StartChar) && (tmp != 1) )		// XOR is calculated for chars from StartChar up to '*'
			XOR ^= GV_bufforBTM[i];
	}

	/* 0 - CRCs do not match, 1 - CRCs match */
	if(atoi(checksum) == XOR )
		status = 1;
	else
		status = 0;
	return status;
}

void USART1_IRQHandler(void){

	if( USART_GetFlagStatus(USART1, USART_FLAG_RXNE ) != RESET ){
		static uint8_t cnt = 0;
		char t = USART1->DR; 		// Received character from USART1 data register is saved in

		if( GV_flag_BTMRX == 0 ){
			if( t == '%' ){			// Data frame first sign
				cnt = 0;
				GV_bufforBTM[cnt] = t;
				cnt++;
			}
			if( (t != '%') && (GV_bufforBTM[0] == '%') ){
				GV_bufforBTM[cnt] = t;
				cnt++;

				if( (t == '\n') || ( cnt >= BTM_BUFFOR_LENGTH-1 ) ){
					cnt = 0;
					GV_flag_BTMRX = 1;
					USART_ClearFlag(USART1, USART_FLAG_ORE | USART_FLAG_PE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_TXE);
				}
			}
		}
	}
}


