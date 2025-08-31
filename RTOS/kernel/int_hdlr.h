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
/*! @brief  Interrupt Register Bit No 
 */
#define INTREG_B0           (0)
#define INTREG_B1           (1)
#define INTREG_B2           (2)
#define INTREG_B3           (3)
#define INTREG_B4           (4)
#define INTREG_B5           (5)
#define INTREG_B6           (6)
#define INTREG_B7           (7)
#define INTREG_B8           (8)
#define INTREG_B9           (9)
#define INTREG_B10          (10)
// ...
#define INTREG_B31          (31)

/*--------------------------------------------------------------------------------------*/
/*! @brief  Interrupt Handler Number 
 */
#define INTHDLR_NO_STACK    (  0)
#define INTHDLR_NO_RESET    (  1)
#define INTHDLR_NO_SVC      ( 11)
#define INTHDLR_NO_PENDSV   ( 14)
#define INTHDLR_NO_SYSTICK  ( 15)
#define INTHDLR_NO_USART2   ( 54) 
#define INTHDLR_NO_MAX      (255)

/*--------------------------------------------------------------------------------------*/
/*! @brief  Interrupt Priority Number   
 */
#define INTPRI_NO_USART2    ( 38)
#define INTPRI_NO_MAX       (240)

/*--------------------------------------------------------------------------------------*/
/*! @brief  Interrupt Priority
 *          Upper 4bit can be configured. Write Lower 4bit is ignored.
 */
#define INTPRI_MIN          (0xF0)
#define INTPRI_MAX          (0x00)

/*--------------------------------------------------------------------------------------*/
/*! @brief  Interrupt Enable Number
 */
#define INTENA_OFS_ISER0        (0)
#define INTENA_OFS_ISER1        (1)
#define INTENA_OFS_ISER2        (2)
#define INTENA_OFS_ISER3        (3)
#define INTENA_OFS_ISER4        (4)
#define INTENA_OFS_ISER5        (5)
#define INTENA_OFS_ISER6        (6)
#define INTENA_OFS_ISER7        (7)
#define INTENA_OFS_MAX          (8)

#define INTENA_NO_USART2        ((INTENA_OFS_ISER1<<8)|(INTREG_B6))

/*--------------------------------------------------------------------------------------*/
/*! @brief  Interrupt Disable Number 
 */
#define INTDIS_OFS_ICER0        (0)
#define INTDIS_OFS_ICER1        (1)
#define INTDIS_OFS_ICER2        (2)
#define INTDIS_OFS_ICER3        (3)
#define INTDIS_OFS_ICER4        (4)
#define INTDIS_OFS_ICER5        (5)
#define INTDIS_OFS_ICER6        (6)
#define INTDIS_OFS_ICER7        (7)
#define INTDIS_OFS_MAX          (8)

#define INTDIS_BNO_HOGE         (0)

#define INTDIS_NO_HOGE          ((INTDIS_OFS_ICER0<<8)|INTDIS_BNO_HOGE)

/*--------------------------------------------------------------------------------------*/
/*! @brief  interrupt handler type 
 */
typedef void (*INT_HDLR)(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  interrupt func 
 */
void kernel_int_init(void);
int32_t int_drv_set_handler(uint32_t int_hdr_no, INT_HDLR hdlr);
int32_t int_drv_set_pri(uint32_t int_pri_no, uint32_t pri); 
int32_t int_drv_ena_int(uint32_t int_ena_no);
int32_t int_drv_dis_int(uint32_t int_dis_no);

#endif
