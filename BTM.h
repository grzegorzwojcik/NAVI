/*
 * BTM.h
 *
 *  Created on: Jun 7, 2015
 *      Author: Grzegorz WÓJCIK
 */

#ifndef BTM_H_
#define BTM_H_

#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "functions.h"

/*** PREDEFINED SETTINGS	***/
#define BTM_PORT 			GPIOA
#define BTM_USART1_TX		GPIO_Pin_9
#define BTM_USART1_RX		GPIO_Pin_10
#define BTM_BUFFER_LENGTH	30
#define BTM_DF_CHAR			'#'		// Data frame starting character

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
volatile unsigned char GV_bufferBTM[BTM_BUFFER_LENGTH];
volatile unsigned char GV_flag_BTMRX;
volatile unsigned char GV_flag_BTMTX;

/* Private functions ---------------------------------------------------------*/
void BTM_initRCC(void);
void BTM_initGPIO(void);
void BTM_initUART(void);
void BTM_ClearBuffer(void);
FlagStatus BTM_checkCRC(char StartChar, uint8_t Length);

#endif /* BTM_H_ */
