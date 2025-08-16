/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   bootloader_main.c
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include <error.h>
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
/*! @brief  register base address
 */
typedef struct {
    uint32_t iser;      // interrupt set-enable register base
    uint32_t icer;      // interrupt clear-enable register base
    uint32_t ipr;       // interrupt priority register base
} reg_base_t;

static const reg_base_t reg_base_addr = {
    NVIC_ISER0,
    NVIC_ICER0,
    NVIC_IPR_BASE,
};

/*--------------------------------------------------------------------------------------*/
/*! @brief  macro
 */
#define get_pri_ofs(no)         (no)    // priority number == register offset

#define get_ena_ofs(no)         (((no)>>8)&0xFF)
#define get_ena_bno(no)         ((no)&0xFF)

#define get_dis_ofs(no)         (((no)>>8)&0xFF)
#define get_dis_bno(no)         ((no)&0xFF)

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
void kernel_int_init(void)
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
int32_t int_drv_set_handler(uint32_t int_hdr_no, INT_HDLR hdlr) 
{
    // parameter check
    if (int_hdr_no>= INTHDLR_NO_MAX) {
        return ERR_PAR;
    }
    if (hdlr == NULL) {
        return ERR_PAR;
    }

    // set handler
    vect_table[int_hdr_no] = hdlr;

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  set priority 
 */
int32_t int_drv_set_pri(uint32_t int_pri_no, uint32_t pri)
{
    uint32_t ofs = get_pri_ofs(int_pri_no);
    uint32_t base_reg_addr = reg_base_addr.ipr;

    // parameter check
    if (ofs >= INTPRI_NO_MAX) {
        return ERR_PAR;
    }

    // set priority
    set_byte(base_reg_addr + ofs, pri);     

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  enable interrupt 
 */
int32_t int_drv_ena_int(uint32_t int_ena_no)
{
    uint32_t ofs = 4 * get_ena_ofs(int_ena_no);
    uint32_t bno = get_ena_bno(int_ena_no);
    uint32_t base_reg_addr = reg_base_addr.iser;
    uint32_t set_val;

    // parameter check
    if (ofs >= INTENA_OFS_MAX) {
        return ERR_PAR;
    } else if (bno >= 32) {
        return ERR_PAR;
    }

    // enable interrupt
    set_val = get_word(base_reg_addr + ofs);
    set_field(uint32_t, set_val, 1<<bno, 1);
    set_word(base_reg_addr + ofs, set_val);

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  disable interrupt
 */
int32_t int_drv_dis_int(uint32_t int_dis_no)
{
    uint32_t ofs = 4 * get_dis_ofs(int_dis_no);
    uint32_t bno = get_dis_bno(int_dis_no);
    uint32_t base_reg_addr = reg_base_addr.icer;
    uint32_t set_val;

    // parameter check
    if (ofs >= INTDIS_OFS_MAX) {
        return ERR_PAR;
    } else if (bno >= 32) {
        return ERR_PAR;
    }

    // disable interrupt
    set_val = get_word(base_reg_addr + ofs);
    set_field(uint32_t, set_val, 1<<bno, 1);
    set_word(base_reg_addr + ofs, set_val);

    return ERR_OK;
}
