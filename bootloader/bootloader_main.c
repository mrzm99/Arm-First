/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   bootloader_main.c
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include <stdint.h>

extern uint32_t __etext;    // end of text section
extern uint32_t __sdata;    // top of data section
extern uint32_t __edata;    // end of data section
extern uint32_t __sbss;     // top of bss section
extern uint32_t __ebss;     // end of bss section
extern void main(void);     // application main

/*--------------------------------------------------------------------------------------*/
/*! @brief  Initialize Section
 */
static void section_init(void)
{
    char *p_top;
    char *p_end;
    char *p_val;

    // clear bss section to 0
    p_top = (char *)__sbss;
    p_end = (char *)__ebss;
    while (p_top < p_end) {
        *p_top = 0;
        p_top++;
    }

    // initialize data section
    p_top = (char *)__sdata;
    p_end = (char *)__edata;
    p_val = (char *)__etext;
    while (p_top < p_end) {
        *p_top = *p_val;
        p_top++;
        p_val++;
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  Initialize system
 */
void system_init(void)
{
    // init section
    section_init();

    // クロック初期化

    // jump to application main
    main();
}