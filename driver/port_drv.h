/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   port_drv.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __PORT_DRV_H__
#define __PORT_DRV_H__

/*--------------------------------------------------------------------------------------*/
/*! @brief port no 
 */
#define PORTA           (0)
#define PORTB           (1)
#define PORTC           (2)
#define PORTH           (3)
#define PORT_NO_MAX     (4)

/*--------------------------------------------------------------------------------------*/
/*! @brief bit no 
 */
#define BIT0            (0)
#define BIT1            (1)
#define BIT2            (2)
#define BIT3            (3)
#define BIT4            (4)
#define BIT5            (5)
#define BIT6            (6)
#define BIT7            (7)
#define BIT8            (8)
#define BIT9            (9)
#define BIT10           (10)
#define BIT11           (11)
#define BIT12           (12)
#define BIT13           (13)
#define BIT14           (14)
#define BIT15           (15)
#define BIT_NO_MAX      (16)

/*--------------------------------------------------------------------------------------*/
/*! @brief pin no 
 */
#define get_pno(p)      (((p)>>8)&(0xFF))
#define get_bno(p)      ((p)&(0x0FF)) 

#define PORTA2          ((PORTA<<8)|(BIT2))
#define PORTA3          ((PORTA<<8)|(BIT3))
#define PORTA15         ((PORTA<<8)|(BIT15))
#define PORTB4          ((PORTB<<8)|(BIT4))

/*--------------------------------------------------------------------------------------*/
/*! @brief mode 
 */
#define PORT_INPUT      (0)
#define PORT_OUTPUT     (1)
#define PORT_FUNC       (2)
#define PORT_ANALOG     (3)
#define PORT_MODE_MAX   (4)

/*--------------------------------------------------------------------------------------*/
/*! @brief alternate function 
 */
#define get_mode(f)         (((f)>>8)&(0xFF))
#define get_func(f)         ((f)&(0xFF))

#define PORTA2_OUTPUT       ((PORT_OUTPUT<<8)|(0x00))
#define PORTA3_OUTPUT       ((PORT_OUTPUT<<8)|(0x00))

#define PORTA15_INPUT       ((PORT_INPUT<<8)|(0x00))
#define PORTA15_OUTPUT      ((PORT_OUTPUT<<8)|(0x00))
#define PORTA15_ANALOG      ((PORT_ANALOG<<8)|(0x00))

#define PORTB4_INPUT        ((PORT_INPUT<<8)|(0x00))
#define PORTB4_OUTPUT       ((PORT_OUTPUT<<8)|(0x00))
#define PORTB4_ANALOG       ((PORT_ANALOG<<8)|(0x00))


/*--------------------------------------------------------------------------------------*/
/*! @brief lvl
 */
#define PORT_LVL_LOW    (0)
#define PORT_LVL_HIGH   (1)
#define PORT_LVL_MAX    (2)

/*--------------------------------------------------------------------------------------*/
/*! @brief prototype  
 */
#include <stdint.h>
void port_drv_init(void);
int32_t port_drv_set_pin_func(uint32_t port_no, uint32_t mode, uint32_t init_val, uint32_t otype, uint32_t speed, uint32_t pull_up_down);
int32_t port_drv_set_pin_lvl(uint32_t port_no, uint32_t lvl);
int32_t port_drv_get_pin_lvl(uint32_t port_no);
int32_t port_drv_set_pin_lvl_no_rmw(uint32_t port_no, uint32_t lvl);


#endif
