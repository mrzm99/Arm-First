/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   task_manage.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief   
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "task_manage.h"
#include "que.h"
#include "kernel_config.h"
#include "kernel_define.h"
#include "kernel_schedule.h"
#include "utility.h"
#include "mem_alloc.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  macro 
 */
#define is_que_empty(p_que)             ((p_que)->p_next == (p_que)?true:false)

/*--------------------------------------------------------------------------------------*/
/*! @brief  task management object
 */
tcb_t tcb[TSK_NUM];                 // TCB entity
tcb_t *p_knl_run_tcb;                   // run state TCB
tcb_t *p_knl_top_tcb;                   // top TCB of ready que
que_t knl_rdy_que_root[TSK_PRI_NUM];    // ready que
que_t knl_tim_que_root;                 // timer que

/*--------------------------------------------------------------------------------------*/
/*! @brief  init TCB 
 */
static void init_tcb(tcb_t *p_tcb, ID tskid, T_CTSK *pk_ctsk)
{
    memset(p_tcb, 0, sizeof(tcb_t));

    que_init(&p_tcb->ready_que);
    que_init(&p_tcb->tim_que);
    p_tcb->p_stk = NULL;
    
    p_tcb->tskid = tskid;
    p_tcb->tskstat = TS_DMT;
    p_tcb->tskpri = pk_ctsk->itskpri;
    p_tcb->tskbpri = pk_ctsk->itskpri;

    p_tcb->tskatr = pk_ctsk->tskatr;
    p_tcb->exinf = pk_ctsk->exinf;
    p_tcb->task = pk_ctsk->task;
    p_tcb->itskpri = pk_ctsk->itskpri;
    p_tcb->stksz = pk_ctsk->stksz;
    p_tcb->stk = pk_ctsk->stk;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  make TCB ready 
 */
static void make_tcb_ready(tcb_t *p_tcb, VP_INT *exinf)
{
    p_tcb->p_stk = p_tcb->stk + p_tcb->stksz;
    kernel_setup_stack(p_tcb, exinf);
    p_tcb->tskstat = TS_READY;
    p_tcb->tskwait = 0;
    p_tcb->waiobjid = 0;
    p_tcb->lefttmo = 0;
    p_tcb->wupcnt = 0;
    p_tcb->actcnt = 0;
    p_tcb->suscnt = 0;
    put_rdy_que(p_tcb);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  get ready top 
 */
tcb_t *get_top_ready_que(void)
{
    for (uint32_t i = 0; i < TSK_PRI_NUM; i++) {
        if (is_que_empty(&knl_rdy_que_root[i]) == false) {
            return (tcb_t*)knl_rdy_que_root[i].p_next;
        }
    }

    return NULL;
}
/*--------------------------------------------------------------------------------------*/
/*! @brief  task init
 */
void kernel_task_init(void)
{
    // zero clear TCB
    memset(tcb, 0, sizeof(tcb_t) * TSK_NUM);
    // init global variables
    p_knl_run_tcb = NULL;
    p_knl_top_tcb = NULL;
    for (int i = 0; i < TSK_NUM; i++) {
        tcb[i].tskid = -1;
    }
    for (int i = 0; i < TSK_PRI_NUM; i++) {
        que_init(&knl_rdy_que_root[i]);
    }
    que_init(&knl_tim_que_root);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  create task 
 */
ER cre_tsk(ID tskid, T_CTSK *pk_ctsk)
{
    tcb_t *p_tcb;

    // parameter check
    if (pk_ctsk == NULL) {
        return E_PAR;
    }
    if (pk_ctsk->task == NULL) {
        return E_PAR;
    }
    if (pk_ctsk->itskpri >= TSK_PRI_NUM) {
        return E_PAR;
    } 
    if (pk_ctsk->stksz == 0) {
        return E_PAR;
    }
    if (tskid >= TSK_NUM) {
        return E_PAR;
    }

    // >>>> start critical section >>>>
    critical_section_start();

    // get TCB from tskid 
    p_tcb = get_tcb_from_tskid(tskid);
    // check TCB used
    if (p_tcb->tskid != -1) {
        return E_OBJ;
    }
    // get stack if pk_ctsk->stksize == NULL
    if (pk_ctsk->stk == NULL) {
        pk_ctsk->stk = (void*)mem_alloc(pk_ctsk->stksz);
        if (pk_ctsk->stk == NULL) {
            return E_NOMEM;
        }
    }
    // init TCB
    init_tcb(p_tcb, tskid, pk_ctsk);
    // make TCB ready if TA_ACT attribute has specified 
    if ((pk_ctsk->tskatr & TA_ACT) == TA_ACT) {
        make_tcb_ready(p_tcb, pk_ctsk->exinf);
        kernel_schedule_task();
    }

    // <<<< end critical section <<<<
    critical_section_end();

    return E_OK;
}

