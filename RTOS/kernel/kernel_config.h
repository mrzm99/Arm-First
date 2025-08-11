/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   kernel_config.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __KERNEL_CONFIG_H__
#define __KERNEL_CONFIG_H__

/*--------------------------------------------------------------------------------------*/
/*! @brief  TASK config 
 */
#define TSK_NUM             (10)                // TASK NUM
#define TSK_PRI_NUM         (10)                // TASK Priority NUM
#define TSK_PRI_MAX         (0)                 // TASK Priority Max 
#define TSK_PRI_MIN         (TSK_PRI_NUM-1)     // TASK Priority Min

#define TSK_WUP_MAX         (5)                 // Waku up count max

/*--------------------------------------------------------------------------------------*/
/*! @brief  Cycle Handler config 
 */
#define CYC_HDR_NUM         (5)                 // Cycle Hanlder NUM

/*--------------------------------------------------------------------------------------*/
/*! @brief  Semaphore config 
 */
#define SEM_NUM             (5)                 // Semaphore NUM

/*--------------------------------------------------------------------------------------*/
/*! @brief  Eventflag config
 */
#define EVTFLG_NUM          (5)                 // Eventflag NUM

#endif
