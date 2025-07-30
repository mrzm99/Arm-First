/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   utility.c
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "utility.h"

 /*--------------------------------------------------------------------------------------*/
 /*! @brief 
  */
uint32_t get_int_stat(void)
{
    uint32_t primask_val;

    __asm__ volatile (
        "MRS %0, PRIMASK"
        : "=r" (primask_val)
        :
        : "memory"
    );

    return primask_val;
}
