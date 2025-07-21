/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   kernel.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief   
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "kernel.h"
#include "iodefine.h"
#include "systick.h"
#include "utility.h"
#include "port_drv.h"
#include "system_define.h"

extern void app_main(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
static void nvic_init(void)
{

}

/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
void kernel_init(void)
{
    systick_init();

    int_ena();
    //port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
    systick_init();
    systick_start(SYSTICK_CLK_AHB_DIV_8, AHB_CLOCK_FREQ/8);

    // jump to application main
    app_main();

    // never reached
    while (1);
}
