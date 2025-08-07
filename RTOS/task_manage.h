/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   task_manage.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __TASK_MANAGE_H__
#define __TASK_MANAGE_H__

#include "que.h"
#include "kernel_define.h"
#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  Task Control Block (TCB)
 */
typedef struct {
    que_t       ready_que;      // ready que
    que_t       tim_que;        // timer que
    void        *p_stk;         // stack pointer
   
    ID          tskid;          // task id
    STAT        tskstat;        // task state
    PRI         tskpri;         // task current priority
    PRI         tskbpri;        // task base priority
    STAT        tskwait;        // wait factor
    ID          waiobjid;       // wait object id
    TMO         lefttmo;        // time out
    UINT        wupcnt;         // wake up queuing count
    UINT        actcnt;         // act queuing count
    UINT        suscnt;         // suspend request count

    ATR         tskatr;         // task attribute
    VP_INT      exinf;          // extension information (generate parameter)
    FP          task;           // task start address
    PRI         itskpri;        // wake up task priority
    SIZE        stksz;          // stack size
    VP          stk;            // stack top address
} tcb_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief  T_CTSK 
 */
typedef struct {
    ATR     tskatr;     // task attribute
    VP_INT  exinf;      // extension information
    FP      task;       // start address
    PRI     itskpri;    // wake up priority
    SIZE    stksz;      // stack size
    VP      stk;        // stack top address
} T_CTSK;

/*--------------------------------------------------------------------------------------*/
/*! @brief  TASK state 
 */
#define TS_RUN          (0)
#define TS_READY        (1)
#define TS_WAIT         (2)
#define TS_SUS          (3)
#define TS_WAI_SUS      (4)
#define TS_DMT          (5)
#define TS_NONEXIS      (6)

/*--------------------------------------------------------------------------------------*/
/*! @brief  TASK Attribute 
 */
#define TA_HLANG        (0)
#define TA_ASM          (1)
#define TA_ACT          (2)

/*--------------------------------------------------------------------------------------*/
/*! @brief  macro 
 */
#define put_rdy_que(p_tcb)      (enque_last(&knl_rdy_que_root[(p_tcb)->tskpri], &((p_tcb)->ready_que)))

/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel func 
 */
void kernel_task_init(void);
tcb_t *get_top_ready_que(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  Service Call 
 */
ER cre_tsk(ID tskid, T_CTSK *pk_ctsk);

#endif
