/*
 * w5500_spi.h
 *
 *  Created on: Sep 14, 2022
 *      Author: wsrra
 */

#ifndef SRC_W5500_W5500_CHIP_H_
#define SRC_W5500_W5500_CHIP_H_
#include "main.h"
#include "w5500.h"
#include "Config.h"
//
//#define W5500_USE_SPI_DMA		// uncomment to enable SPI DMA, or comment to enable only SPI

#define SOCK_RX_BUFF_SIZE 		(unsigned char[]) {2, 2, 2, 2, 2, 2, 2, 2}	// RX buffer size for the 8-sockets
#define SOCK_TX_BUFF_SIZE 		(unsigned char[]) {2, 2, 2, 2, 2, 2, 2, 2}	// TX buffer size for the 8-sockets

#define W5500_IRG_PRIORITY		0

#define W5500_RETRY_COUNT		CONF_DEF_W5500_RETRY_COUNT 		//2// actual count W5500_RETRY_COUNT+1
#define W5500_RETRY_BASE_TIME	CONF_DEF_W5500_RETRY_BASE_TIME //200// in ms

/*
 * Socket common interrupt
 * */

typedef union W5500_CmnIntr{
	struct intrC{
		uint16_t rsvd_0 : 1;
		uint16_t rsvd_1 : 1;
		uint16_t rsvd_2 : 1;
		uint16_t rsvd_3 : 1;
		uint16_t magicPack 	 : 1;
		uint16_t pppConnCls  : 1;
		uint16_t destUnreach : 1;
		uint16_t ipConflict  : 1;
		uint16_t sock_0 : 1;
		uint16_t sock_1 : 1;
		uint16_t sock_2 : 1;
		uint16_t sock_3 : 1;
		uint16_t sock_4 : 1;
		uint16_t sock_5 : 1;
		uint16_t sock_6 : 1;
		uint16_t sock_7 : 1;
	}ir;
	uint16_t val;
}W5500_CmnIntr;

/*
 * Socket N interrupt
 * */
typedef union W5500_SockIntr{
	struct intrS{
		uint8_t conn 	: 1;
		uint8_t disconn : 1;
		uint8_t recv 	: 1;
		uint8_t timeOut : 1;
		uint8_t sendOk 	: 1;
		uint8_t rsvd_5 	: 1;
		uint8_t rsvd_6 	: 1;
		uint8_t rsvd_7 	: 1;

	}ir;
	uint8_t val;
}W5500_SockIntr;


/*
 *
 * structure for SPI pin configuration
 *
 * */

typedef struct W5500_SPI{
	GPIO_TypeDef *cs_port;				// chip selector, GPIO port of SPI bus
	uint16_t cs_pin;					// chip selector, GPIO pin of SPI bus
	GPIO_TypeDef *rst_port;				// reset, GPIO port of SPI bus
	uint16_t rst_pin;					// reset, GPIO pin of SPI bus
	GPIO_TypeDef *int_port;				// interrupt, GPIO port of SPI bus
	uint16_t int_pin;					// interrupt, GPIO pin of SPI bus
	IRQn_Type irqType;
//	SPI_HandleTypeDef *hspi;// todo: need to uncomment
	uint32_t errorCode;
	uint32_t dmaTxErrCode;
	uint32_t dmaRxErrCode;
}W5500_SPI;


typedef struct W5500_Phy{
	uint8_t intrptFlag;
	W5500_SPI spi;
	W5500_CmnIntr intr;
	wiz_NetInfo netInfo;

}W5500_Phy;

void W5500_Init(W5500_Phy *w5500);
void W5500_InitSPIIO(W5500_SPI *spi);
int8_t W5500_SetNetInfo(wiz_NetInfo *netInfo);
int8_t W5500_GetNetInfo(wiz_NetInfo *netInfo);
uint8_t SPI_ReadWrite(uint8_t data);
uint16_t W5500_GetCmnInterpt();
uint8_t W5500_GetSockInterpt(uint8_t socket);
void W5500_SetAutoKeepAlive(uint8_t sockId, uint16_t time);
//uint16_t W5500_GetAutoKeepAlive(uint8_t sockId);
void W5500_SetMaxSegmentSize(uint8_t sockId, uint16_t size);
//uint16_t W5500_GetMaxSegmentSize(uint8_t sockId);
void W5500_SetRetryTime(uint16_t time_ms);
uint16_t W5500_GetRetryTime(void);
void W5500_SetRetryCount(uint8_t count);
uint8_t W5500_GetRetryCount(void);

uint8_t W5500_GetSR();

void W5500_ReadInterrupt(W5500_Phy *w5500);
void W5500_ClearAllInterrupt(void);

void W5500_deselect();
void W5500_select();
void W5500_PrintNetInfo(void);

void W5500_writeReadBurstDma(uint8_t *wbuff, uint8_t *rbuff, uint16_t len);

#endif /* SRC_W5500_W5500_CHIP_H_ */
