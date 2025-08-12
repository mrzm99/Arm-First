/*--------------------------------------------------------------------------------------*/
/*!
 *      @file   mem_poolf.h 
 *      @date   2025.xx.xx
 *      @author mrzm99
 *      @brief  memory pool (fix)
 *      @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __MEM_POOLF_H__
#define __MEM_POOLF_H__

#include "kernel_config.h"
#include "kernel_define.h"
#include "que.h"

/*--------------------------------------------------------------------------------------*/
/*! @brief  fix memory pool management block
 */
typedef struct {
    que_t       mpf_que;        // memory pool wait que
    que_t       free_blk_que;   // free block que
    uint8_t     *top_addr;      // top address
    uint8_t     *end_addr;      // end address

    ATR         mpfatr;         // memory pool attribute
    UINT        blkcnt;         // memory block num
    UINT        blksz;          // memory block size (byte)
    VP          mpf;            // top address of memory pool
} mpfcb_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief  T_CMPF
 */
typedef struct {
    ATR     mpfatr;     // fix memory pool attribute
    UINT    blkcnt;     // memory block num
    UINT    blksz;      // memory block size (byte)
    VP      mpf;        // top address of memory pool
} T_CMPF;

/*--------------------------------------------------------------------------------------*/
/*! @brief  Kernel func
 */
void kernel_mempoolf_init(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  Service Calll
 */
ER cre_mpf(ID mpfid, T_CMPF *p_cmpf);
ER del_mpf(ID mpfid);
ER get_mpf(ID mpfid, VP *p_blk, TMO tmout);
ER rel_mpf(ID mpfid, VP *p_blk);

#endif
