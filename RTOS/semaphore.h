/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   semaphore.h
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "que.h"
#include "kernel_define.h"
#include <stdint.h>

 /*--------------------------------------------------------------------------------------*/
 /*! @brief semaphore control block 
  */
typedef struct {
    que_t       sem_que;    // semaphore que
    uint32_t    semcnt;     // current semaphore count 
    uint32_t    maxsem;     // max semaphore count
    ATR         sematr;     // semaphore attribute
} semcb_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief  T_CSEM 
 */
typedef struct {
    ATR         sematr;     // semaphore attribute
    uint32_t    inisem;     // initial semaphore count
    uint32_t    maxsem;     // max semaphore count
} T_CSEM;

/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel func 
 */
void kernel_sem_init(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  Service Call 
 */
ER cre_sem(ID semid, T_CSEM *p_csem);
ER wai_sem(ID semid, TMO tmout);
ER sig_sem(ID semid);
ER del_sem(ID semid);


#endif
