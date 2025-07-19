/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   system_define.h
 *  @date   2025.04.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __SYSTEM_DEFINE_H__
#define __SYSTEM_DEFINE_H__

/*--------------------------------------------------------------------------------------*/
/*! @brief Clock frequency
 */
// Sytem clock 160MHz
#define SYSTEM_CLOCK_FREQ   (160)
// AHB clock 160MHZ
#define AHB_CLOCK_FREQ      (160)


/*--------------------------------------------------------------------------------------*/
/*! @brief  Interrupt pirority
 */
#define INT_PRI_MIN         (0)
#define INT_PRI_NAX         (255)
#define INT_PRI_SYSTICK     (10) 


#endif
