/*
 * functions.h
 *
 *  Created on: Jun 6, 2015
 *      Author: Grzegorz WÓJCIK
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
volatile uint8_t GV_SystemReady;	// 0 = NOT READY, 1 = OK
volatile uint16_t GV_SystemCounter;	// System time counter, incrementing within Systick handler
volatile uint8_t GV_TimeStart;		// When set to 1, data is being saved to the SD Card
volatile uint16_t GV_TimeCounter;	// time counter, which is DECREMENTING within Systick handler
									// when it reaches 0, data is no more saved to the SD card [GV_TimeStart becomes 0]


/* Private functions ---------------------------------------------------------*/
void PLL_Configurattion(void);
void SYSTEM_ClockCheck(void);
void USART_puts(USART_TypeDef* USARTx, volatile unsigned char *s);

#endif /* FUNCTIONS_H_ */


