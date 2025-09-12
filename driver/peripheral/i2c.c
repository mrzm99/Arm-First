/*--------------------------------------------------------------------------------------*/
/*! 
 *  @file   i2c.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/

#include "i2c.h"
#include "system_define.h"
#include "error.h"
#include "iodefine.h"
#include "utility.h"
#include "int_hdlr.h"
#include "port_drv.h"
#include <stdint.h>
#include <string.h>
#include <stdint.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief macro
 */
#define I2C_MODE_SEND       (0)
#define I2C_MODE_RECV       (1)

#define ISSUE_START_COND(ch)        do {    \
                                        const i2c_reg_t *p_reg= &i2c_reg_info[ch]; \
                                        uint16_t set_val = get_hword(p_reg->cr1); \
                                        set_field(uint16_t, set_val, 1<<8, 1);  \
                                        set_hword(p_reg->cr1, set_val);   \
                                    } while(0)

#define ISSUE_STOP_COND(ch)         do { \
                                        const i2c_reg_t *p_reg= &i2c_reg_info[ch]; \
                                        uint16_t set_val = get_hword(p_reg->cr1); \
                                        set_field(uint16_t, set_val, 1<<9, 1); \
                                        set_hword(p_reg->cr1, set_val); \
                                    } while(0)

#define ISSUSE_RESTART_COND(ch)     (ISSUE_START_COND((ch)))

#define SEND_AKC(ch)                do { \
                                        const i2c_reg_t *p_reg= &i2c_reg_info[ch]; \
                                        uint16_t set_val = get_hword(p_reg->cr1); \
                                        set_field(uint16_t, set_val, 1<<10, 1); \
                                        set_hword(p_reg->cr1, set_val); \
                                    } while(0)

#define SEND_NACK(ch)               do { \
                                        const i2c_reg_t *p_reg = &i2c_reg_info[ch]; \
                                        uint16_t set_val = get_hword(p_reg->cr1); \
                                        set_field(uint16_t, set_val, 1<<10, 0); \
                                        set_hword(p_reg->cr1, set_val); \
                                    } while(0)

/*--------------------------------------------------------------------------------------*/
/*! @brief prototype
 */
static void i2c1_er_inthdlr(void);
static void i2c1_ev_inthdlr(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief i2c state
 */
typedef enum {
    I2C_ST_IDLE = 0,            //!< idle
    I2C_ST_WAIT_START_COND,     //!< wait start condition
    I2C_ST_WAIT_SEND_SLVADDR,   //!< wait send slave addr
    I2C_ST_WAIT_SEND_REGADDR,   //!< wait send register addr
    I2C_ST_WAIT_SEND_DATA,      //!< wait send data
    I2C_ST_WAIT_STOP_COND,      //!< wait stop condition
    I2C_ST_WAIT_RECV_DATA,      //!< wait recv data
    I2C_ST_WAIT_RESTART_COND,   //!< wait restart condition
    I2C_ST_HANDLER_NUM,         //!< handler num
} i2c_state_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief reg info
 */
typedef struct {
    uint32_t cr1;
    uint32_t cr2;
    uint32_t oar1;
    uint32_t oar2;
    uint32_t dr;
    uint32_t sr1;
    uint32_t sr2;
    uint32_t ccr;
    uint32_t trise;
    uint32_t fltr;
} i2c_reg_t; 

static const i2c_reg_t i2c_reg_info[I2C_CH_MAX] = {
    // ch1
    {
        I2C1_CR1,
        I2C1_CR2,
        I2C1_OAR1,
        I2C1_OAR2,
        I2C1_DR,
        I2C1_SR1,
        I2C1_SR2,
        I2C1_CCR,
        I2C1_TRISE,
        I2C1_FLTR,
    }
};

/*--------------------------------------------------------------------------------------*/
/*! @brief peri info
 */
#define I2C_INTNO_EV        (0)
#define I2C_INTNO_ER        (1)
#define I2C_INTNO_MAX       (2)

typedef struct {
    uint32_t inthdr_no;
    INT_HDLR inthdr;
    uint32_t pri_no;
    uint32_t ena_no;
    uint32_t dis_no;
} inthdr_info_t;

typedef struct {
    inthdr_info_t inthdr_info[I2C_INTNO_MAX];
    uint32_t port_no_scl;
    uint32_t port_mode_scl;
    uint32_t port_no_sda;
    uint32_t port_mode_sda;
    // additional info
} i2c_peri_info_t;

static const i2c_peri_info_t peri_info[I2C_CH_MAX] = {
    // ch1
    {
        {
            {INTHDLR_NO_I2C1_ER, i2c1_er_inthdlr, INTPRI_NO_I2C1_ER, INTENA_NO_I2C1_ER, 0}, // ER interrupt info
            {INTHDLR_NO_I2C1_EV, i2c1_ev_inthdlr, INTPRI_NO_I2C1_EV, INTENA_NO_I2C1_EV, 0}, // EV interrupt info
        },
        PORTB6,
        PORTB6_I2C1_SCL,
        PORTB7,
        PORTB7_I2C1_SDA,
    }
};

/*--------------------------------------------------------------------------------------*/
/*! @brief I2C control block
 */
typedef struct {
    uint8_t             slv_addr;       //!< slave addr
    uint8_t             reg_addr;       //!< reg addr
    uint8_t             *p_sdata;       //!< send data
    uint8_t             *p_rdata;       //!< recv data
    uint32_t            send_num;       //!< send num
    uint32_t            recv_num;       //!< recv num
    uint32_t            data_num;       //!< recved num
    i2c_send_callback   send_callback;  //!< send callback
    i2c_send_callback   recv_callback;  //!< recv callback
    void                *p_callback_par;//!< callback parameter
    i2c_state_t         state;          //!< i2c state
    uint8_t             mode;           //!< send or recv
} i2c_ctrl_blck_t;

static i2c_ctrl_blck_t i2c_ctl_blk[I2C_CH_MAX];
#define get_myself(ch)      (&i2c_ctl_blk[(ch)])

/*--------------------------------------------------------------------------------------*/
/*! @brief handler table
 */
static void idle_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);
static void wait_start_cond_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);
static void wait_send_slave_addr_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);
static void wait_send_reg_addr_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);
static void wait_send_data_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);
static void wait_stop_condition_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);
static void wait_recv_data_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);
static void wait_restart_cond_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);

