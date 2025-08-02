/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   bootloader_main.c
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include "iodefine.h"
#include "int_hdlr.h"
#include "utility.h"
#include "error.h"
extern void reset_handlr(void);
extern uint32_t __estack;

/*--------------------------------------------------------------------------------------*/
/*! @brief  vector table on RAM
 */
static INT_HDLR __attribute__ ((section(".vect_tbl_on_ram"))) vect_table[INTHDLR_NO_MAX];

/*--------------------------------------------------------------------------------------*/
/*! @brief  default handler
 */
static void default_hdlr(void)
{
    while (1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  init vector table
 */
void init_vecttbl(void)
{
    for (int i = 0; i < INTHDLR_NO_MAX; i++) {
        vect_table[i] = default_hdlr;
    }

    // set reset_handler and initial stack pointer
    vect_table[INTHDLR_NO_STACK] = (void(*)(void))(&__estack);
    vect_table[INTHDLR_NO_RESET] = reset_handlr;

    // set vector table address to VTOR
    set_word(SCB + VTOR, vect_table);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  set handler 
 */
int32_t set_handler(uint32_t exp_no, INT_HDLR hdlr) 
{
    // parameter check
    if (exp_no >= INTHDLR_NO_MAX) {
        return ERR_PAR;
    }
    if (hdlr == NULL) {
        return ERR_PAR;
    }

    // set handler
    vect_table[exp_no] = hdlr;

    return ERR_OK;
}
