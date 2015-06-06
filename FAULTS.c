/*
 * FAULTS.c
 *
 *  Created on: Jun 6, 2015
 *      Author: Grzegorz
 */


#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "FAULTS.h"

void FAULTS_Servo_initGPIO(){



	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

	GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
}

void FAULTS_Servo_initTIM(){

	/* W³asne struktury */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 		TIM_OCInitStructure;

	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;				/* 72 000 000 Hz divided by 720 equals 100 000 Hz */
	TIM_TimeBaseStructure.TIM_Period = 2000-1;					/* Need 50Hz -> 100 000 Hz /x = 50 Hz  -> period x = 2000 */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = ;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* SETTING UP 3rd CHANNEL OF TIM2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1999;							/* Po ilu zliczonych taktach ma zostac zgloszone zdarzenie? */
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);				/* Inicjalizacja kana³u OCx  */
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);	/* W³¹czenie buforowania dla OC3 */

	TIM_ARRPreloadConfig(TIM2, ENABLE);					/* W³¹czenie buforowania dla okresu licznika */
	TIM_Cmd(TIM2, ENABLE);								/* W³¹czenie licznika */
	//TIM_ITConfig(SERVO_TIMER, ENABLE);						/* W³¹czenie przerwania */

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
