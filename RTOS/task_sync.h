/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   task_sync.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __TASK_SYNC_H__
#define __TASK_SYNC_H__

#include "kernel_define.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief  tmo macro 
 */
#define TMO_POL         (0)
#define TMO_FEVR        (-1)

/*--------------------------------------------------------------------------------------*/
/*! @brief  Service Call 
 */
ER wup_tsk(ID tskid);
ER slp_tsk(TMO tmout);
ER dly_tsk(RELTIM dlytim);

#endif
