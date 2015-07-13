/*
 * AUTOPILOT.c
 *
 *  Created on: Jul 8, 2015
 *      Author: Grzegorz WÓJCIK
 */


#include "AUTOPILOT.h"



/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
void AP_initRCC(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

void AP_initGPIO(void){
	/* STM32F103RBT6
	 * USART2:
	 * TX: PA2
	 * RX: PA3 */
	/*	PA0 & PA1 are run through pins for AP TX & RX lines	*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = AP_USART2_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AP_USART2_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void AP_initUART(void){

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 			// Enable the USART2 receive and transmit interrupt
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 			// Enable the USART2 receive and transmit interrupt
	USART_Cmd(USART2, ENABLE);

	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);

	//USART_ClearFlag(USART2, USART_FLAG_RXNE);
	//USART_ClearFlag(USART2, USART_FLAG_TXE);
	GV_flag_APRX = 0;
	GV_flag_APTX = 0;
}

/* Clearing AP buffor */
void AP_ClearBuffer(){
	uint8_t tmp = 0;
	for (tmp = 0; tmp < AP_BUFFER_LENGTH; tmp++){
		GV_bufferAP[tmp] = 0;
	}
}

/*
* @brief Function Name  : Checksum function
* @brief Description    : This function generates CRC of the GV_bufferAP string. After detecting 'StartChar' it starts
* 							generating the CRC and stops when '*' is detected. The CRC does not consider
* 							'StartCHar', nor '*' sign.
* @param StartChar		: Char, after which CRC generation is started
* @param Length			: String length
* @return				: uint8_t CRC
*
* @example				: CRC of the: "#,2,0,0,1,600,* is 53 (decimal)"
* @INFO						: this function uses global variable GV_bufferAP[]
*/
uint8_t AP_calculateCRC(char StartChar, uint8_t Length){
	uint8_t XOR = 0;	// set CRC to 0
	uint8_t i = 0;

	for( XOR = 0, i = 0; i < Length; i++ ){
		if(GV_bufferAP[i] == '*')
			break;
		if(GV_bufferAP[i] != StartChar)
			XOR ^= GV_bufferAP[i];
	}

	return XOR;
}

/*
* @brief Function Name  	: Checksum check
* @brief Description    	: This function compares CRC included in received data frame with the generated one.
* @param GeneratedChecksum	: Checksum generated for example by AP_calculateCRC
* @return 					: 0 = CRCs do not match, 1 = CRCs match
*
* @INFO						: this function uses global variable GV_bufferAP[]
*/

FlagStatus AP_checkCRC(char StartChar, uint8_t Length){
	static uint8_t XOR = 0;					// calculated checksum
	static uint8_t i = 0, j = 0, tmp = 0;
	char checksum[4] = {0};					// received checksum

	for( XOR = 0, i = 0, j = 0, tmp = 0; i < Length; i++ ){
		if( tmp == 1 ){
			checksum[j] = GV_bufferAP[i];
			j++;
			if( j >= 3 )
				break;
		}
		if(GV_bufferAP[i] == '*')
			tmp = 1;
		if( (GV_bufferAP[i] != StartChar) && (tmp != 1) )		// XOR is calculated for chars from StartChar up to '*'
			XOR ^= GV_bufferAP[i];
	}

	/* 0 - CRCs do not match, 1 - CRCs match */
	if(atoi(checksum) == XOR)
		return SET;
	else
		return RESET;
}

void USART2_IRQHandler(void){

	if( USART_GetFlagStatus(USART2, USART_FLAG_RXNE ) != RESET ){
		static uint8_t cnt = 0;
		char t = USART2->DR; 		// Received character from USART2 data register is saved in

		if( GV_flag_APRX == 0 ){
			if( t == AP_DF_CHAR ){			// Data frame first sign
				cnt = 0;
				GV_bufferAP[cnt] = t;
				cnt++;
			}
			if( (t != AP_DF_CHAR) && (GV_bufferAP[0] == AP_DF_CHAR) ){
				GV_bufferAP[cnt] = t;
				cnt++;

				if( (t == '\n') || ( cnt >= AP_BUFFER_LENGTH-1 ) ){
					cnt = 0;
					GV_flag_APRX = 1;
					USART_ClearFlag(USART2, USART_FLAG_ORE | USART_FLAG_PE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_TXE);
				}
			}
		}
	}
}
