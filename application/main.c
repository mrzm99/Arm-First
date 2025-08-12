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
#include "mem_poolf.h"

static void test_cyc(void)
{
    uint32_t set_val;

    set_val = port_drv_get_pin_lvl(PORTB4); 
    set_val = (~set_val) & 1;
    port_drv_set_pin_lvl(PORTB4, set_val);
}

static void test1(void)
{
    uint8_t *p_mem;

    slp_tsk(TMO_FEVR);

    get_mpf(0, &p_mem, TMO_FEVR);

    while (1);
}

static void test2(void)
{
    uint8_t *p_mem;
    get_mpf(0, &p_mem, TMO_FEVR);
    get_mpf(0, &p_mem, TMO_FEVR);
    get_mpf(0, &p_mem, TMO_FEVR);
    wup_tsk(2);
    rel_mpf(0, &p_mem);

    while (1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    T_CCYC ccyc;
    ccyc.cycatr = TA_HLANG|TA_STA;
    ccyc.exinf = NULL;
    ccyc.cychdr = test_cyc;
    ccyc.cyctime = 1000;
    ccyc.cycphs = 0;
    port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);

    if (cre_cyc(0, &ccyc) < 0) {
        while (1);
    }
    
    T_CMPF cmpf;
    cmpf.mpfatr = TA_TFIFO;
    cmpf.blkcnt = 3;
    cmpf.blksz = 64;
    cmpf.mpf = NULL;
    if (cre_mpf(0, &cmpf) < 0) {
        while (1);
    }

    T_CTSK init_ctsk;
    init_ctsk.tskatr = TA_HLANG|TA_ACT;
    init_ctsk.exinf = NULL;
    init_ctsk.task = test1;
    init_ctsk.itskpri = 0;
    init_ctsk.stksz = 256;
    init_ctsk.stk = NULL;
    if (cre_tsk(2, &init_ctsk)) {
        while (1);
    }

    init_ctsk.tskatr = TA_HLANG|TA_ACT;
    init_ctsk.exinf = NULL;
    init_ctsk.task = test2;
    init_ctsk.itskpri = 1;
    init_ctsk.stksz = 256;
    init_ctsk.stk = NULL;
    if (cre_tsk(3, &init_ctsk)) {
        while (1);
    }

    uint8_t *p_mem1, *p_mem2, *p_mem3;
    get_mpf(0, &p_mem1, TMO_FEVR);
    get_mpf(0, &p_mem2, TMO_FEVR);
    get_mpf(0, &p_mem3, TMO_FEVR);
    for (int i = 0; i < 64; i++) {
        *p_mem1++ = 0xA5;
        *p_mem2++ = 0xA5;
        *p_mem3++ = 0xA5;
    }
    rel_mpf(0, &p_mem2);
    p_mem2 = NULL;
    get_mpf(0, &p_mem2, TMO_FEVR);
    for (int i = 0; i < 64; i++) {
        *p_mem2++ = 0x00;
    }


    slp_tsk(TMO_FEVR);

    // no reached 
    while (1); 
}
