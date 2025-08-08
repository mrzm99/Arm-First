/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   app_main.c
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "error.h"
#include "port_drv.h"
#include "utility.h"
#include "task_manage.h"
#include "task_sync.h"
#include "cyc_hdlr.h"
#include <stdio.h>

static void test_cyc(void)
{
    uint32_t set_val;

    set_val = port_drv_get_pin_lvl(PORTB4); 
    set_val = (~set_val) & 1;
    port_drv_set_pin_lvl(PORTB4, set_val);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    T_CCYC ccyc;
    ccyc.cycatr = TA_HLANG|TA_PHS;
    ccyc.exinf = NULL;
    ccyc.cychdr = test_cyc;
    ccyc.cyctime = 1000;
    ccyc.cycphs = 3000;
    port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);

    if (cre_cyc(0, &ccyc) < 0) {
        while (1);
    }
    if (sta_cyc(0) < 0) {
        while (1);
    }
    while (1); 
}
