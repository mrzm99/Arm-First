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
 #define set_word(base, offset, val)    (*((volatile uint32_t *)(base + offset)) = (uint32_t)val)
 #define set_hword(base, offset, val)   (*((volatile uint16_t *)(base + offset)) = (uint16_t)val)
 #define set_byte(base, offset, val)    (*((volatile uint8_t *)(base + offset)) = (uint8_t)val)
 #define get_word(base, offset)         (*((volatile uint32_t *)(base + offset)))
 #define get_hword(base, offset)        (*((volatile uint16_t *)(base + offset)))
 #define get_byte(base, offset)         (*((volatile uint8_t *)(base + offseet)))

 #define set_bit(base, offset, bit)     (*((volatile uint32_t *)(base + offset)) |= (1<<(bit)))
 #define clr_bit(base, offset, bit)     (*((volatile uint32_t *)(base + offset)) &= (~(1<<bit)))

extern void busy_wait(uint32_t cnt);
