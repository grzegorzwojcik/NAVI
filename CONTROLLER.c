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
#include "BTM.h"


void CTRL_initNaviStruct(void){
	NAVI_Struct.NAVI_CH1 = 0;
	NAVI_Struct.NAVI_CH2 = 0;
	NAVI_Struct.NAVI_CH3 = 0;
	NAVI_Struct.NAVI_CH4 = 0;

	NAVI_Struct.RC_CH1 = 0;
	NAVI_Struct.RC_CH2 = 0;
	NAVI_Struct.RC_CH3 = 0;
	NAVI_Struct.RC_CH4 = 0;
	NAVI_Struct.RC_CH5 = 0;
	NAVI_Struct.RC_CH6 = 0;
	NAVI_Struct.Freq = 0;

	NAVI_Struct.FaultE = 0;
	NAVI_Struct.FaultM = 0;
	NAVI_Struct.FaultC = 0;
	NAVI_Struct.FaultTime = 0;
}



void CTRL_DataProcess(void){
	/* Sample frame:
	 	 %,1,44,33,22,11,*CRC,0x0A0x0D
	 */
	static uint8_t CommaCounter;
	static uint8_t i;
	static uint8_t j;	//FRAME index counter
	static uint8_t k;	//DATA1 index counter
	static uint8_t l;	//DATA2 index counter
	static uint8_t m;	//DATA3 index counter
	static uint8_t n;	//DATA4 index counter

	static char StartProcessingFlag = 0;	// Flag, set to 1 when '=' is detected
	char FRAME[2]			= {0};	// Temporary char array
	char DATA1[3]			= {0};	// Temporary char array
	char DATA2[3]			= {0};	// Temporary char array
	char DATA3[3]			= {0};	// Temporary char array
	char DATA4[3]			= {0};	// Temporary char array


	for( i = 0, j = 0, k = 0, l = 0, m = 0, n = 0, CommaCounter = 0, StartProcessingFlag = 0; i < BTM_BUFFOR_LENGTH ; i++ ){
		if( StartProcessingFlag == 1 ){

			if( GV_bufforBTM[i] == '\r')
				break;

			if(GV_bufforBTM[i] == ',')		//increment counter when ',' is detected
				CommaCounter++;

			/* Parse FRAME byte */
			if( (CommaCounter == 1) && (GV_bufforBTM[i] != ',') ){
				FRAME[j] = GV_bufforBTM[i];
				j++;
			}
			/* Parse DATA1 bytes */
			else if( CommaCounter == 2 && (GV_bufforBTM[i] != ',')  ){
				DATA1[k] = GV_bufforBTM[i];
				k++;
			}
			/* Parse DATA2 bytes */
			else if( CommaCounter == 3 && (GV_bufforBTM[i] != ',') ){
				DATA2[l] = GV_bufforBTM[i];
				l++;
			}
			/* Parse DATA3 bytes */
			else if( CommaCounter == 4 && (GV_bufforBTM[i] != ',') ){
				DATA3[m] = GV_bufforBTM[i];
				m++;
			}
			/* Parse DATA4 bytes */
			else if( CommaCounter == 5 && (GV_bufforBTM[i] != ',') ){
				DATA4[n] = GV_bufforBTM[i];
				n++;
			}
		}

		if( GV_bufforBTM[i] == '%')	//set flag to 1 when first ',' is detected
			StartProcessingFlag = 1;
	}

	static uint8_t tmp = 0;
	tmp = atoi(FRAME);
	switch (tmp){
		case 1:
			NAVI_Struct.NAVI_CH1 = atoi(DATA1);
			NAVI_Struct.NAVI_CH2 = atoi(DATA2);
			NAVI_Struct.NAVI_CH3 = atoi(DATA3);
			NAVI_Struct.NAVI_CH4 = atoi(DATA4);
			break;
		case 2:
			NAVI_Struct.FaultE = atoi(DATA1);
			NAVI_Struct.FaultM = atoi(DATA2);
			NAVI_Struct.FaultC = atoi(DATA3);
			NAVI_Struct.FaultTime = atoi(DATA4);
		default:
			break;
	}
}
