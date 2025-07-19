/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   port_drv.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include <stdint.h>
#include "port_drv.h"
#include "utility.h"
#include "iodefine.h"
#include "error.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief base register  
 */
static const uint32_t base_reg_addr[PORT_NO_MAX] = {
    GPIOA,
    GPIOB,
    0,
    0,
};



/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
void port_drv_init(void)
{
    // init port
}

/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
int32_t port_drv_set_pin_func(uint32_t port_no, uint32_t mode, uint32_t init_val, uint32_t otype, uint32_t speed, uint32_t pull_up_down)
{
    uint32_t pno = get_pno(port_no);
    uint32_t bno = get_bno(port_no);
    uint32_t pmode = get_mode(mode);
    uint32_t pfunc = get_func(mode);

    // parameter check
    if (pno >= PORT_NO_MAX) {
        return ERR_PAR;
    }
    if (bno >= BIT_NO_MAX) {
        return ERR_PAR;
    }
    if (pmode >= PORT_MODE_MAX) {
        return ERR_PAR;
    }
    if (pfunc >= 0x0F) {
        return ERR_PAR;
    }

    // set port type
    if (pmode == PORT_INPUT) {
        // set port mode
        set_word(base_reg_addr[pno], GPIOx_MODER, PORT_INPUT);
    } else if (pmode == PORT_OUTPUT) {
        // set initial pin level
        set_hword(base_reg_addr[pno], GPIOx_ODR, init_val<<bno);
        // set port mode 
        set_word(base_reg_addr[pno], GPIOx_MODER, PORT_OUTPUT<<(bno*2));
        // set push-pull or open-drain
        
        // set pull-up or pull-down

        // set normal speed or high speed

    } else if (pmode == PORT_ANALOG) {
        // set port mode
        set_word(base_reg_addr[pno], GPIOx_MODER, PORT_ANALOG);
    } else if (pmode == PORT_FUNC) {
        // set port mode
        set_word(base_reg_addr[pno], GPIOx_MODER, PORT_FUNC);
        // set function
        if (bno >= 8) {
            set_word(base_reg_addr[pno], GPIOx_AFRH, pfunc<<(bno*4));
        } else {
            set_word(base_reg_addr[pno], GPIOx_AFRL, pfunc<<(bno*4));
        }
    } else {
        // nope
    }

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
int32_t port_drv_set_pin_lvl(uint32_t port_no, uint32_t lvl)
{
    uint32_t pno = get_pno(port_no);
    uint32_t bno = get_bno(port_no);

    // parameter check
    if (pno >= PORT_NO_MAX) {
        return ERR_PAR;
    } 
    if (bno >= BIT_NO_MAX) {
        return ERR_PAR;
    }

    // set port output level
    set_hword(base_reg_addr[pno], GPIOx_ODR, lvl<<bno);

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
int32_t port_drv_get_pin_lvl(uint32_t port_no)
{
    uint32_t pno = get_pno(port_no);
    uint32_t bno = get_bno(port_no);

    // parameter check
    if (pno >= PORT_NO_MAX) {
        return ERR_PAR;
    }
    if (bno > BIT_NO_MAX) {
        return ERR_PAR;
    } 

    // get port input level
    return (((get_hword(base_reg_addr[pno], GPIOx_IDR))>>bno) & 1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
int32_t port_drv_set_pin_lvl_no_rmw(uint32_t port_no, uint32_t lvl)
{
    // No read-modify-write access
}
