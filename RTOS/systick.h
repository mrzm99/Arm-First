/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   systick.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  SysTick timer 
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  SysTick clock source 
 */
#define     SYSTICK_CLK_AHB_DIV_8       (0)     // AHB/8
#define     SYSTICK_CLK_AHB             (1)     // AHB
#define     SYSTICK_CLK_MAX             (2)  

/*--------------------------------------------------------------------------------------*/
/*! @brief  SysTick func 
 */
void systick_init(void);
int32_t systick_start(uint32_t clk_src, uint32_t clk_MHz);

#endif
