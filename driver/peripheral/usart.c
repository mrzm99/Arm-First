/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   usart.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "usart.h"
#include "system_define.h"
#include "error.h"
#include "iodefine.h"
#include "utility.h"
#include "int_hdlr.h"
#include "port_drv.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  prototype
 */
static void usart2_global_inthdr(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief  reg info 
 */
typedef struct {
    uint32_t sr;
    uint32_t dr;
    uint32_t brr;
    uint32_t cr1;
    uint32_t cr2;
    uint32_t cr3;
    uint32_t gtpr;
} usart_reg_t;

static const usart_reg_t usart_reg_info[USART_CH_MAX] = {
    // ch0
    {},
    // ch1
    {},
    // ch2
    {
        USART2_SR,
        USART2_DR,
        USART2_BRR,
        USART2_CR1,
        USART2_CR2,
        USART2_CR3,
        USART2_GTPR,
    },
};

/*--------------------------------------------------------------------------------------*/
/*! @brief  peri info 
 */
typedef struct {
    uint32_t inthdr_no;
    INT_HDLR inthdr;
} inthdr_info_t;

typedef struct {
    inthdr_info_t inthdr_info;
    uint32_t pri_no;
    uint32_t ena_no;
    uint32_t dis_no;
    uint32_t port_no_tx;
    uint32_t port_mode_tx;
    uint32_t port_no_rx;
    uint32_t port_mode_rx;
    // additional info
} usart_peri_info_t;

static const usart_peri_info_t peri_info[USART_CH_MAX] = {
    // ch0
    {},
    // ch1
    {},
    // ch2
    { 
        {INTHDLR_NO_USART2, usart2_global_inthdr}, 
        INTPRI_NO_USART2,
        INTENA_NO_USART2,
        0,
        PORTA2,
        PORTA2_USART2_TX,
        PORTA3,
        PORTA3_USART2_RX,
    },
};

/*--------------------------------------------------------------------------------------*/
/*! @brief  usart control block 
 */
typedef struct {
    uint8_t *p_sdata;
    uint32_t send_num;
    usart_send_callback send_callback;
    usart_recv_callback recv_callback;
    void *p_callback_par;
} usart_ctrl_blk_t;

static usart_ctrl_blk_t usart_ctrl_blk[USART_CH_MAX];
#define get_myself(ch)  (&usart_ctrl_blk[(ch)])

/*--------------------------------------------------------------------------------------*/
/*! @brief  send data 
 */
static void send_data_hdlr(usart_ctrl_blk_t *this, const usart_reg_t *p_reg_info)
{
    uint32_t set_val;

    if (this->send_num != 0) {
        // send next data
        set_word(p_reg_info->dr, *this->p_sdata);
        this->p_sdata++;
        this->send_num--;
    } else {
        // callback
        if (this->send_callback != NULL) {
            this->send_callback(this->p_callback_par);
        }
        // enable send complete interrupt and disable send data empty interrupt
        set_val = get_word(p_reg_info->cr1);
        clr_field(uint32_t, set_val, (1<<7));
        set_val |= (1<<6);
        set_word(p_reg_info->cr1, set_val);
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  send data complete
 */
static void send_cmplt_hdlr(usart_ctrl_blk_t *this, const usart_reg_t *p_reg_info)
{
    uint32_t set_val;

    // stop send
    set_val = get_word(p_reg_info->cr1);
    clr_field(uint32_t, set_val, (1<<3));
    set_word(p_reg_info->cr1, set_val);

    // clear flag
    set_val = get_word(p_reg_info->sr);
    clr_field(uint32_t, set_val, (1<<6)); 
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  receive data 
 */
static void recv_hdlr(usart_ctrl_blk_t *this, const usart_reg_t *p_reg_info)
{
    uint32_t set_val;
    uint8_t rdata;
    
    // get data
    rdata = get_byte(p_reg_info->dr);
    // callback
    if (this->recv_callback != NULL) {
        // ToDo: inform error occurence 
        this->recv_callback(rdata, this->p_callback_par);
    }
    // clear flag 
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  error  
 */
static void error_hdlr(usart_ctrl_blk_t *this, usart_reg_t *p_reg_info)
{
    // erro handling
    // clear flag
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  USART Global Interrupt Handler 
 */
static void common_usart_inthdr(uint32_t ch)
{
    usart_ctrl_blk_t *this = get_myself(ch);
    const usart_reg_t *p_reg_info = &usart_reg_info[ch];
    uint32_t sr_val = get_word(p_reg_info->sr);

    // send data empty
    if (sr_val & (1<<7)) {
        send_data_hdlr(this, p_reg_info);
    }
    // send data complete
    if (sr_val & (1<<6)) {
        send_cmplt_hdlr(this, p_reg_info);
    }
    // receive
    if (sr_val & (1<<5)) {
        recv_hdlr(this, p_reg_info);
    }
    // error
//-    if (sr_val & hoge) {
//-        error_hdlr(this);
//-    } 
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  USART2 Global Interrupt Handler
 */
static void usart2_global_inthdr(void)
{
    common_usart_inthdr(USART_CH_2);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  calc brr register val 
 */
static void calc_brr_set_val(uint32_t baudrate, uint32_t *p_mantissa, uint32_t *p_fraction)
{
    uint32_t apb1_clk = APB1_CLOCK_FREQ * 1000 * 1000;

    // calc mantissa, fraction
    *p_mantissa = apb1_clk / (16 * baudrate); 
    *p_fraction = ((apb1_clk % (16 * baudrate)) * 16) / (16 * baudrate);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  set register 
 */
static void set_reg(const usart_reg_t *p_reg_info, usart_peri_config_t *p_config)
{
    uint32_t set_val;
    uint32_t brr_div_mantissa;
    uint32_t brr_div_fraction;

    // calc brr register val
    calc_brr_set_val(p_config->brr, &brr_div_mantissa, &brr_div_fraction);
    set_val = (brr_div_mantissa<<4)|(brr_div_fraction);
    set_word(p_reg_info->brr, set_val);
    // Async mode
    if (p_config->mode == USART_ASYNC) {
        // set cr1
        set_val = 0;
        if (p_config->ovrsmp == USART_OVR_SMP_16) {
            set_val |= 0<<15;
        } else {
            set_val |= 1<<15;
        }
        if (p_config->datalen == USART_DATA_LEN_8) {
            set_val |= 0<<12;
        } else {
            set_val |= 1<<12;
        }
        if (p_config->parity == USART_PARITY_DIS) {
            set_val |= 0<<12;
        } else {
            // set prity
        }
        set_word(p_reg_info->cr1, set_val);
        // set cr2
        set_val = 0;
        if (p_config->stop == USART_STOP_1) {
            set_val |= 0<<12;
        } else {
            // set stop
        }
        set_word(p_reg_info->cr2, set_val);
        // set cr3
        set_val = 0;
//-     set bits of cr3
        set_word(p_reg_info->cr3, set_val);

    // xxx mode
    } else {

    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  init 
 */
void usart_init(void)
{
    usart_ctrl_blk_t *this;

    for (uint32_t i = 0; i < USART_CH_MAX; i++) {
        this = get_myself(i);
        memset(this, 0, sizeof(usart_ctrl_blk_t));
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  send 
 */
int32_t usart_config(uint32_t ch, usart_peri_config_t *p_config)
{
    const usart_reg_t *p_reg_info;
    uint32_t set_val;

    // check parameter
    if (ch >= USART_CH_MAX) {
        return ERR_PAR;
    }
    if (p_config == NULL) {
        return ERR_PAR;
    }
    
    // start critical section
    critical_section_start();

    // get reg info 
    p_reg_info = &usart_reg_info[ch];

    // disable usart
    set_byte(p_reg_info->cr1, 0x00);    

    // enable USART
    p_reg_info = &usart_reg_info[ch];
    set_val = get_word(p_reg_info->cr1);  
    set_val |= (1<<13);
    set_word(p_reg_info->cr1, set_val);

    // set reg
    set_reg(p_reg_info, p_config);

    // set port
    port_drv_set_pin_func(peri_info[ch].port_no_tx, peri_info[ch].port_mode_tx, PORT_LVL_HIGH, 0, 0, 0);
    port_drv_set_pin_func(peri_info[ch].port_no_rx, peri_info[ch].port_mode_rx, PORT_LVL_HIGH, 0, 0, 0);

    // set interrupt handler
    int_drv_set_handler(peri_info[ch].inthdr_info.inthdr_no, peri_info[ch].inthdr_info.inthdr);

    // enable global interrupt
    int_drv_ena_int(peri_info[ch].ena_no); 

    // end critical section
    critical_section_end();

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  open 
 */
int32_t usart_open(uint32_t ch, uint32_t pri, usart_send_callback send_callback, usart_recv_callback recv_callback, void *p_callback_par)
{
    usart_ctrl_blk_t *this;
    const usart_reg_t *p_reg_info;
    uint32_t set_val;

    // check parameter
    if (ch >= USART_CH_MAX) {
        return ERR_PAR;
    }
    if ((send_callback == NULL) || (recv_callback == NULL)) {
        return ERR_PAR; 
    }
    if (p_callback_par == NULL) {
        return ERR_PAR;
    }

    // get control block
    this = get_myself(ch);

    // start critical setction
    critical_section_start();

    // set interrupt priority
    int_drv_set_pri(peri_info[ch].pri_no, pri);

    // save callback
    this->send_callback = send_callback;
    this->recv_callback = recv_callback;
    this->p_callback_par = p_callback_par;

    // start recv data
    p_reg_info = &usart_reg_info[ch];
    set_val = get_word(p_reg_info->cr1);  
    set_val |= (1<<13)|(1<<5)|(1<<2);
    set_word(p_reg_info->cr1, set_val);

    // end critical section
    critical_section_end();

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  send 
 */
int32_t usart_send(uint32_t ch, uint8_t *p_sdata, uint32_t send_num)
{
    uint32_t set_val;
    usart_ctrl_blk_t *this;
    const usart_reg_t *p_reg_info; 

    // parameter check
    if (ch >= USART_CH_MAX) {
        return ERR_PAR;
    }
    if (p_sdata == NULL) {
        return ERR_PAR;
    }
    if (send_num == 0) {
        return ERR_OK;
    }

    // start critical section
    critical_section_start();

    // get control block
    this = get_myself(ch);

    // send enable
    p_reg_info = &usart_reg_info[ch];
    set_val = get_word(p_reg_info->cr1);
    set_val |= (1<<7)|(1<<3);
    set_word(p_reg_info->cr1, set_val);

    // send data
    set_byte(p_reg_info->dr, *p_sdata);
    p_sdata++;
    send_num--;

    // save data info
    this->p_sdata = p_sdata;
    this->send_num = send_num;

    // end critical section
    critical_section_end();

    return ERR_OK;
}
