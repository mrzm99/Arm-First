/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   cyc_hdlr.h
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __CYC_HDLR_H__
#define __CYC_HDLR_H__

#include "kernel_define.h"
#include "kernel_config.h"
#include "que.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief  Cycle Control Block (CCB) 
 */
typedef struct {
    que_t       cyc_que;    // cyc que
    STAT        cycstat;    // cyc state
    RELTIM      lefttmo;    // cyc left time
    RELTIM      leftphs;    // cyc left time until start
    ID          cycid;      // cyc id

    ATR         cycatr;     // cyc hdlr attribute
    VP_INT      exinf;      // extension information
    FP          cychdr;     // cyc hdlr start address
    RELTIM      cyctime;    // cyc period
    RELTIM      cycphs;     // cyc phase
} ccb_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief  T_CCYC 
 */
typedef struct {
    ATR         cycatr;     // cyc hdlr attribute
    VP_INT      exinf;      // extension information
    FP          cychdr;     // cyc hdlr start address
    RELTIM      cyctime;    // cyc period
    RELTIM      cycphs;     // cyc phase
} T_CCYC;

/*--------------------------------------------------------------------------------------*/
/*! @brief  State 
 */
#define CS_NOEXT    (0x00)
#define CS_RUN      (0x01)
#define CS_STP      (0x02)
#define CS_PHS      (0x04)

/*--------------------------------------------------------------------------------------*/
/*! @brief  Attribute 
 */
#define TA_STA      (0x01)
#define TA_PHS      (0x02)


/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel func 
 */
ER sig_cyc(void); 
void kernel_cyc_init(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  Service Call 
 */
ER cre_cyc(ID cycid, T_CCYC *p_ccyc);
ER sta_cyc(ID cycid);
ER stp_cyc(ID cycid);

#endif
