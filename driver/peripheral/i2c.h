/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   i2c.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __I2C_H__
#define __I2C_H__

#include "system_define.h"
#include "error.h"
#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief ch
 */
#define I2C_CH_1                (0)
#define I2C_CH_MAX              (1)

/*--------------------------------------------------------------------------------------*/
/*! @brief mode
 */
#define I2C_MODE_I2C            (0)

/*--------------------------------------------------------------------------------------*/
/*! @brief analog noise filter
 */
#define I2C_ANALOG_FILTER_ON    (0)
#define I2C_ANALOG_FILTER_OFF   (1)

/*--------------------------------------------------------------------------------------*/
/*! @brief degital noise filter
 */
#define I2C_DEGITAL_FILTER_ON   (0)
#define I2C_DEGITAL_FILTER_OFF  (1)

/*--------------------------------------------------------------------------------------*/
/*! @brief i2c config structure 
 */
typedef struct {
    uint8_t     mode;       //!< i2c mode
    uint32_t    bitrate;    //!< bitrate
    uint8_t     an_filter;  //!< analog noise filter
    uint8_t     dg_filter;  //!< digital noise filter
} i2c_peri_config_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief callback
 */
typedef void (*i2c_send_callback)(void *p_par);
typedef void (*i2c_recv_callback)(void *p_par);

/*--------------------------------------------------------------------------------------*/
/*! @brief API
 */
void i2c_init(void);
int32_t i2c_config(uint32_t ch, i2c_peri_config_t *p_config);
int32_t i2c_open(uint32_t ch, uint32_t pri, i2c_send_callback send_callback, i2c_recv_callback recv_callback, void *p_callback_par);
int32_t i2c_send(uint32_t ch, uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_data, uint32_t send_num);
int32_t i2c_recv(uint32_t ch, uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_data, uint32_t recv_num);
int32_t i2c_close(uint32_t ch);

#endif

