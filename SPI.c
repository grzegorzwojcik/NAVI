/*
 * SPI.c
 *
 *  Created on: Jun 16, 2015
 *      Author: Grzegorz WÓJCIK
 */

#include "SPI.h"
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
	u8 i, cmd_arg[6];
	u32 Count = 0xFFF;

	DESELECT();	// CS = 1

	for (i = 0; i < 10; i++)
		xmit_spi(0xFF); // Wyslij 0xFF 10 razy = 80 cykli zegarowych
						// (wymaganych co najmniej 74 cykli)
	SELECT(); 	// CS = 0

	// Przygotowanie ramki inicjujacej do wyslania
	cmd_arg[0] = (0x40);//	cmd_arg[0] = (CMD0 | 0x40);
	cmd_arg[1] = 0; // Argument komendy
	cmd_arg[2] = 0; // nawet, gdy komenda go nie ma
	cmd_arg[3] = 0; // musi zostac wyslany w postaci zer
	cmd_arg[4] = 0;
	cmd_arg[5] = 0x95; // CRC = 0x95
	for (i = 0; i < 6; i++) // Wyslanie ramki
		xmit_spi(cmd_arg[i]);

	while ((rcvr_spi() != 0x01) && Count){
		// Czeka na 0x01
		Count--;
		if(rcvr_spi() == 0x01)
			GV_SystemCounter = GV_SystemCounter;
	}


	DESELECT(); // CS = 1
	xmit_spi(0xFF); // Wyslij 0xFF

	//PowerFlag = 1;
}

void SELECT(void){ // CS w stan niski
	GPIO_ResetBits(SD_GPIO1, SD_NSS);
}


void DESELECT(void){ // CS w stan wysoki
	GPIO_SetBits(SD_GPIO1, SD_NSS);
}



void xmit_spi(BYTE Data){ // Wyslanie bajtu do SD

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, Data);
}


BYTE rcvr_spi(void){ // Odebranie bajtu z SD
	u8 Data = 0;
	// Wyslanie 0xFF
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, 0xFF);

	// Odebranie bajtu
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	Data = SPI_I2S_ReceiveData(SPI1);
	return Data;
}

BYTE wait_ready(void){

	BYTE res;
	//Timer2 = 50; // Czeka przez 500ms
	rcvr_spi();

	//do
	res = rcvr_spi();
	//while ((res != 0xFF) && Timer2);
	return res;
}
