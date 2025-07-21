/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   app_main.c
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "error.h"
#include "port_drv.h"
#include "utility.h"
#include "systick.h"
#include "system_define.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);
    while (1); 
}
