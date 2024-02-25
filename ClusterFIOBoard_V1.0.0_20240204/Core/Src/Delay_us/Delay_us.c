/*
 * Delay_us.c
 *
 *  Created on: Feb 22, 2024
 *      Author: wsrra
 */
#include "Delay_us.h"
#include "main.h"
#include "Debug.h"
/*clock count in 1 us*/
static volatile uint32_t clkCount = 0;

/**
 * @brief  delay clock init
 * @return status code
 *         - 0 success
 * @note   none
 */
void Delay_Init(void){
    /*Config systick clock source HCLK */
//    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    DBG_SPRINT_NL("SysClk: %u",HAL_RCC_GetSysClockFreq());
    /* Calculate clock count in 1 us*/
    clkCount = HAL_RCC_GetSysClockFreq()/1000000;
}

/**
 * @brief   delay us
 * @param	us
 * @note    none
 */
void Delay_us(uint32_t us){
    uint32_t ticks = us*clkCount;
    uint32_t told = SysTick->VAL;
    uint32_t tnow = SysTick->VAL;
    uint32_t tcnt = 0;
    uint32_t reload = SysTick->LOAD;

    /* delay */
    while (1){
        tnow = SysTick->VAL;
        if (tnow != told){
        	tcnt += (tnow < told) ?
        			(told - tnow) :
					(reload - tnow + told);
            told = tnow;
            if (tcnt >= ticks){ break; }
        }
    }
}

