/*--------------------------------------------------------------------------------------*/
/*!
 *      @file   mem_poolf.c 
 *      @date   2025.xx.xx
 *      @author mrzm99
 *      @brief  memory pool (fix) 
 *      @note
 */
/*--------------------------------------------------------------------------------------*/

#include "mem_poolf.h"
#include "kernel_config.h"
#include "kernel_define.h"
#include "kernel_schedule.h"
#include "utility.h"
#include "mem_alloc.h"
#include "que.h"
#include "task_manage.h"
#include "task_sync.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  mpf management object
 */
mpfcb_t mpfcb[MPF_NUM];

/*--------------------------------------------------------------------------------------*/
/*! @brief  init mpfcb
 */
static void init_mpfcb(mpfcb_t *p_mpfcb, T_CMPF *p_cmpf)
{
    memset(p_mpfcb, 0, sizeof(mpfcb_t));
    p_mpfcb->mpfatr = p_cmpf->mpfatr;
    p_mpfcb->blkcnt = p_cmpf->blkcnt;
    p_mpfcb->blksz = p_cmpf->blksz;
    p_mpfcb->mpf = p_cmpf->mpf;
    p_mpfcb->top_addr = (uint8_t*)p_cmpf->mpf;
    p_mpfcb->end_addr = (uint8_t*)p_cmpf->mpf + (p_cmpf->blkcnt * p_cmpf->blksz);
    que_init(&p_mpfcb->mpf_que);
    que_init(&p_mpfcb->free_blk_que);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  make free block que
 */
static void make_free_blk_que(mpfcb_t *p_mpfcb)
{
    que_t *p_elm = (que_t*)p_mpfcb->top_addr;

    for (int i = 0; i < p_mpfcb->blkcnt; i++) {
        enque_last(&p_mpfcb->free_blk_que, p_elm);
        p_elm = (que_t*)((uint8_t*)p_elm + p_mpfcb->blksz);
    } 
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
void kernel_mempoolf_init(void)
{
    for (int i = 0; i < MPF_NUM; i++) {
        memset(&mpfcb[i], 0, sizeof(mpfcb_t));
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  create memory pool
 */
ER cre_mpf(ID mpfid, T_CMPF *p_cmpf)
{
    mpfcb_t *p_mpfcb;
    ER ercd = E_OK;

    // parameter check
    if (mpfid >= MPF_NUM) {
        return E_ID;
    }
    if (p_cmpf == NULL) {
        return E_PAR;
    }
    if ((p_cmpf->blkcnt == 0) || (p_cmpf->blksz == 0)) {
        return E_PAR; 
    }

    // start critical section
    critical_section_start();

    // get mpfcb
    p_mpfcb = &mpfcb[mpfid];
    // check mpfcb
    if (p_mpfcb->mpf_que.p_next != NULL) {
        ercd = E_OBJ; 
        goto EXIT_CRITICAL_SECTION;
    }
    // init mpfcb
    init_mpfcb(p_mpfcb, p_cmpf);
    // get memory if p_cmpf->mpf is NULL
    if (p_cmpf->mpf == NULL) {
        if ((p_mpfcb->top_addr = mem_alloc(p_mpfcb->blkcnt * p_mpfcb->blksz)) == NULL) {
            ercd = E_NOMEM;
            goto EXIT_CRITICAL_SECTION;
        } else {
            p_mpfcb->end_addr = (uint8_t*)p_mpfcb->top_addr + (p_mpfcb->blkcnt * p_mpfcb->blksz);
        }
    }
    // make free block que
    make_free_blk_que(p_mpfcb);

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  delete memory pool
 */
ER del_mpf(ID mpfid)
{
    mpfcb_t *p_mpfcb;
    que_t *p_elm;
    tcb_t *p_tcb;
    ER ercd = E_OK;

    // parameter check
    if (mpfid >= MPF_NUM) { 
        return E_ID; 
    }

    // start critical section
    critical_section_start();

    // get mpfcb
    p_mpfcb = &mpfcb[mpfid];
    // check mpfcb
    if (p_mpfcb->mpf_que.p_next == NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }
    // release wait task
    p_elm = p_mpfcb->mpf_que.p_next;
    while (p_elm != &p_mpfcb->mpf_que) {
        // get tcb
        p_tcb = (tcb_t*)p_elm;
        // p_elm is dequed in kernel_task_wait_release() so put back one
        p_elm = p_elm->p_prev;
        // release task
        kernel_task_wait_release(p_tcb, E_DLT); 
        // next que
        p_elm = p_elm->p_next;
    }
    // release memory
    if (p_mpfcb->mpf == NULL) {
        mem_free(p_mpfcb->top_addr);
    }
    // clear mpfcb
    memset(p_mpfcb, 0, sizeof(mpfcb_t));

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

   return ercd; 
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  get memory block
 */
ER get_mpf(ID mpfid, VP *p_blk, TMO tmout)
{
    mpfcb_t *p_mpfcb;
    tcb_t *p_tcb = get_my_tcb();

    // parameter check
    if (mpfid >= MPF_NUM) {
        return E_ID;
    }

    // start critical section
    critical_section_start();

    // get mpfcb
    p_mpfcb = &mpfcb[mpfid];
    // check mpfcb
    if (p_mpfcb->mpf_que.p_next == NULL) {
        p_tcb->ercd = E_NOEXS;
        goto EXIT_CRITICAL_SECTION;
    }
    // free block exist 
    if (p_mpfcb->free_blk_que.p_next != &p_mpfcb->free_blk_que) {
        *p_blk = (VP)p_mpfcb->free_blk_que.p_next;
        deque(p_mpfcb->free_blk_que.p_next);
        p_tcb->ercd = E_OK;
    // free block does not exist
    } else {
        if (tmout == TMO_POL) {
            p_tcb->ercd = E_TMOUT; 
        } else {
            kernel_task_make_wait(p_tcb, TW_MPF, tmout, p_mpfcb->mpfatr, &p_mpfcb->mpf_que, mpfid);
            kernel_schedule_task();
            // To insert PendSV interrupt for dispatch, temporarily enable interrupt
            enable_int();
            isb();
            disable_int();
            if (p_tcb->ercd == E_OK) {
                *p_blk = (VP)p_mpfcb->free_blk_que.p_next;
                deque(p_mpfcb->free_blk_que.p_next);
            }
        }
    }

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return p_tcb->ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  release memory block
 */
ER rel_mpf(ID mpfid, VP *p_blk)
{
    mpfcb_t *p_mpfcb;
    tcb_t *p_tcb;
    ER ercd = E_OK;

    // parameter check
    if (mpfid >= MPF_NUM) {
        return E_ID;
    }
    if (p_blk == NULL) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get mpfcb
    p_mpfcb = &mpfcb[mpfid];
    // check mpfcb
    if (p_mpfcb->mpf_que.p_next == NULL) {
        ercd = E_NOEXS;
        goto EXIT_CRITICAL_SECTION;
    }
    // check p_blk
    if ((*(uint8_t**)p_blk < p_mpfcb->top_addr) || (*(uint8_t**)p_blk >= p_mpfcb->end_addr)) {
        ercd = E_PAR;
        goto EXIT_CRITICAL_SECTION;
    }
    // release block
    enque_last(&p_mpfcb->free_blk_que, (que_t*)*p_blk);
    // check wait task exist 
    if (p_mpfcb->mpf_que.p_next != &p_mpfcb->mpf_que) {
        // get wait tack tcb
        p_tcb = (tcb_t*)p_mpfcb->mpf_que.p_next;
        // release 
        kernel_task_wait_release(p_tcb, E_OK); 
    }

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();
    
    return ercd;
}
