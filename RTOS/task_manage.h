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
#include "eventflag.h"
#include "kernel_define.h"
#include "kernel_config.h"
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
    ER          ercd;           // error code

    ATR         tskatr;         // task attribute
    VP_INT      exinf;          // extension information (generate parameter)
    FP          task;           // task start address
    PRI         itskpri;        // wake up task priority
    SIZE        stksz;          // stack size
    VP          stk;            // stack top address
    
    que_t       *p_root_que;    // Not NULL if TA_PRI specified
    
    union {
        evtflg_info_t flg_info; // eventflag info
    } winfo;
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
#define TS_RUN          (0x00)
#define TS_READY        (0x01)
#define TS_WAIT         (0x02)
#define TS_SUS          (0x04)
#define TS_WAI_SUS      (0x08)
#define TS_DMT          (0x10)
#define TS_NOEXIS       (0x20)

/*--------------------------------------------------------------------------------------*/
/*! @brief  TASK wait 
 */
#define TW_NOWAIT       (0x0000)
#define TW_SLP          (0x0001)
#define TW_DLY          (0x0002)
#define TW_SEM          (0x0004)
#define TW_FLG          (0x0008)
#define TW_SDTQ         (0x0010)
#define TW_RDTQ         (0x0020)
#define TW_MBX          (0x0040)
#define TW_MTX          (0x0080)
#define TW_SMBF         (0x0100)
#define TW_RMBF         (0x0200)
#define TW_MPF          (0x0400)
#define TW_MPL          (0x0800)

/*--------------------------------------------------------------------------------------*/
/*! @brief  TASK Attribute 
 */
#define TA_HLANG        (0)
#define TA_ASM          (1)
#define TA_ACT          (2)

/*--------------------------------------------------------------------------------------*/
/*! @brief  QUE Attribute 
 */
#define TA_TFIFO        (0x01)
#define TA_TPRI         (0x02)
#define TA_WSIG         (0x04)      // enable single que wait
#define TA_WMUL         (0x08)      // enable multi que wait


/*--------------------------------------------------------------------------------------*/
/*! @brief  macro 
 */
#define put_rdy_que(p_tcb)              (enque_last(&knl_rdy_que_root[(p_tcb)->tskpri], &((p_tcb)->ready_que)))
#define put_tim_que(p_tcb)              (enque_last(&knl_tim_que_root, &((p_tcb)->tim_que)))
#define get_tcb_from_tskid(tskid)       (&tcb[(tskid)])
#define get_my_tcb()                    (p_knl_run_tcb) 

/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel func 
 */
void kernel_task_init(void);
tcb_t *get_top_ready_que(void);
void kernel_task_make_wait(tcb_t *p_tcb, STAT tskwait, TMO tmout, ATR objatr, que_t *p_root_que, ID objid);
void kernel_task_wait_release(tcb_t *p_tcb, ER ercd);
void kernel_tcb_change_tskpri(tcb_t *p_tcb, PRI tskpri);

/*--------------------------------------------------------------------------------------*/
/*! @brief  task management object (global variable)
 */
extern tcb_t tcb[TSK_NUM];
extern tcb_t *p_knl_run_tcb;
extern tcb_t *p_knl_top_tcb;
extern que_t knl_rdy_que_root[TSK_PRI_NUM];
extern que_t knl_tim_que_root;

/*--------------------------------------------------------------------------------------*/
/*! @brief  Service Call 
 */
ER cre_tsk(ID tskid, T_CTSK *pk_ctsk);
ER chg_pri(ID tskid, PRI tskpri);

#endif
