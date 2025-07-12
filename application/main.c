/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   main.c
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "error.h"
#include "port_drv.h"
#include "utility.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void main(void)
{
    while (1) {
        port_drv_set_pin(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
        busy_wait(40*1000*1000);
        port_drv_set_pin(PORTB4, PORTB4_OUTPUT, PORT_LVL_LOW, 0, 0, 0);
        busy_wait(40*1000*1000);
    }
}
