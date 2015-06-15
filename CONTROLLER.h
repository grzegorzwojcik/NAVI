/*
 * CONTROLLER.h
 *
 *  Created on: Jun 8, 2015
 *      Author: Grzegorz W�JCIK
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
void CTRL_initTIM(void);		// This functions MUST be executed after FAULTS_Servo_initTIM() @Faults.h
void CTRL_DataProcess(void);

#endif /* CONTROLLER_H_ */
