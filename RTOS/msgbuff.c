/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   msgbuff.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "msgbuff.h"
#include "que.h"
#include "kernel_config.h"
#include "kernel_define.h"
#include "kernel_schedule.h"
#include "task_manage.h"
#include "utility.h"
#include "mem_alloc.h"
#include <stdio.h>
#include <string.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  message buffer management object
 */
mbfcb_t mbfcb[MBF_NUM];

/*--------------------------------------------------------------------------------------*/
/*! @brief  macro
 */
#define HEAD_SIZE       (4)

/*--------------------------------------------------------------------------------------*/
/*! @brief  init mbfcb
 */
static void init_mbfcb(mbfcb_t *p_mbfcb, T_CMBF *p_cmbf)
{
    memset(p_mbfcb, 0, sizeof(mbfcb_t));
    p_mbfcb->mbfatr = p_cmbf->mbfatr;
    p_mbfcb->maxmsz = p_cmbf->maxmsz;
    p_mbfcb->mbfsz = p_cmbf->mbfsz;
    p_mbfcb->mbf = p_cmbf->mbf;
    que_init(&p_mbfcb->snd_mbf_que);
    que_init(&p_mbfcb->rcv_mbf_que);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel init mbf
 */
void kernel_mbf_init(void)
{
    for (int i = 0; i < MBF_NUM; i++) {
        memset(&mbfcb[i], 0, sizeof(mbfcb_t));
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  create messge buffer
 */
ER cre_mbf(ID mbfid, T_CMBF *p_cmbf)
{
    mbfcb_t *p_mbfcb;
    ER ercd = E_OK;

    // parameter check
    if (mbfid >= MBF_NUM) {
        return E_ID;
    }
    if (p_cmbf == NULL) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get mbfcb
    p_mbfcb = &mbfcb[mbfid];
    // check mbfcb
    if (p_mbfcb->snd_mbf_que.p_next != NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }
    // init mbfcb
    init_mbfcb(p_mbfcb, p_cmbf);
    // get message buffer memory
    if (p_mbfcb->mbf == NULL) {
        if ((p_mbfcb->mbf = mem_alloc(p_mbfcb->mbfsz)) == NULL) {
            ercd = E_NOMEM;
        }
    }

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  delete message buffer
 */
ER del_mbf(ID mbfid)
{
    mbfcb_t *p_mbfcb;
    tcb_t *p_tcb;
    que_t *p_elm;

    // parameter check
    if (mbfid >= MBF_NUM) {
        return E_ID;
    }

    // start critical section
    critical_section_start();

    // get mbfcb
    p_mbfcb = &mbfcb[mbfid];
    // check mbfcb
    if (p_mbfcb->snd_mbf_que.p_next == NULL) {
        goto EXIT_CRITICAL_SECTION;
    }
    // release wait send task
    p_elm = p_mbfcb->snd_mbf_que.p_next;
    while (&p_mbfcb->snd_mbf_que != p_elm) {
        p_tcb = (tcb_t*)p_elm;
        kernel_task_wait_release(p_tcb, E_DLT); 
        p_elm = p_elm->p_next;
    }
    // release wait recv task
    p_elm = p_mbfcb->rcv_mbf_que.p_next;
    while (&p_mbfcb->rcv_mbf_que != p_elm) {
        p_tcb = (tcb_t*)p_elm;
        kernel_task_wait_release(p_tcb, E_DLT);
        p_elm = p_elm->p_next;
    }
    // schedule
    kernel_schedule_task();

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return E_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  send message
 */
ER snd_mbf(ID mbfid, VP msg, uint32_t msgsz, TMO tmout)
{
    mbfcb_t *p_mbfcb;
    tcb_t *p_tcb;
    uint32_t free_size;
    uint32_t widx;
    uint32_t ridx;
    uint8_t *p_buff;
    uint8_t *p_data;
    ER ercd = E_OK;

    // parameter check
    if (mbfid >= MBF_NUM) {
        return E_ID;
    }
    if ((msg == NULL) || (msgsz == 0)) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get mbfcb
    p_mbfcb = &mbfcb[mbfid];
    // check mbfcb
    if (p_mbfcb->snd_mbf_que.p_next == NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }

    // calc free buff size
    widx = p_mbfcb->widx;
    ridx = p_mbfcb->rdix;
    free_size = widx >= ridx ? (p_mbfcb->mbfsz-(widx-ridx)) : (ridx-widx);

    // get message buffer (ring buffer) 
    p_buff = (uint8_t*)p_mbfcb->mbf; 
    p_data = (uint8_t*)msg;

    // wait until sufficient buff is available
    p_tcb = get_my_tcb();
    while (free_size < (msgsz+HEAD_SIZE)) {
        kernel_task_make_wait(p_tcb, TW_SMBF, tmout, p_mbfcb->mbfatr, &p_mbfcb->snd_mbf_que, mbfid);
        kernel_schedule_task();
        // temporarily enable interrupt to insert PendSV interrupt
        enable_int();
        isb();
        disable_int();
        // check ercd
        if (p_tcb->ercd != E_OK) {
            ercd = p_tcb->ercd;
            goto EXIT_CRITICAL_SECTION;
        }
        // calc free buff size
        widx = p_mbfcb->widx;
        ridx = p_mbfcb->rdix;
        free_size = widx >= ridx ? (p_mbfcb->mbfsz-(widx-ridx)) : (ridx-widx);
    }

    // wirte head (msg size)
    *(uint32_t*)p_buff = msgsz;
    widx += HEAD_SIZE;
    // write message
    while (msgsz > 0) {
        p_buff[widx] = *p_data;
        widx++;
        p_data++;
        msgsz--;
        if (widx >= p_mbfcb->mbfsz) {
            widx = 0;
        }
    } 

    // wake up wait recv task
    if (&p_mbfcb->rcv_mbf_que != p_mbfcb->rcv_mbf_que.p_next) {
        p_tcb = (tcb_t*)p_mbfcb->rcv_mbf_que.p_next;
        deque(p_mbfcb->rcv_mbf_que.p_next);
        kernel_task_wait_release(p_tcb, E_OK);
        kernel_schedule_task();
    }

    // update widx
    p_mbfcb->widx = widx;

EXIT_CRITICAL_SECTION:
     // end critical_ section
     critical_section_end();

     return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  receive message
 */
ER_ID rcv_mbf(ID mbfid, VP msg, TMO tmout)
{
    mbfcb_t *p_mbfcb;
    tcb_t *p_tcb;
    uint32_t recv_msg_size;
    uint32_t widx;
    uint32_t ridx;
    uint8_t *p_buff;
    uint8_t *p_rbuff;
    ER_ID ercd;

    // parameter check
    if (mbfid >= MBF_NUM) {
        return E_ID;
    }
    if (msg == NULL) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get mbfcb
    p_mbfcb = &mbfcb[mbfid];
    // check mbfcb
    if (p_mbfcb->snd_mbf_que.p_next == NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }
    
    // get idx 
    widx = p_mbfcb->widx;
    ridx = p_mbfcb->rdix;
    // no message
    if (widx == ridx) {
        p_tcb = get_my_tcb();
        kernel_task_make_wait(p_tcb, TW_RMBF, tmout, p_mbfcb->mbfatr, &p_mbfcb->rcv_mbf_que, mbfid);
        kernel_schedule_task();
        enable_int();
        isb();
        disable_int();
        if (p_tcb->ercd != E_OK) {
            ercd = p_tcb->ercd;
            goto EXIT_CRITICAL_SECTION;
        }
    }

    // 
    ridx = p_mbfcb->rdix;
    p_buff = (uint8_t*)p_mbfcb->mbf;
    p_rbuff = (uint8_t*)msg;
    // get message size
    recv_msg_size = *(uint32_t*)p_buff;
    ercd = recv_msg_size;
    ridx += HEAD_SIZE;
    // copy message
    while (recv_msg_size > 0) {
        *p_rbuff = p_buff[ridx];
        p_rbuff++;
        ridx++;
        recv_msg_size--;
        if (ridx >= p_mbfcb->mbfsz) {
            ridx = 0;
        }
    }
     
    // update ridx
    p_mbfcb->rdix = ridx;

    // wake up wait send task
    if (&p_mbfcb->snd_mbf_que != p_mbfcb->snd_mbf_que.p_next) {
        p_tcb = (tcb_t*)p_mbfcb->snd_mbf_que.p_next;
        deque(p_mbfcb->snd_mbf_que.p_next);
        kernel_task_wait_release(p_tcb, E_OK);
        kernel_schedule_task();
    }

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}
