/*
 * AUTOPILOT.h
 *
 *  Created on: Jul 8, 2015
 *      Author: Grzegorz WÓJCIK
 */

#ifndef AUTOPILOT_H_
#define AUTOPILOT_H_

#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "functions.h"

/*** PREDEFINED SETTINGS	***/
#define AP_PORT 			GPIOA
#define AP_USART2_TX		GPIO_Pin_2
#define AP_USART2_RX		GPIO_Pin_3
#define AP_BUFFER_LENGTH	30
#define AP_DF_CHAR			'#'		// Data frame starting character

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
volatile unsigned char GV_bufferAP[AP_BUFFER_LENGTH];
volatile unsigned char GV_flag_APRX;
volatile unsigned char GV_flag_APTX;

/* Private functions ---------------------------------------------------------*/
void AP_initRCC(void);
void AP_initGPIO(void);
void AP_initUART(void);
void AP_ClearBuffer(void);
void USART_puts(USART_TypeDef* USARTx, volatile char *s);
FlagStatus AP_checkCRC(char StartChar, uint8_t Length);

#endif /* AUTOPILOT_H_ */
