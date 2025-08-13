/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   mailbox.h
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __MAILBOX_H__
#define __MAILBOX_H__

#include "kernel_define.h"
#include "que.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief mailbox control block 
 */
typedef struct {
    que_t       mbx_que;        // wait task que
    que_t       msg_que;        // message que

    ATR         mbxatr;         // mailbox attribute
//-    PRI         maxpri;
//-    VP          mprihd;
} mbxcb_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief T_CMBX
 */
typedef struct {
    ATR         mbxatr;     // mailbox attribute
//-    PRI         maxpri;     // max message priority
//-    VP          mprihd;     // top address of priority que
} T_CMBX;

/*--------------------------------------------------------------------------------------*/
/*! @brief  T_MSG
 */
typedef que_t T_MSG;

/*--------------------------------------------------------------------------------------*/
/*! @brief  Kernel func
 */
void kernel_mbx_init(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief Service Call
 */
ER cre_mbx(ID mbxid, T_CMBX *p_cmbx);
ER del_mbx(ID mbxid);
ER snd_mbx(ID mbxid, T_MSG *p_msg);
ER rcv_mbx(ID mbxid, T_MSG **p_msg, TMO tmout);

 
#endif
