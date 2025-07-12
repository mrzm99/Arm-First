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
#define INTHDLR_NUM         (255)


typedef void (*INT_HDLR)(void);

void init_vecttbl(void);

#endif
