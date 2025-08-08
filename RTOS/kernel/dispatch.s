/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   dispatch.s
 *  @date   2025.05.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

    .syntax unified
    .cpu cortex-m4
    .thumb

    .global p_knl_run_tcb
    .global p_knl_top_tcb
    .global dispatch
    
    .section .text
/*--------------------------------------------------------------------------------------*/
/*! @brief  dispatch 
 *
 *   PSP frame
 *   +------+  Dispatch routine is only called by PendSV handler 
 *   |  R4  |  When PendSV exception ocuurs, xPSR/PC/LR/R12/R3-0 is stacked on PSP by hardware.
 *   |  R5  |  So, stack the rest registers for save context.
 *   |  R6  |
 *   |  R7  |
 *   |  R8  |
 *   |  R9  |
 *   |  R10 |
 *   |  R11 |  Software save above
 *   +------+  
 *   |  R0  |  Hardware save following 
 *   |  R1  |
 *   |  R2  |
 *   |  R3  |
 *   |  R12 |
 *   |  LR  |
 *   |  PC  |
 *   | xPSR |
 *   +------+
 */
    .type dispatch, %function
dispatch:
    // disable interrupt 
    cpsid   i
    // get knl_run_tcb
    ldr     r0,     =p_knl_run_tcb
    // skip save context if knl_run_tcb is NULL
    ldr     r1,     [r0]
    cmp     r1,     #0
    beq     skip_context
    // save context 
    mrs     r2,     psp
    stmdb   r2!,    {r4-r11}
    str     r2,     [r1, #16]   @ knl_run_tcb->p_stk = psp(stack top)
    // chage task state
    
skip_context:
    // get knl_top_tcb
    ldr     r4,     =p_knl_top_tcb
    // go idle loop if knl_top_tcb is NULL
    ldr     r5,     [r4]
    cmp     r5,     #0
    beq     idle_loop
    // move knl_top_tcb to knl_run_tcb
    str     r5,     [r0]
    // take out context from knl_run_tcb
    ldr     r3,     [r5, #16]   @ r3 = knl_top_tcb->p_stk
    ldmia   r3!,    {r4-r11} 
    msr     psp,    r3
    // set lr (exc_return)
    mov     r1,     0xfffffffd
    mov     lr,     r1
    // enable interrupt
    cpsie   i
    // return
    bx      lr

idle_loop:
    cpsie   i           @ enable interupt
    b       .           @ branch idel_loop
