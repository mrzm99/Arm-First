/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   bootloader_main.c
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include <stdint.h>
#include "iodefine.h"
#include "utility.h"
#include "int_hdlr.h"
#include "kernel.h"

extern void *__etext;    // end of text section
extern void *__sdata;    // top of data section
extern void *__edata;    // end of data section
extern void *__sbss;     // top of bss section
extern void *__ebss;     // end of bss section

/*--------------------------------------------------------------------------------------*/
/*! @brief  Initialize Section
 */
static void section_init(void)
{
    char *p_top;
    char *p_end;
    char *p_val;

    // clear bss section to 0
    p_top = (char *)&__sbss;
    p_end = (char *)&__ebss; 
    while (p_top < p_end) {
        *p_top = 0;
        p_top++;
    }

    // initialize data section
    p_top = (char *)&__sdata;
    p_end = (char *)&__edata;
    p_val = (char *)&__etext;
    while (p_top < p_end) {
        *p_top = *p_val;
        p_top++;
        p_val++;
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  Initialize Clock
 */
static void clock_init(void)
{
    // PLLin(HSI)=16MHz, PLLN=40, PLLM=2, PLLP=2, PLLQ=8
    // f(vco)= PLLin * (PLLN/PLLM) = 320MHz
    // f(pll out)= f(vco) / PLLP = 160MHz
    // f(usb otg, fs, sdio, rng)= f(vco) / PLLQ = 40MHz
    set_word(RCC + RCC_PLLCFGR, 0x08000282);
    // PLLSAI:on|PLLI2S:on|PLL:on|Clock security:off|HSE bypassed|HSE:on|HSI:on
    set_word(RCC + RCC_CR, 0x05010001);

    // System clock = PLL = 160MHz
    // AHB = system clock / 1 = 160MHz
    // APB1 = AHB / 4 = 40MHz
    // APB2 = AHB / 2 = 80MHz
    // RTC = HSE / 16 = 1MHz
    set_word(RCC + RCC_CFGR, 0xA4109402);

    // USB OTG / Ether / DMA2 / DMA1 / CCM and RAM / SRAM/ CRC enable
    // IO port I,H,G,F,E,D,C,B,A enable
    set_word(RCC + RCC_AHB1ENR, 0x7E7411FF);
    // Flexible static memory controller module clock enable
    set_word(RCC + RCC_AHB3ENR, 0x00000001);
    // I2C1,2,3,4 / UART2,3,4,5 / SPI2,3 / TIM14,13,12,7,6,5,4,3,2
    set_word(RCC + RCC_APB1ENR, 0x00FEC1FF);
    // TIM11,10,9 / System configuration controller / SPI1 / SDIO / USART6,1 / TIM8,1
    set_word(RCC + RCC_APB2ENR, 0x00075833);
    // RTC clock enable
    set_word(RCC + RCC_BDCR, 0x00008000);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  Initialize system
 */
void system_init(void)
{
    // init section
    section_init();

    // flash data/instruction cach reset
    set_word(FLASH_INTERFACE + FLASH_ACR, 0x00001800);
    // flash cancell data/instruction cach reset
    set_word(FLASH_INTERFACE + FLASH_ACR, 0x00000000);
    // flash data/instruction cach enable / set access latency = 5 for SYSTEM CLOCK(160MHz)
    set_word(FLASH_INTERFACE + FLASH_ACR, 0x00000705);

    // init clock
    clock_init();

    // enable module
    uint32_t set_val = get_word(RCC + RCC_APB1ENR);
    set_field(uint32_t, set_val, 1<<17, 1);
    set_word(RCC + RCC_APB1ENR, set_val);

    // jump to kernel 
    kernel_init();
}
