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

#define INTHDLR_NO_STACK    (0)
#define INTHDLR_NO_RESET    (1)
#define INTHDLR_NO_SYSTICK  (15)
#define INTHDLR_NO_MAX      (255)

#include <stdint.h>

typedef void (*INT_HDLR)(void);

void init_vecttbl(void);
int32_t set_handler(uint32_t exp_no, INT_HDLR hdlr);

#endif
