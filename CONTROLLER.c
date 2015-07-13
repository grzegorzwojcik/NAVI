/*
 * CONTROLLER.c
 *
 *  Created on: Jun 8, 2015
 *      Author: Grzegorz WÓJCIK
 */

#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "CONTROLLER.h"
#include "AUTOPILOT.h"
#include "BTM.h"
#include "functions.h"


void CTRL_initNaviStruct(void){
	NAVI_Struct.NAVIGATOR_CH1 = 50;		//Elevator-Pitch
	NAVI_Struct.NAVIGATOR_CH2 = 50;		//Aileron-Roll
	NAVI_Struct.NAVIGATOR_CH3 = 0;		//Throttle-Altitude
	NAVI_Struct.NAVIGATOR_CH4 = 50;		//Rudder-Yaw

	NAVI_Struct.RC_CH1 		= 0;		//Elevator-Pitch
	NAVI_Struct.RC_CH2 		= 0;		//Aileron-Roll
	NAVI_Struct.RC_CH3 		= 0;		//Throttle-Altitude
	NAVI_Struct.RC_CH4 		= 0;		//Rudder-Yaw
	NAVI_Struct.RC_CH5 		= 0;		//Switches output PWM signals between RC(1-4) and NAVI(1-4)
	NAVI_Struct.RC_CH6 		= 0;
	NAVI_Struct.Freq 		= 0;

	NAVI_Struct.FaultE 		= 0;
	NAVI_Struct.FaultM 		= 0;
	NAVI_Struct.FaultC 		= 0;
	NAVI_Struct.FaultTime 	= 0;

	NAVI_Struct.DateYYYY= 2015;
	NAVI_Struct.DateMM	= 6;
	NAVI_Struct.DateDD	= 26;

	NAVI_Struct.TimeHH	= 12;
	NAVI_Struct.TimeMM	= 00;
	NAVI_Struct.TimeSS	= 05;

	NAVI_Struct.Voltage		= 0;
	NAVI_Struct.IR_distance	= 0;

	NAVI_Struct.Yaw 	= 0;
	NAVI_Struct.Pitch	= 0;
	NAVI_Struct.Roll	= 0;
	NAVI_Struct.Gyro_Z	= 0;
}

void CTRL_initNAVI_RCC(void){
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
}

void CTRL_initNAVI_GPIO(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = NAVI_CH1 | NAVI_CH2 | NAVI_CH3 | NAVI_CH4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NAVI_GPIO, &GPIO_InitStructure);
}

