/*
 * SPI.c
 *
 *  Created on: Jun 16, 2015
 *      Author: Grzegorz WÓJCIK
 */

#include "SPI.h"




void SPI_initRCC(void){
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE );
}

void SPI_initGPIO(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SD_MISO | SD_MOSI | SD_SCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SD_GPIO1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SD_NSS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SD_GPIO1, &GPIO_InitStructure);

	// Card detect & write protect floating
	GPIO_InitStructure.GPIO_Pin = SD_CD | SD_WP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SD_GPIO2, &GPIO_InitStructure);
}

void SPI_initSPI(void){

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
}

/*
* @brief Function Name  : SPI_initSD
* @brief Description    : This functions initializes SD card with proper CMD commands.
* @param data           : None
*/
void SPI_initSD(void){
	power_on();
	GV_SDfileCreated 	= RESET;

    if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == RESET )
    	GV_SDdetected = SET;		// Card present
    else
    	GV_SDdetected = RESET;		// Card absent
}

/*
* @brief Function Name  : SD_initInterrupt_CardDetect
* @brief Description    : This functions initializes SD card detect interrupt handler. When card is detected,
* 							an event is set by software to create and update data logs.
* @param data           : None
*/
void SD_initInterrupt_CardDetect(void){

	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the GPIOC Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* Configure the GPIO pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


	/* Connect Button EXTI Line to Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);

	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*
* @brief Function Name  : SD_createLog
* @brief Description    : This functions is creating log on the SD card. Main directory is set by variable mainFolder,
* 							and the sub-directories are related to the current Date set in Navi_Struct, i.e.
* 							"mainFolder/NAVI_Struct.DateYYYY/NAVI_Struct.DateMM-NAVI_Struct.DateDD/Flightx"
* 							where x is new data log file.
* @param data           : None
*/
void SD_createLog(void){
	FRESULT fresult;
	FIL plik;
	UINT zapisanych_bajtow;
	FATFS g_sFatFs;
	//DIR directory;
	FILINFO fileInfo;

	static char mainFolder[7] = "AI-METH";	//Main folder name [SD card]
	static char final_path[40] = {0};
	static DWORD offset = 33;				//Offset used during writing data do SD card

	if(GV_SDfileCreated != SET){
		static char path[40] = {0};
		f_mount(0, &g_sFatFs);				//Mount drive 0 [SD card]
		sprintf(path, "%s", mainFolder);

		/* Checking directory & creating file */
		fresult = f_stat(path, &fileInfo);
		switch (fresult) {
			case FR_NO_FILE:				// Create main folder if it is not present

				f_mkdir(mainFolder);

			case FR_OK:						// Proceed further if it is

				/*	Create path for first subfolder (Name = YYYY)	*/
				sprintf(path, "%s/%i", mainFolder, NAVI_Struct.DateYYYY);
				fresult = f_stat(path, &fileInfo);	//Check whether it already exists
				if( fresult == FR_NO_FILE )
					f_mkdir(path);

			/*		Create path for second subfolder (Name = MM-DD)				*/
			/*		We want format MM-DD not M-D, i.e. 06-26, instead of 6-26	*/
				if(NAVI_Struct.DateMM < 10){
					if( NAVI_Struct.DateDD < 10 )
						sprintf(path, "%s/0%i-0%i", path, NAVI_Struct.DateMM, NAVI_Struct.DateDD);
					else
						sprintf(path, "%s/0%i-%i", path, NAVI_Struct.DateMM, NAVI_Struct.DateDD);
				}

			/*		We want format MM-DD not M-D, i.e. 12-09 instead of 12-9	*/
				else{
					if( NAVI_Struct.DateDD < 10 )
						sprintf(path, "%s/%i-0%i", path, NAVI_Struct.DateMM, NAVI_Struct.DateDD);
					else
						sprintf(path, "%s/%i-%i", path, NAVI_Struct.DateMM, NAVI_Struct.DateDD);
				}

				fresult = f_stat(path, &fileInfo);	//Check whether it already exists
				if( fresult == FR_NO_FILE )
					fresult = f_mkdir(path);

				/* CREATING .TXT FILE and CHECKING WHETHER PREVIOUS EXIST */
				static uint8_t FileCounter = 0;
				for( FileCounter = 0; FileCounter < 100; FileCounter++){
					char path_tmp[40] = {0};
					sprintf(path_tmp, "%s/FLIGHT%i.txt", path ,FileCounter);

					if (f_stat(path_tmp, &fileInfo) == FR_NO_FILE){
						fresult = f_open(&plik,path_tmp, FA_CREATE_ALWAYS | FA_WRITE);	//Create file and allow writing to it

						if( fresult == FR_OK )
							GV_SDfileCreated = SET;				// File succesfully created.

						char initMessage[] = "HH:MM:SS Fe Fm Fc Y  P  R  Gz  V";		//First line in .txt file
						fresult = f_open(&plik, path_tmp, FA_WRITE);
						fresult = f_write(&plik, initMessage , 35, &zapisanych_bajtow);
						fresult = f_close (&plik);
						offset = sizeof(initMessage);

						sprintf(final_path, "%s", path_tmp); 	// Save final directory to variable
						break;
					}
				}
				break;
			default:
				break;
		}
	}

	else{
		if(GV_TimeStart == SET){		//Save data to the SD CARD
			if(GV_SDdetected == SET){
				/* Create data log in proper format */
				char log_message[60] = {0};
				sprintf(log_message, "\r\n%i:%i:%i %i %i %i %i %i %i %i %i",
						NAVI_Struct.TimeHH, NAVI_Struct.TimeMM, NAVI_Struct.TimeSS,
						NAVI_Struct.FaultE, NAVI_Struct.FaultM, NAVI_Struct.FaultC,
						NAVI_Struct.Yaw, NAVI_Struct.Pitch, NAVI_Struct.Roll,
						NAVI_Struct.Gyro_Z, NAVI_Struct.Voltage);

				/* Measure log length & save it with proper offset to previous data */
				fresult = f_open(&plik, final_path, FA_WRITE);
				fresult = f_lseek(&plik, offset);
				fresult = f_write(&plik, log_message, sizeof(log_message), &zapisanych_bajtow);
				offset += sizeof(log_message);
				fresult = f_close (&plik);
			}
		}
	}
}


/*
* @brief Function Name  : EXTI9_5_IRQHandler
* @brief Description    : SD card detect interrupt handler (PC5/EXTI5)
* @param data           : None
*/
void EXTI9_5_IRQHandler(void){

	  if(EXTI_GetITStatus(EXTI_Line5) != RESET)		//SD card detect interrupt handler (PC5/EXTI5)
	  {
	    if( GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == RESET ){
	    	GV_SDdetected = SET;
	    }
	    else
	    	GV_SDdetected = RESET;
	    /* Clear the EXTI line 0 pending bit */
	  }
	  EXTI_ClearITPendingBit(EXTI_Line5);
}
