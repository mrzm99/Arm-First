/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   kernel_schedule.h
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief   
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __KERNEL_SCHEDULE_H__
#define __KERNEL_SCHEDULE_H__

#include "iodefine.h"
#include "utility.h"
#include "task_manage.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief  macro 
 */
// Request PendSV interrupt. Dispatch is executed within PendSV handler
#define dispatch_request()      do { \
                                    unsigned int set_val = get_word(SCB + ICSR); \
                                    set_field(uint32_t, set_val, 1<<28, 1); \
                                    set_word(SCB + ICSR, set_val); \
                                } while (0)
// Task schedule and request PendSV interrupt
#define kernel_schedule_task()  do { \
                                    p_knl_top_tcb = get_top_ready_que(); \
                                    dispatch_request(); \
                                } while (0)


/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel func 
 */
void kernel_setup_stack(tcb_t *p_tcb, VP_INT exinf);
void pendsv_init(void);


#endif
