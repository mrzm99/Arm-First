/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   systick.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  SysTick timer 
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "iodefine.h"
#include "utility.h"
#include "error.h"
#include "systick.h"
#include "port_drv.h"   
#include "int_hdlr.h"
#include "system_define.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief  systick handler
 */
static void systick_handler(void)
{
    static int cnt = 0; // debug
    static int lvl = 0;

    port_drv_set_pin_func(PORTA2, PORTA2_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
    port_drv_set_pin_func(PORTA3, PORTA3_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
    if (++cnt >= 1000) {
        cnt = 0;
        if (lvl) {
            port_drv_set_pin_lvl(PORTB4, PORT_LVL_HIGH);
        } else {
            port_drv_set_pin_lvl(PORTB4, PORT_LVL_LOW);
        }
        lvl = ~lvl;
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief systick init 
 */
void systick_init(void)
{
    // SysTick disable
    set_word(STK_CTRL + 0, 0x00000000);
    // clear current cnt val
    set_word(STK_VAL + 0, 0x00000000);
}


/*--------------------------------------------------------------------------------------*/
/*! @brief  systick start 
 */
int32_t systick_start(uint32_t clk_src, uint32_t clk_MHz)
{
    uint32_t cnt_val;
    uint32_t set_val;

    // parameter check
    if (clk_src >= SYSTICK_CLK_MAX) {
        return ERR_PAR;
    }

    // set count value for 1ms cycle
    cnt_val = ((clk_MHz * 1000 * 1000) / (1000)) - 1;
    set_word(STK_LOAD, cnt_val);

    // set SysTick exception handler
    set_handler(INTHDLR_NO_SYSTICK, systick_handler);

    // set SysTick exception priority
    set_val = get_word(SHPR3);
    set_field(uint32_t, set_val, 0xFF000000, INT_PRI_SYSTICK);
    set_word(SHPR3, set_val);

    // set SysTick clock source 
    if (clk_src == SYSTICK_CLK_AHB_DIV_8) {
        set_val = 0<<2;
    } else {
        set_val = 1<<2;
    } 
    set_val |= 0x03;    // enable systick and exception 
    set_word(STK_CTRL, set_val); 

    return ERR_OK;
}
