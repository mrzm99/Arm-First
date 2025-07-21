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
    systick_init();
    mem_init();

    int_ena();
    //port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
    systick_init();
    systick_start(SYSTICK_CLK_AHB_DIV_8, AHB_CLOCK_FREQ/8);

    uint32_t *p_test = (uint32_t*)mem_alloc(sizeof(uint32_t) * 10);
    for (int i = 0; i < 10; i++) {
        *p_test = 0xA5A5A5A5;
        p_test++;
    }
    p_test = (uint32_t*)mem_alloc(sizeof(uint32_t) * 50);
    for (int i = 0; i < 50; i++) {
        *p_test = 0x5A5A5A5A;
        p_test++;
    }
    uint8_t *p_test2 = (uint8_t*)mem_alloc(sizeof(uint8_t) * 100);
    for (int i = 0; i < 10; i++) {
        *p_test2 = 0xF0;
        p_test2++;
    }

    // jump to application main
    app_main();

    // never reached
    while (1);
}
