/*
 * FAULTS.h
 *
 *  Created on: Jun 6, 2015
 *      Author: Grzegorz
 */

#ifndef FAULTS_H_
#define FAULTS_H_

/*** PREDEFINED SETTINGS	***/
#define SERVO_GPIO_PORT 		GPIOB
#define ESC_GPIO_PORT 			GPIOA
#define SERVO_PIN				GPIO_Pin_10
#define ESC_PIN					GPIO_Pin_8


void FAULTS_initRCC(void);
void FAULTS_Servo_initGPIO(void);
void FAULTS_Servo_initTIM(void);
void FAULTS_ESC_initGPIO(void);

/* faults injection  */
void FAULTS_injectSERVO(void);
void FAULTS_injectESC(void);

/* faults removal */
void FAULTS_removalSERVO(void);
void FAULTS_removalESC(void);

#endif /* FAULTS_H_ */
