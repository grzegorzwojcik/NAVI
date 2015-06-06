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
volatile uint8_t GV_SystemStatus;	// 0 = BAD, 1 = OK

/* Private functions ---------------------------------------------------------*/
void SYSTEM_ClockCheck(void);

#endif /* FUNCTIONS_H_ */


