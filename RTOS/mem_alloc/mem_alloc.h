/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   mem_alloc.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief   
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include <stdint.h>

void mem_init(void);
void *mem_alloc(uint32_t size);
void mem_free(void *p_mbk);

#endif
