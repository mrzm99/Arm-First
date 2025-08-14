/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   msgbuff.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __MSGBUFF_H__
#define __MSGBUFF_H__

#include "kernel_define.h"
#include "kernel_config.h"
#include "que.h"
#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  msgbuff control block
 */
typedef struct {
    que_t       snd_mbf_que;
    que_t       rcv_mbf_que;
    uint32_t    widx;
    uint32_t    rdix;

    ATR         mbfatr;
    uint32_t    maxmsz;
    uint32_t    mbfsz;
    VP          mbf;
} mbfcb_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief  T_CMBF
 */
typedef struct {
    ATR         mbfatr;     // message buffer attribute
    uint32_t    maxmsz;     // max messge size
    uint32_t    mbfsz;      // message buffer size
    VP          mbf;        // top addres of message buffer
} T_CMBF;

/*--------------------------------------------------------------------------------------*/
/*! @brief  kernel func
 */
void kernel_mbf_init(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  Service Call
 */
ER cre_mbf(ID mbfid, T_CMBF *p_cmbf);
ER del_mbf(ID mbfid);
ER snd_mbf(ID mbfid, VP msg, uint32_t msgsz, TMO tmout);
ER_ID rcv_mbf(ID mbfid, VP msg, TMO tmout);

#endif
