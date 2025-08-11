/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   eventflag.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "eventflag.h"
#include "kernel_config.h"
#include "kernel_define.h"
#include "kernel_schedule.h"
#include "task_manage.h"
#include "task_sync.h"
#include "que.h"
#include "utility.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  eventflag management object
 */
evtflgcb_t evtflgcb[EVTFLG_NUM];

/*--------------------------------------------------------------------------------------*/
/*! @brief  init evtflgcb
 */
static void init_evtflgcb(evtflgcb_t *p_evtflgcb, T_CFLG *p_cflg)
{
    memset(p_evtflgcb, 0 , sizeof(evtflgcb_t)); 
    p_evtflgcb->flgatr = p_cflg->flgatr;
    p_evtflgcb->flgptn = p_cflg->iflgptn;
    que_init(&p_evtflgcb->flg_que);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  judge flag match
 */
static bool is_match_flg(FLGPTN waiptn, FLGPTN flgptn, MODE wmode)
{
    bool rtn;

    if (wmode == EVENTFLAG_MODE_ANDW) {
        if ((waiptn & flgptn) == waiptn) {
            rtn = true;
        } else {
            rtn = false;
        }
    } else {
        if ((waiptn & flgptn) != 0) {
            rtn = true;
        } else {
            rtn = false;
        }
    }

    return rtn;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  init eventflag
 */
void kernel_flg_init(void)
{
    for (int i = 0; i < EVTFLG_NUM; i++) {
        memset(&evtflgcb[i], 0, sizeof(evtflgcb_t));
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  create eventflag
 */
ER cre_flg(ID evtflgid, T_CFLG *p_cflg)
{
    evtflgcb_t *p_evtflgcb;
    ER ercd = E_OK;

    // parameter check
    if (evtflgid >= EVTFLG_NUM) {
        return E_ID;
    }
    if (p_cflg == NULL) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get evtflgcb
    p_evtflgcb = &evtflgcb[evtflgid];
    // check evtflgcb
    if (p_evtflgcb->flg_que.p_next != NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }
    // init evtflgcb 
    init_evtflgcb(p_evtflgcb, p_cflg);

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  delete eventflag
 */
ER del_flg(ID evtflgid)
{
    evtflgcb_t *p_evtflgcb;
    ER ercd = E_OK;
    que_t *p_elm;
    tcb_t *p_tcb;

    // parameter check
    if (evtflgid >= EVTFLG_NUM) {
        return E_ID;
    }

    // start critical section
    critical_section_start();

    // get evtflgcb
    p_evtflgcb = &evtflgcb[evtflgid];
    // evtflgcb check
    if (p_evtflgcb->flg_que.p_next == NULL) {
        ercd = E_NOEXS;
        goto EXIT_CRITICAL_SECTION;
    }
    //  wake up task
    p_elm = p_evtflgcb->flg_que.p_next;
    while (p_elm != &p_evtflgcb->flg_que) {
        p_tcb = (tcb_t*)p_elm;
        kernel_task_wait_release(p_tcb, E_DLT);
        p_elm = p_elm->p_next;
    }
    // zero clear evtflgcb
    memset(p_evtflgcb, 0, sizeof(evtflgcb_t));
    // schedule
    kernel_schedule_task();

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  wait  eventflag
 */
ER wai_flg(ID evtflgid, FLGPTN waiptn, MODE wmode, FLGPTN *p_flgptn, TMO tmout)
{
    tcb_t *p_tcb = get_my_tcb();
    evtflgcb_t *p_evtflgcb;
    ER ercd;

    // parameter check
    if (evtflgid >= EVTFLG_NUM) {
        return E_ID;
    }
    if (p_flgptn == NULL) {
        return E_PAR;
    }
    if (((wmode & EVENTFLAG_MODE_ANDW) != EVENTFLAG_MODE_ANDW) && (wmode & EVENTFLAG_MODE_ORW) != EVENTFLAG_MODE_ORW) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get evtflgcb
    p_evtflgcb = &evtflgcb[evtflgid];
    // check evtflgcb
    if (p_evtflgcb->flg_que.p_next == NULL) {
        p_tcb->ercd = E_NOEXS;
        goto EXIT_CRITICAL_SECTION;
    }
    // pattern match 
    if (is_match_flg(waiptn, p_evtflgcb->flgptn, wmode) == true) {
        *p_flgptn = p_evtflgcb->flgptn;
        if ((p_evtflgcb->flgatr & TA_CLR) == TA_CLR) {
            p_evtflgcb->flgptn = 0;
        } 
    // pattern unmatch
    } else {
        if (tmout == TMO_POL) {
            p_tcb->ercd = E_TMOUT;
        } else {
            // save evtflg info
            p_tcb->winfo.flg_info.flgptn = 0;
            p_tcb->winfo.flg_info.waiptn = waiptn;
            p_tcb->winfo.flg_info.wmode = wmode;
            // make task wait
            kernel_task_make_wait(p_tcb, TS_WAIT, tmout, p_evtflgcb->flgatr, &p_evtflgcb->flg_que, evtflgid);
            // schedule
            kernel_schedule_task();
        }
    }

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    *p_flgptn = p_tcb->winfo.flg_info.flgptn;
    ercd = p_tcb->ercd;

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  set eventflag
 */
ER set_flg(ID evtflgid, FLGPTN setptn)
{
    evtflgcb_t *p_evtflgcb;
    tcb_t *p_tcb;
    que_t *p_elm;
    ER ercd = E_OK;

    // parameter check
    if (evtflgid >= EVTFLG_NUM) {
        return E_ID;
    }
    
    // start critical section
    critical_section_start();

    // get evtflgcb
    p_evtflgcb = &evtflgcb[evtflgid];
    // check evtflgcb
    if (p_evtflgcb->flg_que.p_next == NULL) {
        ercd = E_NOEXS;
    } else {
        // set flgptn
        p_evtflgcb->flgptn |= setptn;
        // check all wait task
        p_elm = p_evtflgcb->flg_que.p_next;
        while (p_elm != &p_evtflgcb->flg_que) {
            p_tcb = (tcb_t*)p_elm;
            if (is_match_flg(p_tcb->winfo.flg_info.waiptn, p_evtflgcb->flgptn, p_tcb->winfo.flg_info.wmode) == true) {
                // set match flag pattern
                p_tcb->winfo.flg_info.flgptn = p_evtflgcb->flgptn;
                // clear flag pattern
                if ((p_evtflgcb->flgatr & TA_CLR) == TA_CLR) {
                    p_evtflgcb->flgptn = 0;
                }
                // The que pointed by p_elm is updated by kernel_task_wait_release(), so save it
                p_elm = p_elm->p_prev;
                deque(&p_tcb->ready_que);
                // make task release
                kernel_task_wait_release(p_tcb, E_OK);
            }
            p_elm = p_elm->p_next;
        }
        // schedule
        kernel_schedule_task();
    }

    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  clear eventflag
 */
ER clr_flg(ID evtflgid, FLGPTN clrptn)
{
    evtflgcb_t *p_evtflgcb;
    ER ercd = E_OK; 

    // parameter check
    if (evtflgid >= EVTFLG_NUM) {
        return E_ID;
    }

    // start critical section
    critical_section_start();

    // get evtflgcb
    p_evtflgcb = &evtflgcb[evtflgid];
    // check evtflgcb
    if (p_evtflgcb->flg_que.p_next == NULL) {
        ercd = E_NOEXS;
        goto EXIT_CRITICAL_SECTION;
    }
    // clear flag
    p_evtflgcb->flgptn = 0;

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}
