/*
 * SPI.h
 *
 *  Created on: Jun 16, 2015
 *      Author: Grzegorz WÓJCIK
 */
#include <stddef.h>
#include <stdlib.h>
#include "stm32f10x.h"

#include "integer.h"
#include "ff.h"


#ifndef SPI_H_
#define SPI_H_

/* Predefined settings for SD card */
#define SD_NSS		GPIO_Pin_4
#define SD_SCK		GPIO_Pin_5
#define SD_MISO		GPIO_Pin_6
#define SD_MOSI		GPIO_Pin_7
#define	SD_GPIO1	GPIOA

#define SD_WP		GPIO_Pin_4
#define SD_CD		GPIO_Pin_5
#define	SD_GPIO2	GPIOC

void SPI_initRCC(void);
void SPI_initGPIO(void);
void SPI_initSPI(void);
void SPI_initSD(void);


void SELECT(void);
void DESELECT(void);
void xmit_spi(BYTE Data);
BYTE rcvr_spi(void);
BYTE wait_ready(void);


#endif /* SPI_H_ */
