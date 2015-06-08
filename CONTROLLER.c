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
	NAVI_Struct.FaultTime = 10000;
}

