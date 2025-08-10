/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   semaphore.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "semaphore.h"
#include "task_manage.h"
#include "kernel_schedule.h" 
#include "kernel_config.h"
#include "kernel_define.h"
#include "utility.h"
#include <string.h>
#include <stdio.h>
#include <task_sync.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  semaphore control object 
 */
semcb_t semcb[SEM_NUM];

/*--------------------------------------------------------------------------------------*/
/*! @brief  macro 
 */

/*--------------------------------------------------------------------------------------*/
/*! @brief  init semcb 
 */
static void init_semcb(semcb_t *p_semcb, T_CSEM *p_csem)
{
    memset(p_semcb, 0, sizeof(semcb_t));
    que_init(&p_semcb->sem_que);
    p_semcb->semcnt = p_csem->inisem;
    p_semcb->maxsem = p_csem->maxsem;
    p_semcb->sematr = p_csem->sematr;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  semaphore init
 */
void kernel_sem_init(void)
{
    for (int i = 0; i < SEM_NUM; i++) {
        memset(&semcb[i], 0, sizeof(semcb_t));
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  create semaphore
 */
ER cre_sem(ID semid, T_CSEM *p_csem)
{
    semcb_t *p_semcb;
    ER ercd = E_OK;

    // parameter checkk
    if (semid >= SEM_NUM) {
        return E_PAR;
    }
    if (p_csem == NULL) {
        return E_PAR;
    }
    
    // start critical section
    critical_section_start();

    // get semcb
    p_semcb = &semcb[semid];
    // check semcb
    if (p_semcb->sem_que.p_next != NULL) {
        ercd = E_OBJ;
        goto EXIT_CRITICAL_SECTION;
    }
    if (((p_csem->sematr & TA_TFIFO) != TA_TFIFO) && ((p_csem->sematr & TA_TPRI) != TA_TPRI)) {
        ercd = E_PAR;
        goto EXIT_CRITICAL_SECTION;
    } 
    // init semcb
    init_semcb(p_semcb, p_csem);

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  delete semphore
 */
ER del_sem(ID semid)
{
    semcb_t *p_semcb;
    tcb_t *p_tcb;
    que_t *p_elm;
    que_t *p_root;
    ER ercd = E_OK;

    // check parameter
    if (semid >= SEM_NUM) {
        return E_PAR;
    } 

    // start critical section
    critical_section_start();

    // get semcb
    p_semcb = &semcb[semid];
    // check semcb state
    if (p_semcb->sem_que.p_next == NULL) {
        ercd = E_NOEXS;
        goto EXIT_CRITICAL_SECTION;
    }
    // make task release
    p_root = &p_semcb->sem_que;
    p_elm = p_root->p_next;
    while (p_root != p_elm) {
        p_tcb = (tcb_t*)p_elm; 
        kernel_task_wait_release(p_tcb, E_DLT);        

        p_elm = p_elm->p_next;
    } 

    // schedule
    kernel_schedule_task();

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  waite semaphore
 */
ER wai_sem(ID semid, TMO tmout)
{
    semcb_t *p_semcb;
    tcb_t *p_tcb = get_my_tcb();

    // parameter check
    if (semid >= SEM_NUM) {
        return E_ID;
    }

    // start critical section
    critical_section_start();

    // semcb check
    p_semcb = &semcb[semid];
    if (p_semcb->sem_que.p_next == NULL) {
        p_tcb->ercd = E_NOEXS;
        goto EXIT_CRITICAL_SECTION;
    }
    // check resources
    if (p_semcb->semcnt > 0) {
        p_semcb->semcnt--;
        p_tcb->ercd = E_OK;
    } else {
        if (tmout == TMO_POL) {
            p_tcb->ercd = E_TMOUT;
        } else {
            kernel_task_make_wait(p_tcb, TW_SEM, tmout, p_semcb->sematr, &p_semcb->sem_que, semid); 
            kernel_schedule_task(); 
        } 
    }

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return p_tcb->ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  signal semaphore
 */
ER sig_sem(ID semid)
{
    semcb_t *p_semcb;
    tcb_t *p_tcb;
    ER ercd = E_OK;

    // parameter check
    if (semid >= SEM_NUM) {
        return E_ID;
    }
    
    // start critical section
    critical_section_start();

    // get semcb
    p_semcb = &semcb[semid];
    // check semcb
    if (p_semcb->sem_que.p_next == NULL) {
        ercd = E_NOEXS;
    }
    // check semcnt
    if (p_semcb->semcnt == p_semcb->maxsem) {
        ercd = E_QOVR; 
    }
    // check wait task
    if (p_semcb->sem_que.p_next != &p_semcb->sem_que) {
        // get wait task tcb
        p_tcb = (tcb_t*)p_semcb->sem_que.p_next;
        // deque from sem que
        deque(p_semcb->sem_que.p_next);
        // task waiting
        kernel_task_wait_release(p_tcb, E_OK);
        // schedule
        kernel_schedule_task();
    } else {
        // no task waiting
        p_semcb->semcnt++;
    }

    // end critical section
    critical_section_end();

    return ercd;
}
