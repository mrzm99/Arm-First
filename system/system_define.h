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
/*! @brief Clock frequency  [MHz]
 */
#define SYSTEM_CLOCK_FREQ   (160)   //!< SYSMTE clock
#define AHB_CLOCK_FREQ      (160)   //!< AHB clock
#define APB1_CLOCK_FREQ     (40)    //!< APB1 clock


/*--------------------------------------------------------------------------------------*/
/*! @brief  Interrupt pirority
 */
#define INT_PRI_SYSTEM_EXCEPTION_SFT_NUM    (4)
#define INT_PRI_MAX                         (0)
#define INT_PRI_MIN                         (255)
#define INT_PRI_SYSTICK                     (1<<INT_PRI_SYSTEM_EXCEPTION_SFT_NUM) 
#define INT_PRI_PENDSV                      (10<<INT_PRI_SYSTEM_EXCEPTION_SFT_NUM)
#define INT_PRI_I2C_CH1                     (2<<INT_PRI_SYSTEM_EXCEPTION_SFT_NUM)


#endif
