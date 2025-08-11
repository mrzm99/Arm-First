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
#include "eventflag.h"
#include <kernel_define.h>
#include <stdio.h>

static void test1_func(void)
{
    FLGPTN flgptn;
    wai_flg(0, 0xA0A0, EVENTFLAG_MODE_ANDW, &flgptn, 3000);
    while (1);
}

static void test2_func(void)
{
    set_flg(0, 0xA0);
    set_flg(0, 0xA000);
    slp_tsk(TMO_FEVR);
    while (1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    T_CFLG cflg;
    cflg.flgatr = TA_CLR;
    cflg.iflgptn = 0;
    if (cre_flg(0, &cflg)) {
        while (1);
    }
    
    T_CTSK init_ctsk;
    init_ctsk.tskatr = TA_HLANG|TA_ACT;
    init_ctsk.exinf = NULL;
    init_ctsk.task = test1_func;
    init_ctsk.itskpri = 1;
    init_ctsk.stksz = 256;
    init_ctsk.stk = NULL;
    if (cre_tsk(2, &init_ctsk)) {
        while (1);
    }

    T_CTSK test2;
    test2.tskatr = TA_HLANG|TA_ACT;
    test2.exinf = NULL;
    test2.task = test2_func;
    test2.itskpri = 2;
    test2.stksz = 256;
    test2.stk = NULL;
    
    // end app main
    slp_tsk(TMO_FEVR);

    // no reached 
    while (1); 
}
