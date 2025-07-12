/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   iodefine.h
 *  @date   2025.05.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __IO_DEFINE_H__
#define __IO_DEFINE_H__

/*--------------------------------------------------------------------------------------*/
/*! @brief  register base address
 */
#define SCB                 (0xE000ED00U)
#define FLASH_INTERFACE     (0x40023C00U)
#define RCC                 (0x40023800U)
#define GPIOA               (0x40020000U)
#define GPIOB               (0x40020400U)

/*--------------------------------------------------------------------------------------*/
/*! @brief  register offset of SCB (System Contorol Block)
 */

 #define VTOR                   (0x08U)

/*--------------------------------------------------------------------------------------*/
/*! @brief  register offset of FLASH INTERFACE
 */
#define FLASH_ACR               (0x00U)


/*--------------------------------------------------------------------------------------*/
/*! @brief  register offset of RCC
 */
#define RCC_CR                  (0x00U)
#define RCC_PLLCFGR             (0x04U)
#define RCC_CFGR                (0x08U)
#define RCC_CIR                 (0x0CU)
#define RCC_AHB1RSTR            (0x10U)
#define RCC_AHB2RSTR            (0x14U)
#define RCC_AHB3RSTR            (0x18U)
#define RCC_APB1RSTR            (0x20U)
#define RCC_APB2RSTR            (0x24U)
#define RCC_AHB1ENR             (0x30U)
#define RCC_AHB2ENR             (0x34U)
#define RCC_AHB3ENR             (0x38U)
#define RCC_APB1ENR             (0x40U)
#define RCC_APB2ENR             (0x44U)
#define RCC_AHB1LPENR           (0x50U)
#define RCC_AHB2LPENR           (0x54U)
#define RCC_AHB3LPENR           (0x58U)
#define RCC_APB1LPENR           (0x60U)
#define RCC_APB2LPENR           (0x64U)
#define RCC_BDCR                (0x70U)
#define RCC_CSR                 (0x74U)
#define RCC_SSCGR               (0x80U)
#define RCC_PLLI2SCFGR          (0x84U)

/*--------------------------------------------------------------------------------------*/
/*! @brief GPIO offset  
 */
#define GPIOx_MODER             (0x00U)
#define GPIOx_OTYEPR            (0x04U)
#define GPIOx_OSPEEDR           (0x08U)
#define GPIOx_PUPDR             (0x0CU)
#define GPIOx_IDR               (0x10U)
#define GPIOx_ODR               (0x14U)
#define GPIOx_BSRR              (0x18U)
#define GPIOx_LCKR              (0x1CU)
#define GPIOx_AFRL              (0x20U)
#define GPIOx_AFRH              (0x24U)

#endif
