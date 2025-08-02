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
#include "mem_alloc/mem_alloc.h"

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
    // module init
    systick_init();
    mem_init();
    
    // init systick and enable interrupt
    enable_int(); 
    systick_init();
    systick_start(SYSTICK_CLK_AHB_DIV_8, AHB_CLOCK_FREQ/8);

    // jump to application main
    app_main();

    // never reached
    while (1);
}
