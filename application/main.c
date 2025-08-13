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
#include "mailbox.h"
#include "mem_alloc.h"
#include <stdio.h>

typedef struct {
    T_MSG   msghd;
    UW      lvl;
} msg_t;

static void test_cyc(void)
{
    uint32_t set_val;

    set_val = port_drv_get_pin_lvl(PORTB4); 
    set_val = (~set_val) & 1;
    port_drv_set_pin_lvl(PORTB4, set_val);
}

static void test1(void)
{
    uint32_t lvl;
    msg_t *msg;

    while (1) {
        if (rcv_mbx(0, (T_MSG**)&msg, TMO_FEVR) == E_OK) {
            lvl = msg->lvl;
            port_drv_set_pin_lvl(PORTB4, lvl);
//            mem_free(msg);
        }  
    } 
}

static void test2(void)
{
    uint32_t lvl = 0;
    msg_t *msg;

    while (1) {
        msg = (msg_t*)mem_alloc(sizeof(msg_t)); 
        msg->lvl = lvl;
        lvl = ~lvl & 1;
        snd_mbx(0, (T_MSG*)msg);
        dly_tsk(1000);
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);

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

    T_CMBX cmbx;
    cmbx.mbxatr = TA_TFIFO;
    if (cre_mbx(0, &cmbx)) {
        while (1);
    }
   
    // end app main
    slp_tsk(TMO_FEVR);

    // no reached 
    while (1); 
}
