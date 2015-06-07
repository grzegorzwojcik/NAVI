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

void FAULTS_initRCC(void){
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	// Servomechanism
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);							// Servomechanism
}

void FAULTS_Servo_initGPIO(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SERVO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SERVO_GPIO_PORT, &GPIO_InitStructure);

	//GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
}

void FAULTS_Servo_initTIM(void){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;
	TIM_TimeBaseStructure.TIM_Period = 2000-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse = 60;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void FAULTS_injectSERVO(void){
	TIM2->CCR3 = 250;
}

void FAULTS_removalSERVO(void){
	TIM2->CCR3 = 60;
}

