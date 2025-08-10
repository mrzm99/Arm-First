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
#include "semaphore.h"
#include <stdio.h>
#include <string.h>

static void test_tsk2(void)
{
    sig_sem(0);

    while (1);
}

static void test_tsk1(void)
{
    ER ercd;
    wai_sem(0, TMO_FEVR);

    T_CTSK ctsk;
    ctsk.tskatr = TA_HLANG|TA_ACT;
    ctsk.exinf = NULL;
    ctsk.task = test_tsk2;
    ctsk.itskpri = 3;
    ctsk.stksz = 256;
    ctsk.stk = NULL;
//    cre_tsk(1, &ctsk);

    ercd = wai_sem(0, 1000);

    if (ercd < 0) {
        while (1);
    }
    
    while (1);
/*
    uint32_t set_val;

    set_val = port_drv_get_pin_lvl(PORTB4); 
    set_val = (~set_val) & 1;
    port_drv_set_pin_lvl(PORTB4, set_val);
*/
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    T_CSEM csem;
    csem.sematr = TA_TFIFO;
    csem.inisem = 1;
    csem.maxsem = 1;
    cre_sem(0, &csem);
    port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
   
    T_CTSK ctsk;
    ctsk.tskatr = TA_HLANG|TA_ACT;
    ctsk.exinf = NULL;
    ctsk.task = test_tsk1;
    ctsk.itskpri = 0;
    ctsk.stksz = 256;
    ctsk.stk = NULL;

    cre_tsk(0, &ctsk);

    while (1); 
}
