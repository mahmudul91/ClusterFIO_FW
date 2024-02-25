/*
 * w5500_spi.c
 *
 *  Created on: Sep 14, 2022
 *      Author: wsrra
 */
#include <w5500_chip.h>
#include "main.h"
//#include "stm32f1xx_hal.h"
#include "wizchip_conf.h"
#include "stdio.h"
#include "Debug.h"
#include "string.h"
#include "socket.h"
#include "W5500/w5500.h"
#include "w5500_custom.h"
//extern SPI_HandleTypeDef hspi1;// todo: need to uncomment
W5500_SPI *spiComm;

extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

extern uint8_t spiTxCplt;
extern uint8_t spiRxCplt;

void W5500_readBurstDma(uint8_t *buff, uint16_t len);
void W5500_writeBurstDma(uint8_t *buff, uint16_t len);


//rest chip
void W5500_reset(){

	// reset the w5500
	HAL_GPIO_WritePin(spiComm->rst_port, spiComm->rst_pin, GPIO_PIN_RESET);
	uint8_t count = 0xFF;
	while(count--);
	HAL_GPIO_WritePin(spiComm->rst_port, spiComm->rst_pin, GPIO_PIN_SET);// GPIO_PIN_RESET);
}

//selects the chip

void W5500_select(){
	HAL_GPIO_WritePin(spiComm->cs_port, spiComm->cs_pin, GPIO_PIN_RESET);
}

// de-selects the chip
void W5500_deselect(){
	HAL_GPIO_WritePin(spiComm->cs_port, spiComm->cs_pin, GPIO_PIN_SET);
}

#ifndef W5500_USE_SPI_DMA

// reads single byte
uint8_t W5500_read(void){
//	SPI1_Init();
	uint8_t rxData;
//	HAL_SPI_Receive(&hspi1, &rxData, 1, 10U);// todo: need to uncomment
	return rxData;
	//return SPI_ReadWrite(0x00);
//	return w_read();
}

// writes single byte
void W5500_write(uint8_t byte){
//	SPI1_Init();
	//SPI_ReadWrite(byte);
//	HAL_SPI_Transmit(&hspi1, &byte , 1, 10U);// todo: need to uncomment/

//	w_write(byte);
}



// reads rx buffer
void W5500_readBurst(uint8_t *buff, uint16_t len){

//	HAL_SPI_Receive(&hspi1, buff, len, len);// todo: need to uncomment/// todo: need to uncomment

//	for (uint16_t i = 0; i < len; i++) {
////		*buff = SPI_ReadWrite(0x00);
//		*buff = W5500_read();
//		buff++;
//	}
}

// write tx buffer
void W5500_writeBurst(uint8_t *buff, uint16_t len){

//	HAL_SPI_Transmit(&hspi1,  buff, len, len);// todo: need to uncomment

	//	for (uint16_t i = 0; i < len; i++) {
////		SPI_ReadWrite(*buff);
//		W5500_write(*buff);
//		buff++;
//	}
}

#ifdef USE_W5500_CUST_READ_WRITE_FUNC

// write tx buffer
void W5500_writeReadBurst(uint8_t *wbuff, uint8_t *rbuff, uint16_t len){
//	while(spiTxCplt>0) {;}
//	spiTxCplt=1;
//	DEBUG_SPRINT_NL("txd");
//		__HAL_DMA_DISABLE(&hdma_spi1_rx);
//		__HAL_DMA_DISABLE(&hdma_spi1_tx);
//	DEBUG_SPRINT("\r\nstateTX1: spi:%d TX:%d RX:%d",HAL_SPI_GetState(&hspi1),HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx) );
//	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
//		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY){
//		DEBUG_SPRINT_NL("10-SPI: %d, DRX: %d, DTX: %d", HAL_SPI_GetState(&hspi1), HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx));
//	}

//	DEBUG_SPRINT("\r\ntx_2");
//		HAL_SPI_MspInit(&hspi1);
//		__HAL_DMA_ENABLE(&hdma_spi1_rx);
//		__HAL_DMA_ENABLE(&hdma_spi1_tx);
//	DEBUG_SPRINT("\r\nspiCplt: TX:%d RX:%d",spiTxCplt, spiRxCplt);
//	if(spiTxCplt>0) return;

	HAL_SPI_TransmitReceive(&hspi1, wbuff, rbuff, len, len);


//		spiTxCplt =1;
//	}
//	DEBUG_SPRINT("\r\nstateTX2: spi:%d TX:%d RX:%d",HAL_SPI_GetState(&hspi1),HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx) );
//	DEBUG_SPRINT("\r\ntx_3");
//	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("11-DRX: %d", HAL_DMA_GetState(hspi1.hdmarx));
//	}
////	tick = HAL_GetTick();
//	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("12-DTX: %d", HAL_DMA_GetState(hspi1.hdmatx));
////		if(HAL_GetTick()-tick >= TIMOUT) break;
//	}

//	for (uint16_t i = 0; i < len; i++) {
////		SPI_ReadWrite(*buff);
//		HAL_SPI_Transmit_DMA(&hspi1, &buff[i], 1);
////		buff++;
//	}
//	DEBUG_SPRINT("\r\ntx_4");
	return;
}
#endif

