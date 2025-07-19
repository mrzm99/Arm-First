/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   utility.s 
 *  @date   2025.05.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
    .syntax unified
    .cpu cortex-m4
    .thumb

    .global busy_wait
    .global int_ena

/*--------------------------------------------------------------------------------------*/
/*! @brief  wait routine
 *  @param (R0) wait clock cycle num
 */
    .type busy_wait, %function
busy_wait:
    subs    r0, #1
    bne     busy_wait
    bx      lr

/*--------------------------------------------------------------------------------------*/
/*! @brief  enable interrupt
 */
    .type int_ena, %function
int_ena:
    CPSIE   i
    bx      lr

/*--------------------------------------------------------------------------------------*/
/*! @brief disable interrupt 
 */
    .type int_dis, %function
int_dis:
    CPSID   i
    bx      lr
