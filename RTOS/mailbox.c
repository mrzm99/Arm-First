/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   mailbox.c
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "mailbox.h"
#include "kernel_define.h"
#include "kernel_config.h"
#include "utility.h"
#include "que.h"
#include "task_manage.h"
#include "task_sync.h"
#include "kernel_schedule.h"
#include <stdio.h>
#include <string.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  MailBox management object
 */
mbxcb_t mbxcb[MBX_NUM];

/*--------------------------------------------------------------------------------------*/
/*! @brief  init mbxcb
 */
static void init_mbxcb(mbxcb_t *p_mbxcb, T_CMBX *p_cmbx)
{
    memset(p_mbxcb, 0, sizeof(mbxcb_t));
    p_mbxcb->mbxatr = p_cmbx->mbxatr;
    que_init(&p_mbxcb->mbx_que);
    que_init(&p_mbxcb->msg_que);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel init MailBox
 */
void kernel_mbx_init(void)
{
    for (int i = 0; i < MBX_NUM; i++) {
        memset(&mbxcb[i], 0, sizeof(mbxcb_t));
        mbxcb[i].mbx_que.p_next = NULL;
        mbxcb[i].mbx_que.p_prev = NULL;
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  create mailbox  
 */
ER cre_mbx(ID mbxid, T_CMBX *p_cmbx)
{
    mbxcb_t *p_mbxcb;
    ER ercd = E_OK;

    // parameter check
    if (mbxid >= MBX_NUM) {
        return E_ID;
    }
    if (p_cmbx == NULL) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();
    
    // get mbxcb
    p_mbxcb = &mbxcb[mbxid];
    // check mbxcb
    if (p_mbxcb->mbx_que.p_next != NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }
    // init mbxcb
    init_mbxcb(p_mbxcb, p_cmbx);

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  delete mailbox
 */
ER del_mbx(ID mbxid)
{

}

/*--------------------------------------------------------------------------------------*/
/*! @brief  send message
 */
ER snd_mbx(ID mbxid, T_MSG *p_msg)
{
    tcb_t *p_tcb;
    mbxcb_t *p_mbxcb;
    ER ercd = E_OK;

    // parameter check
    if (mbxid >= MBX_NUM) {
        return E_ID;
    }
    if (p_msg == NULL) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get mbxcb
    p_mbxcb = &mbxcb[mbxid];
    // check mbxcb
    if (p_mbxcb->mbx_que.p_next == NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }
    // save message
    enque_last(&p_mbxcb->msg_que, p_msg);
    // wake up task
    if (p_mbxcb->mbx_que.p_next != &p_mbxcb->mbx_que) {
        // get tcb of wait task
        p_tcb = (tcb_t*)p_mbxcb->mbx_que.p_next;
        // deque wait task
        deque(p_mbxcb->mbx_que.p_next);
        // release wait task
        kernel_task_wait_release(p_tcb, E_OK);
        // schedule
        kernel_schedule_task();
    }

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  recv message
 */
ER rcv_mbx(ID mbxid, T_MSG **p_msg, TMO tmout)
{
    tcb_t *p_tcb;
    mbxcb_t *p_mbxcb;
    ER ercd = E_OK;

    if (mbxid >= MBX_NUM) {
        return E_ID;
    }
    if (p_msg == NULL) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get mbxcb
    p_mbxcb = &mbxcb[mbxid];
    // check mbxcb
    if (p_mbxcb->mbx_que.p_next == NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }
    // message exist
    if (p_mbxcb->msg_que.p_next != &p_mbxcb->msg_que) {
        *p_msg = p_mbxcb->msg_que.p_next;
        deque(p_mbxcb->msg_que.p_next);
    // messagae does not exist
    } else {
        if (tmout == TMO_POL) {
            ercd = E_TMOUT;
            goto EXIT_CRITICAL_SECTION;
        } else {
            // get tcb
            p_tcb = get_my_tcb();
            // make task wait
            kernel_task_make_wait(p_tcb, TW_MBX, tmout, p_mbxcb->mbxatr, &p_mbxcb->mbx_que, mbxid);
            // schedule
            kernel_schedule_task();
            // temporarily enable interrupt to insert PendSV interrupt
            enable_int();
            isb();
            disable_int();
            *p_msg = p_mbxcb->msg_que.p_next;
            deque(p_mbxcb->msg_que.p_next);
            ercd = p_tcb->ercd;
        }
    }


EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}
