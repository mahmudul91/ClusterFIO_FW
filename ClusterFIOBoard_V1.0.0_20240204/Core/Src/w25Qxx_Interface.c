/*
 * w25Qxx_Interface.c
 *
 *  Created on: Feb 21, 2024
 *      Author: wsrra
 */


#include "w25Qxx_Interface.h"
#include "Debug.h"
#include "Delay_us.h"
/**< w25qxx handle */
static w25qxx_handle_t w25q;
extern SPI_HandleTypeDef hspi1;


uint8_t W25Q_SPIInit(void);
uint8_t W25Q_SPIDeinit(void);
uint8_t W25Q_SPIWriteRead(uint8_t instruction, uint8_t instruction_line,
        uint32_t address, uint8_t address_line, uint8_t address_len,
        uint32_t alternate, uint8_t alternate_line, uint8_t alternate_len,
        uint8_t dummy, uint8_t *in_buf, uint32_t in_len,
        uint8_t *out_buf, uint32_t out_len, uint8_t data_line);
void W25Q_Delay_ms (uint32_t ms);
void W25Q_Delay_us (uint32_t us);

/**
  * @brief  Init w25qxx
  * @param  i2c: void pointer to the i2c port struct
  * @retval none
  */
uint8_t W25Q_Init(w25qxx_type_t type, w25qxx_interface_t interface, w25qxx_bool_t dualQspiEnable){
	/*Link the interface function*/
    DRIVER_W25QXX_LINK_INIT(&w25q, w25qxx_handle_t);
    DRIVER_W25QXX_LINK_SPI_QSPI_INIT(&w25q, W25Q_SPIInit);
    DRIVER_W25QXX_LINK_SPI_QSPI_DEINIT(&w25q, W25Q_SPIDeinit);
    DRIVER_W25QXX_LINK_SPI_QSPI_WRITE_READ(&w25q, W25Q_SPIWriteRead);
    DRIVER_W25QXX_LINK_DELAY_MS(&w25q, W25Q_Delay_ms);
    DRIVER_W25QXX_LINK_DELAY_US(&w25q, W25Q_Delay_us);
	DRIVER_W25QXX_LINK_DEBUG_PRINT(&w25q, DBG_SPRINT);

	/*Set chip type*/
	if( w25qxx_set_type(&w25q, type) !=0 ){
		DBG_SPRINT("w25qxx: set type failed!\n");
		return 1;
	}

    /* Set chip interface */
    if (w25qxx_set_interface(&w25q, interface) != 0){
    	DBG_SPRINT("w25qxx: set interface failed.\n");
        return 1;
    }

    /*Set dual quad spi */
    if (w25qxx_set_dual_quad_spi(&w25q, dualQspiEnable) != 0){
    	/*Deinit chip*/
    	(void)w25qxx_deinit(&w25q);
    	DBG_SPRINT("w25qxx: set dual quad spi failed.\n");
        return 1;
    }

    /* chip init */
    if (w25qxx_init(&w25q) != 0){
    	DBG_SPRINT("w25qxx: init failed.\n");
        return 1;
    }else if (type >= W25Q256){
		if (w25qxx_set_address_mode(&w25q, W25QXX_ADDRESS_MODE_4_BYTE) != 0){
			/*Deinit chip*/
			(void)w25qxx_deinit(&w25q);
			DBG_SPRINT("w25qxx: set address mode failed.\n");
			return 1;
		}
    }
	return 0;
}


/**
 * @brief   Spi Init
 * @param	none
 * @retval  error status
 */
uint8_t W25Q_SPIInit(void){
	  hspi1.Instance = SPI1;
	  hspi1.Init.Mode = SPI_MODE_MASTER;
	  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	  hspi1.Init.NSS = SPI_NSS_SOFT;
	  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  hspi1.Init.CRCPolynomial = 0x0;
	  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
	  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
	  if( HAL_SPI_Init(&hspi1) != HAL_OK){
		  return 1;
	  }
	  return 0;
}

/**
 * @brief   Get w25q instance
 * @param	none
 * @retval  instance of the w25qxx_handle_t structure
 */
w25qxx_handle_t* W25Q_GetInstance(void){
	return &w25q;
}

