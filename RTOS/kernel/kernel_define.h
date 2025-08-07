/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   kernel_define.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __KERNEL_DEFINE_H__
#define __KERNEL_DEFINE_H__

#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  data type
 */
#define B       int8_t
#define H       int16_t
#define W       int32_t
#define UB      uint8_t
#define UH      uint16_t
#define UW      uint32_t
#define VP      void*
#define UINT    unsigned int
#define ER      int32_t
#define ATR     uint32_t
#define STAT    uint32_t
#define PRI     uint32_t
#define SIZE    uint32_t
#define TMO     int32_t
#define VP_INT  void*
#define ER_BOOl int32_t
#define ER_ID   int32_t
#define ER_UINT uint32_t
#define ID      int32_t

/*--------------------------------------------------------------------------------------*/
/*! @brief  start address
 */
typedef void(*FP)(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  error code 
 */
#define E_OK        (  0)
#define E_SYS       ( -5)
#define E_PAR       (-17)
#define E_ID        (-18)
#define E_CTX       (-25)
#define E_NOMEM     (-33)
#define E_NOID      (-34)
#define E_OBJ       (-41)
#define E_NOEXS     (-42)
#define E_QOVR      (-43)
#define E_RLWAI     (-49)
#define E_TMOUT     (-50)
#define E_DLT       (-51)
#define E_BOVR      (-58)

#endif