typedef void (*state_handler)(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info);
static const state_handler handler_table[I2C_ST_HANDLER_NUM] = {
    idle_hdr,
    wait_start_cond_hdr,
    wait_send_slave_addr_hdr, 
    wait_send_reg_addr_hdr,
    wait_send_data_hdr,
    wait_stop_condition_hdr,
    wait_recv_data_hdr,
    wait_restart_cond_hdr,
};

/*--------------------------------------------------------------------------------------*/
/*! @brief handler at idle
 */
static void idle_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info)
{
    // This function never called
    while (1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief handler at waite start condition
 */
static void wait_start_cond_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info)
{
    // flag clear 
    uint16_t set_val = get_hword(p_reg_info->sr1);
    clr_field(uint16_t, set_val, 1<<0);
    set_hword(p_reg_info->sr1, set_val);

    // send slave address
    this->slv_addr = this->slv_addr & ~1;
    set_byte(p_reg_info->dr, this->slv_addr);

    // change state to "wait send slave addr"
    this->state = I2C_ST_WAIT_SEND_SLVADDR;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief hanlder at wait send slave address
 */
static void wait_send_slave_addr_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info)
{
    // flag clear
    uint16_t set_val = get_hword(p_reg_info->sr1);
    clr_field(uint16_t, set_val, 1<<2);
    set_hword(p_reg_info->sr1, set_val);

    // send register address
    set_byte(p_reg_info->dr, this->reg_addr);

    // chage state to "wait send reg addr"
    this->state = I2C_ST_WAIT_SEND_REGADDR;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief handler at wait send register address
 */
static void wait_send_reg_addr_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info)
{
    // flag clear
    uint16_t set_val = get_hword(p_reg_info->sr1);
    clr_field(uint16_t, set_val, 1<<2);
    set_hword(p_reg_info->sr1, set_val);

    // if send process
    if (this->mode == I2C_MODE_SEND) {
        // send data
        set_byte(p_reg_info->dr, *this->p_sdata);
        this->send_num--;
        this->p_sdata++;
        // chage state to "wait send data"
        this->state = I2C_ST_WAIT_SEND_DATA;
    } else {
        // issue restart condition
        // change state to "wait restart condition"
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief handler at wait send data
 */
static void wait_send_data_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info)
{
    // flag clear
    uint16_t set_val = get_hword(p_reg_info->sr1);
    clr_field(uint16_t, set_val, 1<<2);
    set_hword(p_reg_info->sr1, set_val);

    // if sendnum == 0
    if (this->send_num == 0) {
        // issue stop condition
        ISSUE_STOP_COND(ch);
        // change state to "wait stop condition"
        this->state = I2C_ST_WAIT_STOP_COND;
    } else {
        // send data
        set_byte(p_reg_info->dr, *this->p_sdata);
        this->send_num--;
        this->p_sdata++;
    } 
}

/*--------------------------------------------------------------------------------------*/
/*! @brief handler at wait stop condition
 */
static void wait_stop_condition_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info)
{
    // flag clear 
    uint16_t set_val = get_hword(p_reg_info->sr1);
    clr_field(uint16_t, set_val, 1<<4);
    set_hword(p_reg_info->sr1, set_val);
    
    // stop i2c
}

/*--------------------------------------------------------------------------------------*/
/*! @brief handler at wait recv data
 */
static void wait_recv_data_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info)
{

}

