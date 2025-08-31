/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   usart.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#include "system_define.h"
#include "error.h"
#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief ch 
 */
#define USART_CH_2          (2)
#define USART_CH_MAX        (3)

/*--------------------------------------------------------------------------------------*/
/*! @brief  mode 
 */
#define USART_ASYNC         (0)     //!< asynchronous 

/*--------------------------------------------------------------------------------------*/
/*! @brief  over sampling 
 */
#define USART_OVR_SMP_16    (0)     //!< oversampling by 16 
#define USART_OVR_SMP_8     (1)     //!< oversampling by 8

/*--------------------------------------------------------------------------------------*/
/*! @brief  data length 
 */
#define USART_DATA_LEN_8    (0)     //!< start bit, 8 data bits, n stop bits
#define USART_DATA_LEN_9    (1)     //!< start bit, 9 data bits, n stop bits
 
/*--------------------------------------------------------------------------------------*/
/*! @brief  parity 
 */
#define USART_PARITY_DIS    (0)     //!< parity dissable
#define USART_PARITY_EVEN   (1)     //!< even parity
#define USART_PARITY_ODD    (2)     //!< odd parity 

/*--------------------------------------------------------------------------------------*/
/*! @brief  stop bits 
 */
#define USART_STOP_1        (0)     //!< 1 stop bit
#define USART_STOP_05       (1)     //!< 0.5 stop bit
#define USART_STOP_2        (2)     //!< 2 stop bits
#define USART_STOP_15       (3)     //!< 1.5 stop bits

/*--------------------------------------------------------------------------------------*/
/*! @brief usart config structure
 */
typedef struct {
    uint32_t mode;          //!< mode
    uint32_t ovrsmp;        //!< over samplig
    uint32_t datalen;       //!< data length
    uint32_t parity;        //!< parity
    uint32_t stop;          //!< stop bit
    uint32_t brr;           //!< buadrate
} usart_peri_config_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief callback 
 */
typedef void (*usart_send_callback)(void *p_par);
typedef void (*usart_recv_callback)(uint8_t rdata, void *p_par);

/*--------------------------------------------------------------------------------------*/
/*! @brief  API 
 */
void usart_init(void);
int32_t usart_config(uint32_t ch, usart_peri_config_t *p_config);
int32_t usart_open(uint32_t ch, uint32_t pri, usart_send_callback send_callback, usart_recv_callback recv_callback, void *p_callbak_par);
int32_t usart_send(uint32_t ch, uint8_t *p_sdata, uint32_t send_num);

#endif
