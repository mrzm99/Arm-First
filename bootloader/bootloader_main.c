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

extern uint32_t __etext;    // end of text section
extern uint32_t __sdata;    // top of data section
extern uint32_t __edata;    // end of data section
extern uint32_t __sbss;     // top of bss section
extern uint32_t __ebss;     // end of bss section
extern void main(void);     // application main

/*--------------------------------------------------------------------------------------*/
/*! @brief  Initialize Section
 */
static void section_init(void)
{
    char *p_top;
    char *p_end;
    char *p_val;

    // clear bss section to 0
    p_top = (char *)__sbss;
    p_end = (char *)__ebss;
    while (p_top < p_end) {
        *p_top = 0;
        p_top++;
    }

    // initialize data section
    p_top = (char *)__sdata;
    p_end = (char *)__edata;
    p_val = (char *)__etext;
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
    uint32_t val;

    val = (2<<28)|(2<<6);
    set_word(RCC, RCC_PLLI2SCFGR, val);
    // PLLout=160MHz, PLLsource=HSI
    val = (4<<24)|(0<<22)|(0<<16)|(40<<6)|(2<<0);
    set_word(RCC, RCC_PLLCFGR, val);
    // PLLSAI:on|PLLI2S:on|PLL:on|Clock security:on|HSE bypassed|HSE:on|HSI:on
    val = (1<<26)|(1<<24)|(1<<19)|(1<<18)|(1<<16)|(1<<0);
    set_word(RCC, RCC_CR, val);
    while (1){
        if ((get_word(RCC, RCC_CR)&(1<<25)) == 0) {
            break;
        }
    }

    // System clock = PLL
    val = (3<<27)|(3<<24)|(3<<13)|(4<<10)|(2<<0);
    set_word(RCC, RCC_CFGR, val);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  Initialize system
 */
void system_init(void)
{   
    // init section
    section_init();

    // data/instruction cach reset
    set_word(FLASH_INTERFACE, FLASH_ACR, 0x00001800);
    // cancell data/instruction cach reset
    set_word(FLASH_INTERFACE, FLASH_ACR, 0x00000000);
    // data/instruction cach enable / set access latency = 5 for SYSTEM CLOCK(160MHz)
    set_word(FLASH_INTERFACE, FLASH_ACR, 0x00000705);

    // init clock
    clock_init();

    // debug
    while (1) {
        busy_wait(40 * 1000 * 1000);
    }

    // enable interrupt

    // jump to application main
    main();
}