#else

volatile uint32_t tick  = 0;
#define TIMOUT	100


// reads single byte
uint8_t W5500_readByteDma(void){
	uint8_t readByte=0;
//
//	while(spiRxCplt>0) {;}
//	spiRxCplt=1;
//	uint8_t writeByte=0xFF;
//	DEBUG_SPRINT_NL("rxb");

	tick = HAL_GetTick();
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY){
//		DEBUG_SPRINT_NL("1-SPI: %d, DRX: %d, DTX: %d", HAL_SPI_GetState(&hspi1), HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}


//	HAL_SPI_MspInit(&hspi1);
//	__HAL_DMA_ENABLE(&hdma_spi1_rx);
//	__HAL_DMA_ENABLE(&hdma_spi1_tx);
	HAL_SPI_Receive_DMA(&hspi1, &readByte, 1);
//	__HAL_DMA
	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("2-DRX: %d", HAL_DMA_GetState(hspi1.hdmarx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}
	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("3-DTX: %d", HAL_DMA_GetState(hspi1.hdmatx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}

	return readByte;
}

// writes single byte
void W5500_writeByteDma(uint8_t byte){
//	while(spiTxCplt>0) {;}
//	spiTxCplt=1;
//	uint8_t readByte=0;
//	DEBUG_SPRINT_NL("txb");
	tick = HAL_GetTick();
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY){
//		DEBUG_SPRINT_NL("4-SPI: %d, DRX: %d, DTX: %d", HAL_SPI_GetState(&hspi1), HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}
//	__HAL_DMA_DISABLE(&hdma_spi1_rx);
//	__HAL_DMA_DISABLE(&hdma_spi1_tx);

//	HAL_SPI_MspInit(&hspi1);
//	__HAL_DMA_ENABLE(&hdma_spi1_rx);
//	__HAL_DMA_ENABLE(&hdma_spi1_tx);

	HAL_SPI_Transmit_DMA(&hspi1, &byte, 1);
	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("5-DRX: %d", HAL_DMA_GetState(hspi1.hdmarx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}
	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("6-DTX: %d", HAL_DMA_GetState(hspi1.hdmatx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}

	return;
}



// reads rx buffer
void W5500_readBurstDma(uint8_t *buff, uint16_t len){
//	while(spiRxCplt>0) {;}
//	spiRxCplt=1;
//	DEBUG_SPRINT_NL("rxd");
//	DEBUG_SPRINT("\r\nstateRX1: spi:%d TX:%d RX:%d",HAL_SPI_GetState(&hspi1),HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx) );
	tick = HAL_GetTick();
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY){
//		DEBUG_SPRINT_NL("7-SPI: %d, DRX: %d, DTX: %d", HAL_SPI_GetState(&hspi1), HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}
//	__HAL_DMA_DISABLE(&hdma_spi1_rx);
//	__HAL_DMA_DISABLE(&hdma_spi1_tx);
//	DEBUG_SPRINT("\r\nrx_2");
//	HAL_SPI_MspInit(&hspi1);
//	__HAL_DMA_ENABLE(&hdma_spi1_rx);
//	__HAL_DMA_ENABLE(&hdma_spi1_tx);
//	DEBUG_SPRINT("\r\nspiCplt: TX:%d RX:%d",spiTxCplt, spiRxCplt);

//	if(spiRxCplt>0) return;

	HAL_SPI_Receive_DMA(&hspi1, buff, len);

//	DEBUG_SPRINT("\r\nstateRX2: spi:%d TX:%d RX:%d",HAL_SPI_GetState(&hspi1),HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx) );

//	DEBUG_SPRINT("\r\nrx_3");
	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("8-DRX: %d", HAL_DMA_GetState(hspi1.hdmarx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}
	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("9-DTX: %d", HAL_DMA_GetState(hspi1.hdmatx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}
//


//	DEBUG_SPRINT("\r\nrx_4");
//	for (uint16_t i = 0; i < len; i++) {
////		*buff = SPI_ReadWrite(0x00);
//		HAL_SPI_Receive_DMA(&hspi1, &buff[i], 1);
////		buff++;
//	}
	return;
}

// write tx buffer
void W5500_writeBurstDma(uint8_t *buff, uint16_t len){
//	while(spiTxCplt>0) {;}
//	spiTxCplt=1;
//	DEBUG_SPRINT_NL("txd");
//		__HAL_DMA_DISABLE(&hdma_spi1_rx);
//		__HAL_DMA_DISABLE(&hdma_spi1_tx);
//	DEBUG_SPRINT("\r\nstateTX1: spi:%d TX:%d RX:%d",HAL_SPI_GetState(&hspi1),HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx) );
	tick = HAL_GetTick();
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY){
//		DEBUG_SPRINT_NL("10-SPI: %d, DRX: %d, DTX: %d", HAL_SPI_GetState(&hspi1), HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}

//	DEBUG_SPRINT("\r\ntx_2");
//		HAL_SPI_MspInit(&hspi1);
//		__HAL_DMA_ENABLE(&hdma_spi1_rx);
//		__HAL_DMA_ENABLE(&hdma_spi1_tx);
//	DEBUG_SPRINT("\r\nspiCplt: TX:%d RX:%d",spiTxCplt, spiRxCplt);
//	if(spiTxCplt>0) return;


	HAL_SPI_Transmit_DMA(&hspi1, buff, len);

//		spiTxCplt =1;
//	}
//	DEBUG_SPRINT("\r\nstateTX2: spi:%d TX:%d RX:%d",HAL_SPI_GetState(&hspi1),HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx) );
//	DEBUG_SPRINT("\r\ntx_3");
	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("11-DRX: %d", HAL_DMA_GetState(hspi1.hdmarx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}
	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("12-DTX: %d", HAL_DMA_GetState(hspi1.hdmatx));
		if(HAL_GetTick()-tick >= TIMOUT){
		hspi1.hdmatx->State = HAL_DMA_STATE_READY;
			break;
		}
	}

//	for (uint16_t i = 0; i < len; i++) {
////		SPI_ReadWrite(*buff);
//		HAL_SPI_Transmit_DMA(&hspi1, &buff[i], 1);
////		buff++;
//	}
//	DEBUG_SPRINT("\r\ntx_4");
	return;
}


// write tx buffer
void W5500_writeReadBurstDma(uint8_t *wbuff, uint8_t *rbuff, uint16_t len){
//	while(spiTxCplt>0) {;}
//	spiTxCplt=1;
//	DEBUG_SPRINT_NL("txd");
//		__HAL_DMA_DISABLE(&hdma_spi1_rx);
//		__HAL_DMA_DISABLE(&hdma_spi1_tx);
//	DEBUG_SPRINT("\r\nstateTX1: spi:%d TX:%d RX:%d",HAL_SPI_GetState(&hspi1),HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx) );
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY){

//		DEBUG_SPRINT_NL("10-SPI: %d, DRX: %d, DTX: %d", HAL_SPI_GetState(&hspi1), HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx));

	}

//	DEBUG_SPRINT("\r\ntx_2");
//		HAL_SPI_MspInit(&hspi1);
//		__HAL_DMA_ENABLE(&hdma_spi1_rx);
//		__HAL_DMA_ENABLE(&hdma_spi1_tx);
//	DEBUG_SPRINT("\r\nspiCplt: TX:%d RX:%d",spiTxCplt, spiRxCplt);
//	if(spiTxCplt>0) return;


	HAL_SPI_TransmitReceive_DMA(&hspi1, wbuff, rbuff, len);


//		spiTxCplt =1;
//	}
//	DEBUG_SPRINT("\r\nstateTX2: spi:%d TX:%d RX:%d",HAL_SPI_GetState(&hspi1),HAL_DMA_GetState(hspi1.hdmarx), HAL_DMA_GetState(hspi1.hdmatx) );
//	DEBUG_SPRINT("\r\ntx_3");
	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("11-DRX: %d", HAL_DMA_GetState(hspi1.hdmarx));
	}
//	tick = HAL_GetTick();
	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET){
//		DEBUG_SPRINT_NL("12-DTX: %d", HAL_DMA_GetState(hspi1.hdmatx));
//		if(HAL_GetTick()-tick >= TIMOUT) break;
	}

//	for (uint16_t i = 0; i < len; i++) {
////		SPI_ReadWrite(*buff);
//		HAL_SPI_Transmit_DMA(&hspi1, &buff[i], 1);
////		buff++;
//	}
//	DEBUG_SPRINT("\r\ntx_4");
	return;
}

#endif

//
//
///* SPI */
//uint8_t spiReadByte(void)
//{
//	uint8_t readByte=0;
//	uint8_t writeByte=0xFF;
//
//	while(HAL_SPI_GetState(&hspi1)!=HAL_SPI_STATE_READY);
//	HAL_SPI_TransmitReceive(&hspi1, &writeByte, &readByte, 1, 10);
//
//	return readByte;
//}
//
//void spiWriteByte(uint8_t writeByte)
//{
//	uint8_t readByte=0;
//
//
//	while(HAL_SPI_GetState(&hspi1)!=HAL_SPI_STATE_READY);
//	HAL_SPI_TransmitReceive(&hspi1, &writeByte, &readByte, 1, 10);
//}
///* SPI DMA */
//uint8_t spiDmaReadByte(void)
//{
//
//	uint8_t readByte=0;
//	uint8_t writeByte=0xFF;
//
//	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
//		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY);
//
//	HAL_SPI_Receive_DMA(&hspi1, &readByte, 1);
//
//	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET);
//	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET);
//
//	return readByte;
//
//}
//
//void spiDmaWriteByte(uint8_t writeByte)
//{
//	uint8_t readByte=0;
//
//	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
//		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY);
//
//	HAL_SPI_Transmit_DMA(&hspi1, &writeByte, 1);
//
//	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET);
//	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET);
//
//	return;
//}
//
//
//void spiDmaReadBurst(uint8_t* pBuf, uint16_t len)
//{
//
//	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
//		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY);
//
//	HAL_SPI_Receive_DMA(&hspi1, pBuf, len);
//
//	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET);
//	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET);
//
//	return;
//}
//
//void spiDmaWriteBurst(uint8_t* pBuf, uint16_t len)
//{
//
//	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY &&
//		  HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY && HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY);
//
//	HAL_SPI_Transmit_DMA(&hspi1, pBuf, len);
//
//	while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET);
//	while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET);
//
//	return;
//}

// initialize gpio associate with spi comm control
void W5500_InitSPIIO(W5500_SPI *spi){


	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(spi->cs_port, spi->cs_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(spi->rst_port, spi->rst_pin, GPIO_PIN_SET);

	/*Configure GPIO pins : SPI1_RST_Pin SPI1_CS_Pin */
	GPIO_InitStruct.Pin = spi->cs_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(spi->cs_port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = spi->rst_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(spi->rst_port, &GPIO_InitStruct);
	/*Configure GPIO pin : PHY_SPI1_INT_Pin */
	GPIO_InitStruct.Pin = spi->int_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(spi->int_port, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(spi->irqType, 0, W5500_IRG_PRIORITY);
	HAL_NVIC_EnableIRQ(spi->irqType);
}



void W5500_Init(W5500_Phy *w5500){

	// row-1: rx buffer size, row-2: tx buffer size
	uint8_t buffSize[2][8];
	memcpy(buffSize[0], SOCK_RX_BUFF_SIZE, (sizeof(SOCK_RX_BUFF_SIZE)/sizeof(SOCK_RX_BUFF_SIZE[0])));
	memcpy(buffSize[1], SOCK_TX_BUFF_SIZE, (sizeof(SOCK_TX_BUFF_SIZE)/sizeof(SOCK_TX_BUFF_SIZE[0])));

	spiComm = &w5500->spi;
//	hspi = w5500->spi.hspi;
	// init the spi comm controll gpio
	W5500_InitSPIIO(&w5500->spi);

//	__HAL_SPI_ENABLE(hspi);
//	W5500_SetRetryCount(W5500_RETRY_COUNT);

	// CS pin high by default
	HAL_GPIO_WritePin(w5500->spi.cs_port, w5500->spi.cs_pin, GPIO_PIN_SET);
	W5500_reset();
	HAL_Delay(1);

	// register callback function
	reg_wizchip_cs_cbfunc(W5500_select, W5500_deselect);				// chip select/de-selct callback
#ifndef W5500_USE_SPI_DMA
#ifdef USE_W5500_CUST_READ_WRITE_FUNC
	reg_wizchip_spiburst_rw_cbfunc(W5500_writeReadBurst);
	#endif
//#else
	reg_wizchip_spi_cbfunc(W5500_read, W5500_write);					// read/write single byte callback
	reg_wizchip_spiburst_cbfunc(W5500_readBurst, W5500_writeBurst);		// read/write buffer callback
//#endif

#else
	reg_wizchip_spi_cbfunc(W5500_readByteDma, W5500_writeByteDma);
	reg_wizchip_spiburst_cbfunc(W5500_readBurstDma, W5500_writeBurstDma);		// read/write buffer callback
#endif


	//	wizchip_init(memSize[0],memSize[1]);
	// wizchip initialize

	if(ctlwizchip(CW_INIT_WIZCHIP, (void*)buffSize) == -1){
//		HAL_UART_Transmit(&huart1, (uint8_t *)"Wizchip init failed...\r\n", 24, 24*2);
		printf("\r\nWizchip init failed...");
	}else{
//		HAL_UART_Transmit(&huart1, (uint8_t *)"Wizchip init success...\r\n", 25, 25*2);
		printf("\r\nWizchip init success...");
	}

	/*Sets the retry counter and base delay time*/
	W5500_SetRetryCount(W5500_RETRY_COUNT);
	W5500_SetRetryTime(W5500_RETRY_BASE_TIME);

	// enable all kind of interrupt
	wizchip_setinterruptmask((IK_IP_CONFLICT | IK_DEST_UNREACH | IK_PPPOE_TERMINATED |  IK_SOCK_ALL));		// enable all common interrupt

//	ctlsocket(0,CS_SET_INTMASK , (void*)SIK_ALL);
//	ctlsocket(0,CS_SET_INTMASK , (void*)SIK_);
//	ctlsocket(0,CS_SET_INTMASK , (void*)SIK_RECEIVED);
//	ctlsocket(1,CS_SET_INTMASK , (void*)SIK_ALL );
//	ctlsocket(2,CS_SET_INTMASK , (void*)SIK_ALL );
//	ctlsocket(3,CS_SET_INTMASK , (void*)SIK_ALL );
//	ctlsocket(4,CS_SET_INTMASK , (void*)SIK_ALL );
//	ctlsocket(5,CS_SET_INTMASK , (void*)SIK_ALL );
//	ctlsocket(6,CS_SET_INTMASK , (void*)SIK_ALL );
//	ctlsocket(7,CS_SET_INTMASK , (void*)SIK_ALL );	// enable all socket  interrupt

}


int8_t W5500_SetNetInfo(wiz_NetInfo *netInfo){
	return ctlnetwork(CN_SET_NETINFO, (void*)netInfo);
}

int8_t W5500_GetNetInfo(wiz_NetInfo *netInfo){
	return ctlnetwork(CN_GET_NETINFO, (void*)netInfo);
}




/*
 * gets IR and SIR interrupts
 * */
uint16_t W5500_GetCmnInterpt(){
	return wizchip_getinterrupt();
}



/*
 * gets SnIR interrupts
 * */
uint8_t W5500_GetSockInterpt(uint8_t socket){
	return getSn_IR(socket);
}


/*
 * Set auto Keep alive timer
 * */

void W5500_SetAutoKeepAlive(uint8_t sockId, uint16_t time){// time > 0
    setSn_KPALVTR(sockId, time);
}

/*
 * Set max segment size for a socket
 * */
void W5500_SetMaxSegmentSize(uint8_t sockId, uint16_t size){
	setSn_MSSR(sockId, size);
}


//void W5500_SetAutoKeepAlive(uint8_t sockId, uint16_t time);
//uint16_t W5500_GetAutoKeepAlive(uint8_t sockId);
//void W5500_SetMaxSegmentSize(uint8_t sockId, uint16_t size);
//uint16_t W5500_GetMaxSegmentSize(uint8_t sockId);
void W5500_SetRetryTime(uint16_t time_ms){
	if(time_ms > 0){
		setRTR((time_ms*10));
	}
}
uint16_t W5500_GetRetryTime(void){
	return (getRTR()/10);
}
void W5500_SetRetryCount(uint8_t count){
	setRCR(count);
}
uint8_t W5500_GetRetryCount(void){
	return getRCR();
}

//
//uint8_t W5500_GetSR(){
//
//	getVERSIONR();
//
//	 uint32_t AddrSel = Sn_SR(0);//SIPR;//VERSIONR;
//	   uint8_t ret;
//#define  len  4
//	   uint8_t spi_data[len];
//	   uint8_t rxdata[len];
//
////	   WIZCHIP_CRITICAL_ENTER();
//	   WIZCHIP.CS._select();
//
//	   AddrSel |= (_W5500_SPI_READ_ | 0);
////getSIPR();
////	   if(!WIZCHIP.IF.SPI._read_burst || !WIZCHIP.IF.SPI._write_burst) 	// byte operation
////	   {
////
////		   WIZCHIP.IF.SPI._write_byte((AddrSel & 0x00FF0000) >> 16);
////			WIZCHIP.IF.SPI._write_byte((AddrSel & 0x0000FF00) >>  8);
////			WIZCHIP.IF.SPI._write_byte((AddrSel & 0x000000FF) >>  0);
////
////	   }
////	   else																// burst operation
////	   {
//
//	   rxdata[0] = 0;
//	   rxdata[1] = 0;
//	   rxdata[2] = 0;
//	   rxdata[3] = 0;
//
//			spi_data[0] = (AddrSel & 0x00FF0000) >> 16;
//			spi_data[1] = (AddrSel & 0x0000FF00) >> 8;
//			spi_data[2] = (AddrSel & 0x000000FF) >> 0;
//			spi_data[3] = 0;
////			DEBUG_SPRINT("\r\nSPI_txV: %X, %X, %X, %X",spi_data[0],spi_data[1],spi_data[2],spi_data[3] );
////			HAL_SPI_Receive_DMA(&hspi1, spi_data, 3);
//			HAL_SPI_TransmitReceive(&hspi1, spi_data,rxdata, len,100);
////			for(uint16_t i = 0; i< 1000; i++){
////				__NOP();
////			}
//////			HAL_SPI_Receive_DMA(&hspi1, rxdata, 1);
////			for(uint16_t i = 0; i< 1000; i++){
////				__NOP();
////			}
//
//			while (HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmarx) == HAL_DMA_STATE_RESET){
////				DEBUG_SPRINT_NL("11-DRX: %d", HAL_DMA_GetState(hspi1.hdmarx));
//			}
////			tick = HAL_GetTick();
//			while (HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_BUSY|| HAL_DMA_GetState(hspi1.hdmatx) == HAL_DMA_STATE_RESET){
////				DEBUG_SPRINT_NL("12-DTX: %d", HAL_DMA_GetState(hspi1.hdmatx));
////				if(HAL_GetTick()-tick >= TIMOUT) break;
//			}
//
////			DEBUG_SPRINT("\r\nSPI_txV: %X, %X, %X, %X",spi_data[0],spi_data[1],spi_data[2],spi_data[3] );
////			DEBUG_SPRINT("\r\nSPI_rxV:");
////			for(uint16_t i = 0; i<len; i++)
////			DEBUG_SPRINT(" %d", rxdata[i]);
////			WIZCHIP.IF.SPI._write_burst(spi_data, 3);
////
////	   }
////
////	   ret = WIZCHIP.IF.SPI._read_byte();
//
//	   WIZCHIP.CS._deselect();
////	   WIZCHIP_CRITICAL_EXIT();
//	   return rxdata[3];
//}
//

void W5500_ReadInterrupt(W5500_Phy *w5500){

	w5500->intr.val |= (uint16_t)wizchip_getinterrupt();				// read the socket common interrupt

//	sprintf(dbug.str,"\r\nCom: 0x%x",server->intr.val);
//	DEBUG_PRINT(dbug.str);


}


void W5500_ClearAllInterrupt(void){
	wizchip_clrinterrupt((IK_IP_CONFLICT | IK_DEST_UNREACH | IK_PPPOE_TERMINATED |  IK_SOCK_ALL));
}

void W5500_PrintNetInfo(void){
	wiz_NetInfo gWIZNETINFO;
	wizchip_getnetinfo(&gWIZNETINFO);
	printf("\r\nDevice Net Info{\r\n");
	printf("\tMac : %02x:%02x:%02x:%02x:%02x:%02x\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("\tIP  : %d.%d.%d.%d\r\n",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("\tSN  : %d.%d.%d.%d\r\n",gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("\tGW  : %d.%d.%d.%d\r\n",gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("\tDNS : %d.%d.%d.%d\r\n",gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	printf("}\r\n");
}
