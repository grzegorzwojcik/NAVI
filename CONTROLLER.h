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
typedef struct{
		uint16_t RC_CH1;	//Elevator	-Pitch
		uint16_t RC_CH2;	//Aileron	-Roll
		uint16_t RC_CH3;	//Throttle	-Altitude
		uint16_t RC_CH4;	//Rudder	-Yaw
		uint8_t  RC_CH5;	//Mode
		uint8_t  RC_CH6;	//Mode
		uint16_t Freq;		//PWM frequency

		uint8_t NAVI_CH1;	//Elevator	-Pitch
		uint8_t NAVI_CH2;	//Aileron	-Roll
		uint8_t NAVI_CH3;	//Throttle	-Altitude
		uint8_t NAVI_CH4;	//Rudder	-Yaw
		uint8_t FaultE;		//Electronic fault
		uint8_t FaultM;		//Mechanical fault
		uint8_t FaultC;		//Communication fault
		uint16_t FaultTime;	//Time at which fault should be injected
}NAVI;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
volatile NAVI NAVI_Struct;
/* Private functions ---------------------------------------------------------*/
void CTRL_initNaviStruct(void);
void CTRL_initTIM(void);		// This functions MUST be executed after FAULTS_Servo_initTIM() @Faults.h
void CTRL_DataProcess(void);

#endif /* CONTROLLER_H_ */
