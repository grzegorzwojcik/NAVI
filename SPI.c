/*
 * SPI.c
 *
 *  Created on: Jun 16, 2015
 *      Author: Grzegorz WÓJCIK
 */

#include "SPI.h"
#include "CONTROLLER.h"
#include "functions.h"


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
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
}

void SPI_initSD(void){
	power_on();
}

void SD_initCardDetect(void){

	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the GPIOC Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* Configure the GPIO_LED pin */
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
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



void SD_createLog(void){
	FRESULT fresult;
	FIL plik;
	WORD zapisanych_bajtow;
	FATFS g_sFatFs;
	//DIR directory;
	FILINFO fileInfo;

	static char mainFolder[7] = "AI-METH";	//Main folder name [SD card]
	static char path[40] = {0};

	f_mount(0, &g_sFatFs);		//Mount drive 0 [SD card]

	sprintf(path, "%s", mainFolder);
	fresult = f_stat(path, &fileInfo);			// Checking directory

	switch (fresult) {
		case FR_NO_FILE:						// Create if main folder is not present
			f_mkdir(mainFolder);

		case FR_OK:								// Proceed further if it is
			//Create path for first subfolder (Name = YYYY)
			sprintf(path, "%s/%s", mainFolder, NAVI_Struct.DateYYYY);
			fresult = f_stat(path, &fileInfo);	//Check whether it already exists
			if( fresult == FR_NO_FILE )
				f_mkdir(path);

			//Create path for second subfolder (Name = MM-DD)
			sprintf(path, "%s/%s/%s-%s", mainFolder, NAVI_Struct.DateYYYY, NAVI_Struct.DateMM, NAVI_Struct.DateDD);
			fresult = f_stat(path, &fileInfo);	//Check whether it already exists
			if( fresult == FR_NO_FILE )
				fresult = f_mkdir(path);

			static uint8_t FileCounter = 0;
			for( FileCounter = 0; FileCounter < 100; FileCounter++){
				sprintf(path, "%s/%s/%s-%s/Flight%i.txt",
						mainFolder, NAVI_Struct.DateYYYY, NAVI_Struct.DateMM, NAVI_Struct.DateDD, FileCounter);

				if (f_stat(path, &fileInfo) == FR_NO_FILE){
					fresult = f_open(&plik,path, FA_CREATE_ALWAYS | FA_WRITE);	//Create file and allow writing to it
					fresult = f_close (&plik);
					break;
				}
			}
			break;
		default:
			break;
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
	    	GV_SDdetected = 1;
	    	SD_createLog();
	    }
	    else
	    	GV_SDdetected = 0;
	    /* Clear the EXTI line 0 pending bit */
	    EXTI_ClearITPendingBit(EXTI_Line5);
	  }
}
