/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   utility.h
 *  @date   2025.05.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include <stdint.h>

 /*--------------------------------------------------------------------------------------*/
 /*! @brief operate register/memory
  */
 #define set_word(addr, val)    (*((volatile uint32_t *)(addr)) = (uint32_t)val)
 #define set_hword(addr, val)   (*((volatile uint16_t *)(addr)) = (uint16_t)val)
 #define set_byte(addr, val)    (*((volatile uint8_t  *)(addr)) = (uint8_t) val)
 #define get_word(addr)         (*((volatile uint32_t *)(addr)))
 #define get_hword(addr)        (*((volatile uint16_t *)(addr)))
 #define get_byte(addr)         (*((volatile uint8_t  *)(addr)))

/*--------------------------------------------------------------------------------------*/
/*! @brief memory align 
 */
#define mem_align(s, n)     ((s + (n-1)) & ~(n-1))

/*--------------------------------------------------------------------------------------*/
/*! @brief wait 
 */
extern void busy_wait(uint32_t cnt);

/*--------------------------------------------------------------------------------------*/
/*! @brief interrupt operation 
 */
#define enable_int()    __asm__ volatile ("CPSIE I" ::: "memory")
#define disable_int()   __asm__ volatile ("CPSID I" ::: "memory")
extern uint32_t get_int_stat(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  critical section 
 */
#define critical_section_start()    { unsigned int __primask__ = get_int_stat(); disable_int()
#define critical_section_end()      if (!__primask__) { enable_int(); }}

/*--------------------------------------------------------------------------------------*/
/*! @bjief field operation 
 */
#define set_field(type, reg, mask, val)     reg=((type)((reg)&(~(mask))))|((type)((val)<<get_sft_num(mask)))
#define clr_field(type, reg, mask)          (reg=((type)((reg)&(~(mask))))) 
#define get_field(type, reg, mask)          ((type)((reg)&(~(mask)))>>get_sft_num(mask))

#define get_sft_num(mask)                   (((mask)&(1U<<0))?0:\
                                            ((mask)&(1U<<1))?1:\
                                            ((mask)&(1U<<2))?2:\
                                            ((mask)&(1U<<3))?3:\
                                            ((mask)&(1U<<4))?4:\
                                            ((mask)&(1U<<5))?5:\
                                            ((mask)&(1U<<6))?6:\
                                            ((mask)&(1U<<7))?7:\
                                            ((mask)&(1U<<8))?8:\
                                            ((mask)&(1U<<9))?9:\
                                            ((mask)&(1U<<10))?10:\
                                            ((mask)&(1U<<11))?11:\
                                            ((mask)&(1U<<12))?12:\
                                            ((mask)&(1U<<13))?13:\
                                            ((mask)&(1U<<14))?14:\
                                            ((mask)&(1U<<15))?15:\
                                            ((mask)&(1U<<16))?16:\
                                            ((mask)&(1U<<17))?17:\
                                            ((mask)&(1U<<18))?18:\
                                            ((mask)&(1U<<19))?19:\
                                            ((mask)&(1U<<20))?20:\
                                            ((mask)&(1U<<21))?21:\
                                            ((mask)&(1U<<22))?22:\
                                            ((mask)&(1U<<23))?23:\
                                            ((mask)&(1U<<24))?24:\
                                            ((mask)&(1U<<25))?25:\
                                            ((mask)&(1U<<26))?26:\
                                            ((mask)&(1U<<27))?27:\
                                            ((mask)&(1U<<28))?28:\
                                            ((mask)&(1U<<29))?29:\
                                            ((mask)&(1U<<30))?30:\
                                            ((mask)&(1U<<31))?31:31)

