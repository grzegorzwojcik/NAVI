/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : Quadcopter navigation board main function.
**
**  Author		: Grzegorz WÓJCIK
**
**  Environment : Atollic TrueSTUDIO/STM32
**
*****************************************************************************
*/

/* Includes */
#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "functions.h"
#include "FAULTS.h"

/* Private typedef */
 USART_InitTypeDef USART_InitStructure;
/* Private define  */
/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */


int main(void)
{


	/* TODO - Add your application code here */
	//HSEStartUpStatus = RCC_WaitForHSEStartUp();
	//SYSTEM_ClockCheck();
	SysTick_Config(SystemCoreClock/1000);	// SysTick 1 kHZ (1ms interval)
	RCC_ClocksTypeDef ClksFreq;
	RCC_GetClocksFreq(&ClksFreq);


	/* Enter infinite loop only when clock frequencies are OK */
	/*	if( GV_SystemStatus == 1 ){
	FAULTS_Servo_initGPIO();
	FAULTS_Servo_initTIM();



	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 		TIM_OCInitStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;
	TIM_TimeBaseStructure.TIM_Period = 2000 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode - TIM_CounterMode_Up;
	//TIM_TimeBaseStructure.TIM_RepetitionCounter
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1500;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM2->CCR3 = 1500;
	TIM3->CCR2 = 1500;

		while (1)
		{
			if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) != 0 ){
				//GV_SystemCounter++;
			}

			if( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) != 0 ){
				static uint16_t chuj = 0;
				chuj ++;
				if( GV_SystemCounter >= 1000 )
					chuj = 0;
			}
		}


	}

	else
		while(1);
	*/

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

	  RCC_ClocksTypeDef RCC_Clocks;
		RCC_GetClocksFreq(&RCC_Clocks);

		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_SetBits(GPIOC, GPIO_Pin_7);

		GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseStructure.TIM_Prescaler = 720-1;
		TIM_TimeBaseStructure.TIM_Period = 2000-1;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_Pulse = 60;
		//TIM_OCInitStructure.TIM_OCIdleState
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OC2Init(TIM3, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

		TIM_ARRPreloadConfig(TIM3, ENABLE);
		TIM_Cmd(TIM3, ENABLE);
		FAULTS_Servo_initGPIO();
		FAULTS_Servo_initTIM();
		static uint16_t chuj = 60;
		static uint16_t chuj2 = 60;
		TIM2->CCR3 = chuj2;
		TIM3->CCR2 = chuj;


	while(1){
		TIM3->CCR2 = chuj;
		TIM2->CCR3 = chuj2;
	}
}



/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}
