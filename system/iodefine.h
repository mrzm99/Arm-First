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
#define STK_CTRL            (0xE000E010U)
#define STK_LOAD            (0xE000E014U)
#define STK_VAL             (0xE000E018U)
#define STK_CALIB           (0xE000E01CU)
#define FLASH_INTERFACE     (0x40023C00U)
#define RCC                 (0x40023800U)
#define GPIOA               (0x40020000U)
#define GPIOB               (0x40020400U)
#define SHPR1               (0xE000ED18U)
#define SHPR2               (0xE000ED1CU)
#define SHPR3               (0xE000ED20U)

/*--------------------------------------------------------------------------------------*/
/*! @brief  register offset of SCB (System Contorol Block)
 */
#define ICSR                    (0x04U)
#define VTOR                    (0x08U)

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
/*! @brief NVIC 
 */
#define NVIC_ISER0              (0xE000E100U)
#define NVIC_ISER1              (0xE000E104U)
#define NVIC_ISER2              (0xE000E108U)
#define NVIC_ISER3              (0xE000E10CU)
#define NVIC_ISER4              (0xE000E110U)
#define NVIC_ISER5              (0xE000E114U)
#define NVIC_ISER6              (0xE000E118U)
#define NVIC_ISER7              (0xE000E11CU)

#define NVIC_ICER0              (0xE000E180U)
#define NVIC_ICER1              (0xE000E184U)
#define NVIC_ICER2              (0xE000E188U)
#define NVIC_ICER3              (0xE000E18CU)
#define NVIC_ICER4              (0xE000E190U)
#define NVIC_ICER5              (0xE000E194U)
#define NVIC_ICER6              (0xE000E198U)
#define NVIC_ICER7              (0xE000E19CU)

#define NVIC_ISPR0              (0xE000E200U)
#define NVIC_ISPR1              (0xE000E204U)
#define NVIC_ISPR2              (0xE000E208U)
#define NVIC_ISPR3              (0xE000E20CU)
#define NVIC_ISPR4              (0xE000E210U)
#define NVIC_ISPR5              (0xE000E214U)
#define NVIC_ISPR6              (0xE000E218U)
#define NVIC_ISPR7              (0xE000E21CU)

#define NVIC_ICPR0              (0xE000E280U)
#define NVIC_ICPR1              (0xE000E284U)
#define NVIC_ICPR2              (0xE000E288U)
#define NVIC_ICPR3              (0xE000E28CU)
#define NVIC_ICPR4              (0xE000E290U)
#define NVIC_ICPR5              (0xE000E294U)
#define NVIC_ICRP6              (0xE000E298U)
#define NVIC_ICRP7              (0xE000E29CU)

#define NVIC_IABP0              (0xE000E300U)
#define NVIC_IABP1              (0xE000E304U)
#define NVIC_IABP2              (0xE000E308U)
#define NVIC_IABP3              (0xE000E30CU)
#define NVIC_IABP4              (0xE000E310U)
#define NVIC_IABP5              (0xE000E314U)
#define NVIC_IABP6              (0xE000E318U)
#define NVIC_IABP7              (0xE000E31CU)

#define NVIC_IPR_BASE           (0xE000E400U)

#define NVIC_STIR               (0xE000EF00U)


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

/*--------------------------------------------------------------------------------------*/
/*! @brief USART
 */
#define USART_OFS_SR            (0x00)
#define USART_OFS_DR            (0x04)
#define USART_OFS_BRR           (0x08)
#define USART_OFS_CR1           (0x0C)
#define USART_OFS_CR2           (0x10)
#define USART_OFS_CR3           (0x14)
#define USART_OFS_GTPR          (0x18)

#define USART2_BASE             (0x40004400)
#define USART2_SR               (USART2_BASE+USART_OFS_SR)
#define USART2_DR               (USART2_BASE+USART_OFS_DR)
#define USART2_BRR              (USART2_BASE+USART_OFS_BRR)
#define USART2_CR1              (USART2_BASE+USART_OFS_CR1)
#define USART2_CR2              (USART2_BASE+USART_OFS_CR2)
#define USART2_CR3              (USART2_BASE+USART_OFS_CR3)
#define USART2_GTPR             (USART2_BASE+USART_OFS_GTPR)

/*--------------------------------------------------------------------------------------*/
/*! @brief I2C
 */
#define I2C_OFS_CR1             (0x00)
#define I2C_OFS_CR2             (0x04)
#define I2C_OFS_OAR1            (0x08)
#define I2C_OFS_OAR2            (0x0C)
#define I2C_OFS_DR              (0x10)
#define I2C_OFS_SR1             (0x14)
#define I2C_OFS_SR2             (0x18)
#define I2C_OFS_CCR             (0x1C)
#define I2C_OFS_TRISE           (0x20)
#define I2C_OFS_FLTR            (0x24)

#define I2C1_BASE               (0x40005400)
#define I2C1_CR1                (I2C1_BASE+I2C_OFS_CR1)
#define I2C1_CR2                (I2C1_BASE+I2C_OFS_CR2)
#define I2C1_OAR1               (I2C1_BASE+I2C_OFS_OAR1)
#define I2C1_OAR2               (I2C1_BASE+I2C_OFS_OAR2)
#define I2C1_DR                 (I2C1_BASE+I2C_OFS_DR)
#define I2C1_SR1                (I2C1_BASE+I2C_OFS_SR1)
#define I2C1_SR2                (I2C1_BASE+I2C_OFS_SR2)
#define I2C1_CCR                (I2C1_BASE+I2C_OFS_CCR)
#define I2C1_TRISE              (I2C1_BASE+I2C_OFS_TRISE)
#define I2C1_FLTR               (I2C1_BASE+I2C_OFS_FLTR)

#endif
