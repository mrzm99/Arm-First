/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   kernel.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief   
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "kernel.h"
#include "systick.h"
#include "utility.h"
#include "system_define.h"
#include "mem_alloc/mem_alloc.h"
#include "task_manage.h"
#include "kernel_schedule.h"
#include "cyc_hdlr.h"
#include "eventflag.h"
#include "mem_poolf.h"
#include "semaphore.h"
#include <kernel_config.h>
#include <stdio.h>

extern void app_main(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel init task (idle loop for waiting interrupt)
 *          If wait in the PendSV handler, the next PendSV interrupt will not be received 
 *          and dispatch will become impossible. So create a task to wait for interrupts
 *          when there are no tasks to execute.
 */
static void kernel_init_task(void)
{
    while (1);
}
/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
void kernel_init(void)
{
    // kernel module init
    kernel_task_init();
    kernel_cyc_init();
    kernel_sem_init();
    kernel_flg_init();
    kernel_mempoolf_init();
    pendsv_init();
    systick_init();
    mem_init();
    
    // init systick 
    systick_init();
    systick_start(SYSTICK_CLK_AHB_DIV_8, AHB_CLOCK_FREQ/8);

    // create init task
    T_CTSK init_ctsk;
    init_ctsk.tskatr = TA_HLANG|TA_ACT;
    init_ctsk.exinf = NULL;
    init_ctsk.task = kernel_init_task;
    init_ctsk.itskpri = TSK_PRI_MIN;
    init_ctsk.stksz = 64;
    init_ctsk.stk = NULL;
    if (cre_tsk(0, &init_ctsk)) {
        while (1);
    }

    // create application main task
    T_CTSK app_ctsk;
    app_ctsk.tskatr = TA_HLANG|TA_ACT;
    app_ctsk.exinf = NULL;
    app_ctsk.task = app_main;
    app_ctsk.itskpri = TSK_PRI_MAX;
    app_ctsk.stksz = 1024;
    app_ctsk.stk = NULL;
    if (cre_tsk(1, &app_ctsk)) {
        while (1);
    }

    // enable interrupt
    enable_int(); 

    // never reached
    while (1);
}
