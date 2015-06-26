/*
 * CONTROLLER.h
 *
 *  Created on: Jun 8, 2015
 *      Author: Grzegorz WÓJCIK
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

/*** PREDEFINED SETTINGS	***/
/* Typedefs ------------------------------------------------------------------*/
#define NAVI_CH1	GPIO_Pin_8		//Elevator	-Pitch 		(TIM4_CH3)
#define NAVI_CH2	GPIO_Pin_9		//Aileron	-Roll		(TIM4_CH4)
#define NAVI_CH3	GPIO_Pin_7		//Throttle	-Altitude	(TIM4_CH2)
#define NAVI_CH4	GPIO_Pin_6		//Rudder	-Yaw		(TIM4_CH1)
#define NAVI_GPIO	GPIOB


typedef struct{
		uint16_t RC_CH1;		//Elevator	-Pitch
		uint16_t RC_CH2;		//Aileron	-Roll
		uint16_t RC_CH3;		//Throttle	-Altitude
		uint16_t RC_CH4;		//Rudder	-Yaw
		uint8_t  RC_CH5;		//Mode
		uint8_t  RC_CH6;		//Mode
		uint16_t Freq;			//PWM frequency

		uint8_t NAVIGATOR_CH1;	//Elevator	-Pitch
		uint8_t NAVIGATOR_CH2;	//Aileron	-Roll
		uint8_t NAVIGATOR_CH3;	//Throttle	-Altitude
		uint8_t NAVIGATOR_CH4;	//Rudder	-Yaw

		uint8_t FaultE;			//Electronic fault
		uint8_t FaultM;			//Mechanical fault
		uint8_t FaultC;			//Communication fault
		uint16_t FaultTime;		//Time at which fault should be injected

		char* DateYYYY;		//Year	e.g. 2015
		char* DateMM;			//Month	e.g. 06
		char* DateDD;			//Day	e.g. 08

		uint8_t TimeHH;			//Hour e.g. 12
		uint8_t TimeMM;			//Minutes e.g. 00
		uint8_t TimeSS;			//Seconds e.g. 00
}NAVI;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
volatile NAVI NAVI_Struct;
/* Private functions ---------------------------------------------------------*/
void CTRL_initNaviStruct(void);
void CTRL_initNAVI_RCC(void);
void CTRL_initNAVI_GPIO(void);
void CTRL_initNAVI_PWM(void);
void CTRL_initTIM(void);			// This functions MUST be executed after FAULTS_Servo_initTIM() @Faults.h
void CTRL_DataProcess(void);		// Processing received data frames and completing the NAVI_Struct variables
void CTRL_controlAUTOPILOT(uint8_t Prc_Min, uint8_t Prc_Max, uint16_t PWM_Min, uint16_t PWM_Max);
	// CTRL_controlAUTOPILOT -			Generating control signals (50Hz PWMs) for the Autopilot (Flight controller)
										// These signals are based on data received from mobile devices

#endif /* CONTROLLER_H_ */