/*--------------------------------------------------------------------------------------*/
/*! @brief handler at wait restart condition
 */
static void wait_restart_cond_hdr(uint32_t ch, i2c_ctrl_blck_t *this, const i2c_reg_t *p_reg_info)
{

}

/*--------------------------------------------------------------------------------------*/
/*! @brief common ER interrupt handler
 */
static void common_er_inthdlr(uint32_t ch)
{
    // debug
    while (1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief common EV interrupt handler
 */
static void common_ev_inthdler(uint32_t ch)
{
    i2c_ctrl_blck_t *this = get_myself(ch);
    const i2c_reg_t *p_reg_info = &i2c_reg_info[ch];

   handler_table[this->state](ch, this, p_reg_info); 
}

/*--------------------------------------------------------------------------------------*/
/*! @brief i2c1 ER interrupt handler
 */
static void i2c1_er_inthdlr(void)
{
    common_er_inthdlr(I2C_CH_1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief i2c1 EV interrupt handler
 */
static void i2c1_ev_inthdlr(void)
{
    common_ev_inthdler(I2C_CH_1);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief set register ### fixed setting ###
 */
static void set_reg(const i2c_reg_t *p_reg, i2c_peri_config_t *p_config)
{
    uint32_t set_val;
    uint32_t ccr_val;

    // set CCR
    ccr_val = (APB1_CLOCK_FREQ * 1000 * 1000) / (2 * p_config->bitrate);
    set_word(p_reg->ccr, ccr_val);
    // set CR1
    set_val = 0x00;
    set_word(p_reg->cr1, set_val);
    // set CR2
    set_val = APB1_CLOCK_FREQ;
    set_word(p_reg->cr2, set_val);
    // set TRISE
    set_val = 1000 / (1000 / APB1_CLOCK_FREQ);
    set_word(p_reg->trise, set_val);
}

/*--------------------------------------------------------------------------------------*/
/*! @brief init
 */
void i2c_init(void)
{
    i2c_ctrl_blck_t *this;

    for (int ch = 0; ch < I2C_CH_MAX; ch++) {
        this = get_myself(ch);
        memset(this, 0, sizeof(i2c_ctrl_blck_t));
        this->state = I2C_ST_IDLE;
    }
}

/*--------------------------------------------------------------------------------------*/
/*! @brief config 
 */
int32_t i2c_config(uint32_t ch, i2c_peri_config_t *p_config)
{
    const i2c_reg_t *p_reg_info;
    uint32_t set_val;

    // parameter check
    if (ch >= I2C_CH_MAX) {
        return ERR_PAR;
    }
    if (p_config == NULL) {
        return ERR_PAR;
    }

    // Todo: check config data  

    // start critical section
    critical_section_start();

    // get reg info
    p_reg_info = &i2c_reg_info[ch];

    // Todo: disalbe i2c
    
    // set register
    set_reg(p_reg_info, p_config);

    // set port
    port_drv_set_pin_func(peri_info[ch].port_no_scl, peri_info[ch].port_mode_scl, PORT_LVL_HIGH, 0 ,0 ,0);
    port_drv_set_pin_func(peri_info[ch].port_no_sda, peri_info[ch].port_mode_sda, PORT_LVL_HIGH, 0, 0, 0);

    // set interrupt handler
    int_drv_set_handler(peri_info[ch].inthdr_info[I2C_INTNO_ER].inthdr_no, peri_info[ch].inthdr_info[I2C_INTNO_ER].inthdr);
    int_drv_set_handler(peri_info[ch].inthdr_info[I2C_INTNO_EV].inthdr_no, peri_info[ch].inthdr_info[I2C_INTNO_EV].inthdr);

    // end critical section
    critical_section_end();

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief open
 */
int32_t i2c_open(uint32_t ch, uint32_t pri, i2c_send_callback send_callback, i2c_recv_callback recv_callback, void *p_callback_par)
{
    i2c_ctrl_blck_t *this;
    const i2c_reg_t *p_reg_info; 
    uint16_t set_val;

    // check parameter
    if (ch >= I2C_CH_MAX) {
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

    // start critical section
    critical_section_start();

    // save callback
    this->send_callback = send_callback;
    this->recv_callback = recv_callback;
    this->p_callback_par = p_callback_par;

    // set interupt priority
    int_drv_set_pri(peri_info[ch].inthdr_info[I2C_INTNO_ER].pri_no, pri);
    int_drv_set_pri(peri_info[ch].inthdr_info[I2C_INTNO_EV].pri_no, pri);

    // enable interrupt of int driver 
    int_drv_ena_int(peri_info[ch].inthdr_info[I2C_INTNO_ER].ena_no);
    int_drv_ena_int(peri_info[ch].inthdr_info[I2C_INTNO_EV].ena_no);

    // enable interrupt on i2c module
    p_reg_info = &i2c_reg_info[ch];
    set_val = get_hword(p_reg_info->cr2);
    set_field(uint16_t, set_val, 3<<8, 3);
    set_hword(p_reg_info->cr2, set_val);

    // end critical section
    critical_section_end();

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief send
 */
int32_t i2c_send(uint32_t ch, uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_data, uint32_t send_num)
{
    uint32_t set_val;
    i2c_ctrl_blck_t *this;
    const i2c_reg_t *p_reg_info;

    // parameter check
    if (ch >= I2C_CH_MAX) {
        return ERR_PAR;
    }
    if (p_data == NULL) {
        return ERR_PAR;
    }
    if (send_num == NULL) {
        ERR_OK;
    }

    // start critical section
    critical_section_start();

    // get reg info
    p_reg_info = &i2c_reg_info[ch];

    // get control block 
    this = get_myself(ch);

    // save send data info
    this->slv_addr = slave_addr;
    this->reg_addr = reg_addr;
    this->p_sdata = p_data;
    this->send_num = send_num;

    // set mode
    this->mode = I2C_MODE_SEND;

    // i2c module enable
    set_val = get_word(p_reg_info->cr1);
    set_field(uint32_t, set_val, 1, 1);
    set_word(p_reg_info->cr1, set_val);

    // change state
    this->state = I2C_ST_WAIT_START_COND;

    // issue start condition
    ISSUE_START_COND(ch);

    // end critical section
    critical_section_end();

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief recv
 */
int32_t i2c_recv(uint32_t ch, uint8_t slave_addr, uint8_t reg_addr, uint8_t *p_data, uint32_t recv_num)
{
    uint32_t set_val;
    i2c_ctrl_blck_t *this;
    const i2c_reg_t *p_reg_info;

    // parameter check
    if (ch >= I2C_CH_MAX) {
        return ERR_PAR;
    }
    if (p_data == NULL) {
        return ERR_PAR;
    }
    if (recv_num == 0) {
        return ERR_OK;
    }

    // start critical section
    critical_section_start();

    // get reg info 
    p_reg_info = &i2c_reg_info[ch];

    // get control block
    this = get_myself(ch);

    // save recv info
    this->slv_addr = slave_addr;
    this->reg_addr = reg_addr;
    this->p_rdata = p_data;
    this->recv_num = recv_num;

    // set mode
    this->mode = I2C_MODE_RECV;

    // i2c module enable
    set_val = get_word(p_reg_info->cr1);
    set_field(uint32_t, set_val, 1, 1);
    set_word(p_reg_info->cr1, set_val);

    // change state
    this->state = I2C_ST_WAIT_START_COND;

    // issue start condition 
    ISSUE_START_COND(ch);

    // end critical section
    critical_section_end();

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief close
 */
int32_t i2c_close(uint32_t ch)
{
    // parameter check
    // disable i2c
    // disable interrupt
}