/**
 * @brief   Deinit spi
 * @param	none
 * @retval  error status
 */
uint8_t W25Q_SPIDeinit(void){
    if (HAL_SPI_DeInit(&hspi1) != HAL_OK){
        return 1;
    }
    return 0;
}


/**
 * @brief   Millisecond delay
 * @param	ms: delay time in ms
 * @retval  error status
 */
void W25Q_Delay_ms (uint32_t ms){
	HAL_Delay(ms);
}


/**
 * @brief   microsecond delay
 * @param	us: delay time in us
 * @retval  error status
 */
void W25Q_Delay_us (uint32_t us){
	Delay_us(us);
}


/**
 * @brief      spi interface write read bytes
 * @param[in]  *handle points to a w25qxx handle structure
 * @param[in]  *in_buf points to an input buffer
 * @param[in]  in_len is the input length
 * @param[out] *out_buf points to an output buffer
 * @param[in]  out_len is the output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 */
uint8_t W25Q_SPIWriteRead(uint8_t instruction, uint8_t instruction_line,
        uint32_t address, uint8_t address_line, uint8_t address_len,
        uint32_t alternate, uint8_t alternate_line, uint8_t alternate_len,
        uint8_t dummy, uint8_t *in_buf, uint32_t in_len,
        uint8_t *out_buf, uint32_t out_len, uint8_t data_line){

	if(in_buf == NULL && in_len == 0 && out_buf == NULL && out_len == 0){
		return 1;
	}
	HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_RESET);
	/*SPI Write*/
	if(in_buf != NULL && in_len > 0){
		if( HAL_SPI_Transmit(&hspi1, in_buf, in_len, in_len+1) != HAL_OK){
			HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);
			return 1;
		}
	}

	/*SPI Read*/
	if(out_buf != NULL && out_len > 0){
		if(HAL_SPI_Receive(&hspi1, out_buf, out_len, out_len+1) != HAL_OK){
			HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);
			return 1;
		}
	}
	HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);
	return 0;
}



/**
 * @brief   spi bus write
 * @param 	addr is the spi register address
 * @param 	*buf points to a data buffer
 * @param	len is the length of the data buffer
 * @return  status code
 *          - 0 success
 *          - 1 write failed
 */
uint8_t W25Q_SPIWrite(uint8_t addr, uint8_t *buf, uint16_t len){
    uint8_t buffer = addr;
    /*check null buffer and len */
    if (buf == NULL && len == 0){
    	return 1;
    }
    /* set cs low */
    HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_RESET);

    /* transmit the addr */
    if (HAL_SPI_Transmit(&hspi1, (uint8_t *)&buffer, 1, 2) != HAL_OK){
        /* set cs high */
        HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);
        return 1;
    }

	/* transmit the buffer */
	if (HAL_SPI_Transmit(&hspi1, buf, len, len+1) != HAL_OK){
		/* set cs high */
		HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);
		return 1;
	}

    /* set cs high */
    HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);

    return 0;
}




/**
 * @brief	spi bus read
 * @param  	addr is the spi register address
 * @param 	*buf points to a data buffer
 * @param	len is the length of the data buffer
 * @return  status code
 *          - 0 success
 *          - 1 read failed
 */
uint8_t W25Q_SPIRead(uint8_t addr, uint8_t *buf, uint16_t len){
	uint8_t buffer;
    /*check null buffer and len */
    if (buf == NULL && len == 0){
    	return 1;
    }
    /* set cs low */
    HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_RESET);

    /* transmit the addr */
    buffer = addr;
    if (HAL_SPI_Transmit(&hspi1, (uint8_t *)&buffer, 1, 2) != HAL_OK){
        /* set cs high */
        HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);
        return 1;
    }

	/* receive to the buffer */
	if (HAL_SPI_Receive(&hspi1, buf, len, len+1) != HAL_OK){
		/* set cs high */
		HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);
		return 1;
	}
    /* set cs high */
    HAL_GPIO_WritePin(W25QXX_SPI1_CS_GPIO_Port, W25QXX_SPI1_CS_Pin, GPIO_PIN_SET);

    return 0;
}
