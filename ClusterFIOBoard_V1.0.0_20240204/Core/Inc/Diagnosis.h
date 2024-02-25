/*
 * Diagnosis.h
 *
 *  Created on: Aug 9, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef INC_DIAGNOSIS_H_
#define INC_DIAGNOSIS_H_
#include "main.h"

typedef struct Diag_UART{
	uint16_t peCount;    	//             /*!< Parity error        */
	uint16_t neCount;     	//            /*!< Noise error         	*/
	uint16_t feCount;    		//             /*!< Frame error         */
	uint16_t oreCount;    	//            /*!< Overrun error       */
	uint16_t dteCount;    		//            /*!< DMA transfer error  */
	uint16_t dmaTxErrCode;
	uint16_t dmaRxErrCode;
	uint16_t ErrCode;
}Diag_UART;

typedef struct Diag_SPI{
	uint16_t modfeCount;   	//           /*!< MODF error - Master mode fault        */
	uint16_t crceCount;     		//          /*!< CRC error         	*/
	uint16_t oreCount;    	//            /*!< Overrun error         */
	uint16_t feCount;    		//            /*!< Frame error       */
	uint16_t dteCount;    		//            /*!< DMA transfer error  */
	uint16_t flgeCount;    		//            /*!< Error on RXNE/TXE/BSY Flag   */
	uint16_t abreCount;    		//            /*!< Error during SPI Abort procedure    */
	uint16_t dmaTxErrCode;
	uint16_t dmaRxErrCode;
	uint16_t ErrCode;
}Diag_SPI;



typedef struct Diag_I2C{
	uint16_t ErrCode;
	uint16_t dmaTxErrCode;
	uint16_t dmaRxErrCode;
//	uint16_t busErrCount;             	/*!< BERR: Bus  error            */
//	uint16_t arloErrCount;     			/*!< ARLO: Arbitration lost (master mode) error            */
//	uint16_t afErrCount;    			/*!< AF: Acknowledge failure error              */
//	uint16_t ovrErrCount;    			/*!< OVR: Overrun/Underrun error             */
//	uint16_t dmaTxErrCount;    			/*!< DMA transfer error    */
//	uint16_t timoutErrCount;    		/*!< Timeout Error         */
//	uint16_t sizeErrCount;    			/*!< Size Management error */
//	uint16_t dmaParamErrCount;			/*!< DMA Parameter Error   */
//	uint16_t wrongStartErrCount;		/*!< Wrong start Error     */
}Diag_I2C;




typedef struct Diagnosis{
	Diag_UART rtuBus;
	Diag_SPI w5500Spi;
	Diag_I2C i2c;
}Diagnosis;

//
//void Diag_ProcessRTUBusError(Diag_UART *rtuBusErr, uint32_t );
//void Diag_ProcessW5500SPIError(Diag_SPI *spiErr);
#endif /* INC_DIAGNOSIS_H_ */
