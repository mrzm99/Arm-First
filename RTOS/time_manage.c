/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   time_manage.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "time_manage.h"
#include "kernel_define.h"
#include "utility.h"
#include "que.h"
#include "kernel_schedule.h"
#include "task_manage.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief  macro 
 */
#define get_tcb_from_tim_que(p_elm)     ((tcb_t*)((uint8_t*)p_elm-sizeof(que_t)))

/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel sig tim 
 */
static void knl_sig_tim(void)
{
    que_t *p_elm;
    tcb_t *p_tcb;

    // get knl_tim_que_root top
    p_elm = knl_tim_que_root.p_next;
    // loop all elements
    while (&knl_tim_que_root != p_elm) {
        // get TCP from time que 
        p_tcb = get_tcb_from_tim_que(p_elm);
        // check time left
        if (p_tcb->lefttmo != 0) {
            // decriment left time
            p_tcb->lefttmo--;         
        } else {
            // get rid of ques like sem que, evt que etc

            // deque time que
            p_elm = p_elm->p_prev;
            deque(&p_tcb->tim_que);
            // clear tskwait
            p_tcb->tskwait = TW_NOWAIT;
            // update tsktat
            p_tcb->tskstat &= ~TS_WAIT;
            // put ready que if not suspend
            if (p_tcb->tskstat != 0) {
                // task is suspended
            } else {
                p_tcb->tskstat = TS_READY;
                put_rdy_que(p_tcb);
            }

        }
        p_elm = p_elm->p_next;
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  sig timer 
 */
ER sig_time(void)
{
    // start critical section
    critical_section_start();

    // timer tick process
    knl_sig_tim();
    // cycle handler process
    // alarm handler process
    // overrun hanlder process
    // task schedule
    kernel_schedule_task();

    // end critical section
    critical_section_end();

    return E_OK;
}
