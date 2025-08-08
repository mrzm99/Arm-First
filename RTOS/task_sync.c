/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   task_sync.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "task_sync.h"
#include "task_manage.h"
#include "kernel_define.h"
#include "utility.h"
#include "que.h"
#include "kernel_schedule.h"
#include <kernel_config.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  wake up task 
 */
ER wup_tsk(ID tskid)
{
    tcb_t *p_tcb;
    ER ercd = E_OK;

    if (tskid >= TSK_NUM) {
        return E_PAR; 
    }

    // start critical section 
    critical_section_start();

    // get TCB from tskid
    p_tcb = get_tcb_from_tskid(tskid);
    // check task state
    if (p_tcb->tskstat == TS_NOEXIS) {
        ercd = E_NOEXS;
        goto END_CRITICAL_SECTION;
    } else if (p_tcb->tskstat == TS_DMT) {
        ercd = E_OBJ;
        goto END_CRITICAL_SECTION;
    } else if (p_tcb->wupcnt >= TSK_WUP_MAX) {
        ercd = E_QOVR;
        goto END_CRITICAL_SECTION;
    } else {
        // wake up if the task sleeps
        if ((p_tcb->tskstat == TS_WAIT) && (p_tcb->tskwait == TW_SLP)) {
            // get rid of tim que
            deque(&p_tcb->tim_que);
            // clear tskwait
            p_tcb->tskwait = TW_NOWAIT;
            // update tskstat
            p_tcb->tskstat &= ~TS_WAIT;
            // wake up task
            if (p_tcb->tskstat != 0) {
                // task is suspended
            } else {
                p_tcb->tskstat = TS_READY;
                put_rdy_que(p_tcb);
                kernel_schedule_task();
            }
        } else {
            p_tcb->wupcnt++;
        }
    }

END_CRITICAL_SECTION:
    // end critical section  
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  sleep task 
 */
ER slp_tsk(TMO tmout)
{
    tcb_t *p_tcb;
    ER ercd = E_OK;

    // start critical section
    critical_section_start();

    // get tcb
    p_tcb = p_knl_run_tcb;
    // check wakeup cnt 
    if (p_tcb->wupcnt  != 0) {
        p_tcb->wupcnt--;
    } else {
        // deque from ready que
        deque(&p_tcb->ready_que);
        if (tmout == TMO_POL) {
            put_rdy_que(p_tcb);
        } else {
            // update task state
            p_tcb->tskstat = TS_WAIT;
            p_tcb->tskwait = TW_SLP;
            // set error code
            p_tcb->ercd = E_TMOUT;
            // put timer que
            if (tmout == TMO_FEVR) {
                // nope
            } else {
                // set tmout
                p_tcb->lefttmo = tmout;
                // put timer que  
                put_tim_que(p_tcb);
            }
        }
        // schedule task
        kernel_schedule_task();
    }

    // end critical section
    critical_section_end();

    ercd = p_tcb->ercd;

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  delay task 
 */
ER dly_tsk(RELTIM dlytim)
{
    tcb_t *p_tcb;

    // start critical section
    critical_section_start();

    // get tcb
    p_tcb = p_knl_run_tcb;
    // deque from ready que
    deque(&p_tcb->ready_que);
    // set delay time
    p_tcb->lefttmo = dlytim;
    // update task state
    p_tcb->tskstat = TS_WAIT;
    p_tcb->tskwait = TW_DLY;
    // put timer que
    put_tim_que(p_tcb);
    // schedule task
    kernel_schedule_task();

    // end critical section
    critical_section_end();

    if (p_tcb->lefttmo != 0) {
        return E_RLWAI;
    }

    return E_OK;
}
