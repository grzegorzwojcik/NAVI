/*
 * BTM.h
 *
 *  Created on: Jun 7, 2015
 *      Author: Grzegorz WÓJCIK
 */

#ifndef BTM_H_
#define BTM_H_

/*** PREDEFINED SETTINGS	***/
#define BTM_PORT 			GPIOB
#define BTM_USART1_TX		GPIO_Pin_9
#define BTM_USART1_RX		GPIO_Pin_10

void BTM_initRCC(void);
void BTM_initGPIO(void);
void BTM_initUART(void);

#endif /* BTM_H_ */
