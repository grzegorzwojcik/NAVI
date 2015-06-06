/*
 * FAULTS.h
 *
 *  Created on: Jun 6, 2015
 *      Author: Grzegorz
 */

#ifndef FAULTS_H_
#define FAULTS_H_

/*** PREDEFINED SETTINGS	***/
#define SERVO_RCC_APB2Periph	RCC_APB2Periph_GPIOB
#define SERVO_GPIO_PORT 		GPIOB
#define SERVO_PIN				GPIO_Pin_10

#define SERVO_RCC_APB1Periph	RCC_APB1Periph_TIM2
#define SERVO_TIMER				TIM2			/*	Timer with PWM	*/


void FAULTS_Servo_initGPIO();
void FAULTS_Servo_initTIM();

#endif /* FAULTS_H_ */
