/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   eventflag.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __EVENTFLAG_H__
#define __EVENTFLAG_H__

#include "kernel_define.h"
#include "kernel_config.h"
#include "que.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief evtflg control block  
 */
typedef struct {
    que_t       flg_que;    // eventflag que
    FLGPTN      flgptn;     // flag pattern
    ATR         flgatr;     // flag attribute
} evtflgcb_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief  evtflg info
 */
typedef struct {
    FLGPTN      waiptn;     // wait pattern
    MODE        wmode;      // wait MODE
    FLGPTN      flgptn;     // pattern that flag established
} evtflg_info_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief T_CFLG
 */
typedef struct {
    ATR         flgatr;     // flag attribute
    FLGPTN      iflgptn;    // init flag pattern
} T_CFLG;

/*--------------------------------------------------------------------------------------*/
/*! @brief  Attribute
 */
#define TA_CLR      (0x01)
#define TA_UNCLER   (0x02)

/*--------------------------------------------------------------------------------------*/
/*! @brief MODE
 */
#define EVENTFLAG_MODE_ANDW     (0x01)      // AND wait
#define EVENTFLAG_MODE_ORW      (0x02)      // OR wait

/*--------------------------------------------------------------------------------------*/
/*! @brief kernel func 
 */
void kernel_flg_init(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief Service Call 
 */
ER cre_flg(ID evtflgid, T_CFLG *p_cflg);
ER del_flg(ID evtflgid);
ER wai_flg(ID evtflgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn, TMO tmout);
ER set_flg(ID evtflgid, FLGPTN setptn);
ER clr_flg(ID evtflgid, FLGPTN clrptn);

#endif
