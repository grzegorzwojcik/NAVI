/*
 * SPI.c
 *
 *  Created on: Jun 16, 2015
 *      Author: Grzegorz WÓJCIK
 */

#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "SPI.h"


void SPI_initRCC(void){
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE );
}

void SPI_initGPIO(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SD_MISO | SD_MOSI | SD_SCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SD_GPIO1, &GPIO_InitStructure);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SD_NSS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SD_GPIO1, &GPIO_InitStructure);

	// Card detect & write protect floating
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SD_CD | SD_WP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SD_GPIO2, &GPIO_InitStructure);
}

void SPI_initSPI(void){
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
