/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   int_hdlr.h 
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief  interrupt handler
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __INT_DHLR_H__
#define __INT_DHLR_H__

#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  Interrupt number
 */
#define INTHDLR_NO_STACK    (  0)
#define INTHDLR_NO_RESET    (  1)
#define INTHDLR_NO_SVC      ( 11)
#define INTHDLR_NO_PENDSV   ( 14)
#define INTHDLR_NO_SYSTICK  ( 15)
#define INTHDLR_NO_MAX      (255)

/*--------------------------------------------------------------------------------------*/
/*! @brief  interrupt handler type 
 */
typedef void (*INT_HDLR)(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  interrupt func 
 */
void init_vecttbl(void);
int32_t set_handler(uint32_t exp_no, INT_HDLR hdlr);

#endif
