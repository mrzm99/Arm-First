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
#include <stdio.h>

static void test_dly(void)
{
    uint32_t set_val;

    port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
    while (1) {
        dly_tsk(1000);
        set_val = port_drv_get_pin_lvl(PORTB4); 
        set_val = (~set_val) & 1;
        port_drv_set_pin_lvl(PORTB4, set_val);
    }
}

static void test_task(void)
{
    T_CTSK ctsk;
    ctsk.tskatr = TA_HLANG|TA_ACT;
    ctsk.exinf = NULL;
    ctsk.task = test_dly;
    ctsk.itskpri = 0;
    ctsk.stksz = 1024;
    ctsk.stk = NULL;
    cre_tsk(0, &ctsk);

    while (1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    T_CTSK ctsk;
    ctsk.tskatr = TA_HLANG|TA_ACT;
    ctsk.exinf = NULL;
    ctsk.task = test_task;
    ctsk.itskpri = 4;
    ctsk.stksz = 1024;
    ctsk.stk = NULL;

    if (cre_tsk(4, &ctsk) < 0) {
        while (1);
    }

    while (1); 
}
