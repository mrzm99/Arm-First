/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   mem_alloc.c 
 *  @date   2025..xx
 *  @author mrzm99
 *  @brief  
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "mem_alloc.h"
#include "utility.h"

extern void *__edata;
extern void *__ram_end;

#define get_use(p)      ((*((uint32_t*)(p))) & 0x01)
#define get_size(p)     ((*((uint32_t*)(p))) & 0xFFFFFFFCU)

#define UNUSE           (0)
#define USE             (1)
#define TAG_SIZE        (8) // top: 4byte, end: 4byte

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
typedef struct {
    uint8_t *p_top;     // memory block top
    uint8_t *p_end;     // memory block end
} memory_ctl_blk_t;

static memory_ctl_blk_t memory_ctl_blk;
#define get_ctl_blk()   (&memory_ctl_blk)

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
static void set_tag(uint8_t *p_mbk, uint32_t size, uint32_t use)
{
    uint8_t *p_top = p_mbk;
    uint8_t *p_end = p_mbk + size - 4;

    // set top tag
    *(uint32_t*)p_top = size|use;
    // set end tag
    *(uint32_t*)p_end = size|use;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
static void divide_mbk(uint8_t *p_mbk, uint32_t size)
{
    uint32_t free_size = get_size(p_mbk) - size;
    set_tag(p_mbk, size, UNUSE);
    set_tag(p_mbk+size, free_size, UNUSE);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
static void merge_memory_block(uint8_t *p_prev, uint8_t *p_next)
{
    uint32_t new_size = get_size(p_prev) + get_size(p_next);
    set_tag(p_prev, new_size, UNUSE);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
void mem_init(void)
{
    // get memory management info
    memory_ctl_blk_t *this = get_ctl_blk();

    // init memory mangement info
    this->p_top = (uint8_t*)&__edata;
    this->p_end = (uint8_t*)&__ram_end;

    // init tag
    set_tag(this->p_top, (uint32_t)&__ram_end - (uint32_t)&__edata, UNUSE);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
void *mem_alloc(uint32_t size)
{
    // get memory management info
    memory_ctl_blk_t *this = get_ctl_blk();

    // get top addr
    uint8_t *p_mbk = this->p_top;

    // align size and add tag size
    size = mem_align(size, 4);
    size += TAG_SIZE;

    while (p_mbk < this->p_end) {
        // get unused memory block
        if ((get_use(p_mbk) == UNUSE) && (get_size(p_mbk) >= size)) {
            // devide memory block
            if (get_size(p_mbk) >= (size + TAG_SIZE)) {
                divide_mbk(p_mbk, size);
            }
            // set tag
            set_tag(p_mbk, size, USE);
            // p_mbk is address for top tag, so return p_mbk + 1 
            return (void*)(p_mbk + 4);
        }
        // search next tag
        p_mbk = p_mbk + get_size(p_mbk);
    }

    return NULL;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief
 */
void mem_free(void *p_mbk)
{
    memory_ctl_blk_t *this = get_ctl_blk();
    uint8_t *p_mem = (uint8_t*)p_mbk;

    if (p_mbk == NULL) {
        return;
    }

    // get tag
    p_mem -= 4;
    // set tag
    set_tag(p_mem, get_size(p_mem), UNUSE);
    // merge if prev memory block is unused
    if (((p_mem-4) >= this->p_top) & (get_use(p_mem-4) == UNUSE)) {
        merge_memory_block(p_mem - get_size(p_mem-4), p_mem);
    } 
    // merge if next memory block is unused
    if (((p_mem + get_size(p_mem)) < this->p_end) && (get_use(p_mem + get_size(p_mem)) == UNUSE)) {
        merge_memory_block(p_mem, p_mem + get_size(p_mem));
    }
}
