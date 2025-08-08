/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   cyc_hldr.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "cyc_hdlr.h"
#include "que.h"
#include "kernel_config.h"
#include "kernel_define.h"
#include "utility.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  macro 
 */
#define put_cyc_que(p_ccb)                  (enque_last(&cyc_que, &(p_ccb)->cyc_que))
#define get_ccb_from_ccy_que(p_ccy_que)     ((ccb_t *)(p_ccy_que))

/*--------------------------------------------------------------------------------------*/
/*! @brief  Cycle Handler management object 
 */
static ccb_t ccb[CYC_HDR_NUM];
static que_t cyc_que;

/*--------------------------------------------------------------------------------------*/
/*! @brief  init ccb 
 */
static void init_ccb(ID cycid, ccb_t *p_ccb, T_CCYC *p_ccyc)
{
    memset(p_ccb, 0, sizeof(ccb_t));
    p_ccb->cycid = cycid;
    p_ccb->cycatr = p_ccyc->cycatr;
    p_ccb->exinf = p_ccyc->exinf;
    p_ccb->cychdr = p_ccyc->cychdr;
    p_ccb->cyctime = p_ccyc->cyctime;
    p_ccb->cycphs = p_ccyc->cycphs;
    que_init(&p_ccb->cyc_que);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  put into cyc que 
 */
static void rdy_cyc_que(ccb_t *p_ccb)
{
    // update ccb
    p_ccb->cycstat = CS_RUN;
    if ((p_ccb->cycatr & TA_PHS) == TA_PHS) {
        p_ccb->cycstat |= CS_PHS;
    }
    p_ccb->lefttmo = p_ccb->cyctime;
    p_ccb->leftphs = p_ccb->cycphs;
    
    // put cyc que
    put_cyc_que(p_ccb);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  sig cycle handler 
 */
ER sig_cyc(void)
{
    que_t *p_elm = cyc_que.p_next;
    ccb_t *p_ccb;

    // loop all elements
    while (&cyc_que != p_elm) {
        // get ccb from p_elm
        p_ccb = get_ccb_from_ccy_que(p_elm);
        // check cyc state 
        if ((p_ccb->cycstat & CS_PHS) == CS_PHS) {
            if (p_ccb->leftphs != 0) {
                // decriment left phs time 
                p_ccb->leftphs--;
            } else {
                // drop phs flag
                p_ccb->cycstat &= ~CS_PHS;
            }
        } else {
            if (p_ccb->lefttmo != 0) {
                // decriment left time
                p_ccb->lefttmo--;
            } else {
                p_ccb->cychdr();
                p_ccb->lefttmo = p_ccb->cyctime;
            }
        }
        p_elm = p_elm->p_next;
    }

    return E_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  init cycle handler 
 */
void kernel_cyc_init(void)
{
    for (int i = 0; i < CYC_HDR_NUM; i++) {
        memset(&ccb[i], 0, sizeof(ccb_t));
        ccb[i].cycstat = CS_NOEXT;
        que_init(&ccb[i].cyc_que);
    }
    que_init(&cyc_que);
}
/*--------------------------------------------------------------------------------------*/
/*! @brief  create cycle handler  
 */
ER cre_cyc(ID cycid, T_CCYC *p_ccyc)
{
    ccb_t *p_ccb;
    ER ercd = E_OK;

    // parameter check
    if (cycid >= CYC_HDR_NUM) {
        return E_PAR;
    }
    if (p_ccyc == NULL) {
        return E_PAR;
    }
    if (p_ccyc->cychdr == NULL) {
        return E_PAR;
    }
    
    // start critical section 
    critical_section_start();

    // get ccb
    p_ccb = &ccb[cycid];
    // check ccb state
    if (p_ccb->cycstat != CS_NOEXT) {
        ercd = E_OBJ; 
        goto EXIT_CRITICAL_SECTION;
    }
    // init ccyc
    init_ccb(cycid, p_ccb, p_ccyc);
    // ready cyc handler
    if ((p_ccb->cycatr & TA_STA) == TA_STA) {
        rdy_cyc_que(p_ccb);
    } else {
        p_ccb->cycstat = CS_STP; 
    }

EXIT_CRITICAL_SECTION:
    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  start cycle handler  
 */
ER sta_cyc(ID cycid)
{
    ccb_t *p_ccb;
    ER ercd = E_OK;

    // parameter check
    if (cycid >= CYC_HDR_NUM) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get ccb from cycid
    p_ccb = &ccb[cycid];
    // check ccb state
    if (p_ccb->cycstat == CS_NOEXT) {
        ercd = E_OBJ;
    } else if (p_ccb->cycstat == CS_RUN) {
        // nope
    } else if (p_ccb->cycstat == CS_STP) {
        rdy_cyc_que(p_ccb);
    } else {
        // nope
    }

    // end critical section
    critical_section_end();

    return ercd;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  stop cycle handler 
 */
ER stp_cyc(ID cycid)
{
    ccb_t *p_ccb;

    // parameter check
    if (cycid >= CYC_HDR_NUM) {
        return E_PAR;
    }

    // start critical section
    critical_section_start();

    // get ccb  from cycid
    p_ccb = &ccb[cycid];
    // check cyc state
    if ((p_ccb->cycstat & CS_RUN) != CS_RUN) {
        // nope
    } else {
        p_ccb->cycstat = CS_STP;
        deque(&p_ccb->cyc_que);
    }

    // end critical section
    critical_section_end();

    return E_OK;
}

