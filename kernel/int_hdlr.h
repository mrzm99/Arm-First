/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   bootloader_main.c
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief  interrupt handler
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#define INTHDLR_NO_STACK    (0)
#define INTHDLR_NO_RESET    (1)
#define INTHDLR_NUM         (255)


typedef void (*INT_HDLR)(void);

void init_vecttbl(void);