void CTRL_initNAVI_PWM(void){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;		// 72MHz / 720 = 100kHz
	TIM_TimeBaseStructure.TIM_Period = 2000-1;			// 100kHz / 2000 = 50Hz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

	/* CHANNEL 1 (NAVI_CH4 - PB6)*/
	TIM_OCInitStructure.TIM_Pulse = 65;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* CHANNEL 2 (NAVI_CH3 - PB7)*/
	TIM_OCInitStructure.TIM_Pulse = 65;
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* CHANNEL 3 (NAVI_CH1 - PB8)*/
	TIM_OCInitStructure.TIM_Pulse = 65;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* CHANNEL 4 (NAVI_CH2 - PB9)*/
	TIM_OCInitStructure.TIM_Pulse = 65;
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

/* This functions MUST be executed after FAULTS_Servo_initTIM() @Faults.h */
/* 50 Hz control loop */
void CTRL_initTIM(void){

	/* Enable the TIM2 Update interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit( TIM2, TIM_IT_Update );
	TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );
}


/*
* @brief Function Name  	: CTRL_DataProcess
* @brief Description    	: This function processes the GV_bufferBTM data frame and completes
* 								structure elements that are related to it.
* @return 					: None
*
* @INFO						: this function uses global variable GV_bufferBTM[]
*/
void CTRL_DataProcess(CTRLsource_t CTRL_source_device){
	/* Sample frames:
	 	#,1,44,33,22,11,*CRC,0x0A0x0D
	 	#,1,50,50,66,33,*49,
		#,1,50,50,69,32,*63,
		#,6,15:25:06,16:43:15,0,0,*55
		#,2,0,0,0,5000,*7
	 */
	unsigned char tmp_buffer[30] = {0};

	switch (CTRL_source_device) {
	uint8_t cnt = 0;
		case CTRL_source_Autopilot:
			for(cnt = 0; cnt <= AP_BUFFER_LENGTH; cnt++)
				tmp_buffer[cnt] = GV_bufferAP[cnt];
			break;

		case CTRL_source_MobileDevice:
			for(cnt = 0; cnt <= BTM_BUFFER_LENGTH; cnt++)
				tmp_buffer[cnt] = GV_bufferBTM[cnt];
			break;
	}


	static uint8_t CommaCounter;
	static uint8_t i;
	static uint8_t j;	//FRAME index counter
	static uint8_t k;	//DATA1 index counter
	static uint8_t l;	//DATA2 index counter
	static uint8_t m;	//DATA3 index counter
	static uint8_t n;	//DATA4 index counter

	static char StartProcessingFlag = 0;	// Flag, set to 1 when '=' is detected
	char FRAME[2]			= {0};	// Temporary char array
	char DATA1[5]			= {0};	// Temporary char array
	char DATA2[5]			= {0};	// Temporary char array
	char DATA3[5]			= {0};	// Temporary char array
	char DATA4[5]			= {0};	// Temporary char array


	for( i = 0, j = 0, k = 0, l = 0, m = 0, n = 0, CommaCounter = 0,
			StartProcessingFlag = 0; i < BTM_BUFFER_LENGTH ; i++ ){
		if( StartProcessingFlag == 1 ){

			if( tmp_buffer[i] == '\r')
				break;

			if( tmp_buffer[i] == ',')		//increment counter when ',' is detected
				CommaCounter++;

			/* Parse FRAME byte */
			if( (CommaCounter == 1) && (tmp_buffer[i] != ',') ){
				FRAME[j] = tmp_buffer[i];
				j++;
			}
			/* Parse DATA1 bytes */
			else if( CommaCounter == 2 && (tmp_buffer[i] != ',')  ){
				DATA1[k] = tmp_buffer[i];
				k++;
			}
			/* Parse DATA2 bytes */
			else if( CommaCounter == 3 && (tmp_buffer[i] != ',') ){
				DATA2[l] = tmp_buffer[i];
				l++;
			}
			/* Parse DATA3 bytes */
			else if( CommaCounter == 4 && (tmp_buffer[i] != ',') ){
				DATA3[m] = tmp_buffer[i];
				m++;
			}
			/* Parse DATA4 bytes */
			else if( CommaCounter == 5 && (tmp_buffer[i] != ',') ){
				DATA4[n] = tmp_buffer[i];
				n++;
			}
		}

		if( tmp_buffer[i] == '#')	//set flag to 1 when first ',' is detected
			StartProcessingFlag = 1;
	}

	static uint8_t tmp = 0;
	tmp = atoi(FRAME);
	switch (tmp){
		case 1:				// Data frame is related to the QUADRO CONTROL
			NAVI_Struct.NAVIGATOR_CH1 = atoi(DATA1);
			NAVI_Struct.NAVIGATOR_CH2 = 100 - atoi(DATA2);
			NAVI_Struct.NAVIGATOR_CH3 = atoi(DATA3);
			NAVI_Struct.NAVIGATOR_CH4 = 100 - atoi(DATA4);
			break;
		case 2:				// Data frame is related to the FAULT INJECTION AND REMOVAL
			NAVI_Struct.FaultE 		= atoi(DATA1);
			NAVI_Struct.FaultM 		= atoi(DATA2);
			NAVI_Struct.FaultC 		= atoi(DATA3);
			NAVI_Struct.FaultTime 	= atoi(DATA4);
			if(NAVI_Struct.FaultTime > 0){
				GV_TimeCounter = NAVI_Struct.FaultTime/1000 + 30;
				GV_TimeStart = 1;
			}
			break;
		case 3:				// Data frame is related to the SENSOR BOARD Euler angles & Z angular velocity measurements
			NAVI_Struct.Yaw 	= atoi(DATA1);
			NAVI_Struct.Pitch 	= atoi(DATA2);
			NAVI_Struct.Roll 	= atoi(DATA3);
			NAVI_Struct.Gyro_Z 	= atoi(DATA4);
			break;
		case 4:				// Data frame is related to the SENSOR BOARD ADC voltage & sonar measurements
			NAVI_Struct.Voltage 	= atoi(DATA1);
			NAVI_Struct.IR_distance = atoi(DATA2);
			break;
		case 6:			// Data frame is related to the current DATA sent, via Bluetooth, by external device
			NAVI_Struct.DateYYYY	= atoi(DATA1);
			NAVI_Struct.DateMM 		= atoi(DATA2);
			NAVI_Struct.DateDD 		= atoi(DATA3);
			break;
		case 7:			// Data frame is related to the current TIME sent, via Bluetooth, by external device
			NAVI_Struct.TimeHH = atoi(DATA1);
			NAVI_Struct.TimeMM = atoi(DATA2);
			NAVI_Struct.TimeSS = atoi(DATA3);
			break;
		default:
			break;
	}
}


/*
* @brief Function Name  	: CTRL_controlAUTOPILOT check
* @brief Description    	: This function generates PWMs that are input to the Autopilot (Flight controller) board
* @param Prc_Min			: Minimum accepted % to be converted into PWM
* @param Prc_Max			: Maximum accepted % to be converted into PWM
* @param PWM_Min			: Minimal PWM output
* @param PWM_Max			: Maximum PWM output
* @return 					: None
*
* @INFO						: this function uses global structure NAVI_Struct
*/
void CTRL_controlAUTOPILOT(uint8_t Prc_Min, uint8_t Prc_Max, uint16_t PWM_Min, uint16_t PWM_Max){
	static uint8_t tmp = 0;
	static uint8_t CH1 = 0;
	static uint8_t CH2 = 0;
	static uint8_t CH3 = 0;
	static uint8_t CH4 = 0;
	tmp = PWM_Max - PWM_Min;

	CH1 = (NAVI_Struct.NAVIGATOR_CH1 * tmp / 100) + PWM_Min;	// NAVI_CH1	(PITCH)
	if( CH1 > PWM_Max )
		CH1 = PWM_Max;
	if( CH1 < PWM_Min)
		CH1 = PWM_Min;

	CH2 = (NAVI_Struct.NAVIGATOR_CH2 * tmp / 100) + PWM_Min;	// NAVI_CH2 (ROLL)
	if( CH2 > PWM_Max )
		CH2 = PWM_Max;
	if( CH2 < PWM_Min)
		CH2 = PWM_Min;

	CH3 = (NAVI_Struct.NAVIGATOR_CH3 * tmp / 100) + PWM_Min;	// NAVI_CH3 (THROTTLE)
	if( CH3 > PWM_Max )
		CH3 = PWM_Max;
	if( CH3 < PWM_Min)
		CH3 = PWM_Min;

	CH4 = (NAVI_Struct.NAVIGATOR_CH4 * tmp / 100) + PWM_Min;	// NAVI_CH4 (YAW)
	if( CH4 > PWM_Max )
		CH4 = PWM_Max;
	if( CH4 < PWM_Min)
		CH4 = PWM_Min;

	TIM4->CCR3 = CH1;		// NAVI_CH1	(PITCH)
	TIM4->CCR4 = CH2;		// NAVI_CH2 (ROLL)
	TIM4->CCR2 = CH3;		// NAVI_CH3 (THROTTLE)
	TIM4->CCR1 = CH4;		// NAVI_CH4 (YAW)
}


