/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   app_main.c
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "error.h"
#include "port_drv.h"
#include "utility.h"
#include "systick.h"
#include "iodefine.h"
#include "que.h"
#include "system_define.h"
#include "task_manage.h"
#include <stdio.h>

static void test_task(void)
{
    while (1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    /*
    uint32_t set_val;
    que_t root;
    que_t elm;

    que_init(&root);
    enque_top(&root, &elm);

    port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
    set_val = get_word(SCB + ICSR);
    set_field(uint32_t, set_val, 1<<28, 1);
    set_word(SCB + ICSR, set_val);
    */
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
