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

#define ISSUE_START_COND(ch)
#define ISSUE_STOP_COND(ch)
#define ISSUSE_RESTART_COND(ch)     (ISSUE_START_COND((ch)))

/*--------------------------------------------------------------------------------------*/
/*! @brief prototype
 */
static void i2c1_er_inthdlr(void);
static void i2c1_ev_inthdlr(void);

/*--------------------------------------------------------------------------------------*/
/*! @brief i2c state
 */
typedef enum {
    I2C_ST_IDLE = 0,
    I2C_ST_WAIT_START_COND,     //!< wait start condition
    I2C_ST_WAIT_SEND_SLVADDR,   //!< wait send slave addr
    I2C_ST_WAIT_SEND_REGADDR,   //!< wait send register addr
    I2C_ST_WAIT_SEND_DATA,      //!< wait send data
    I2C_ST_WAIT_STOP_COND,      //!< wait stop condition
    I2C_ST_WAIT_RECV_DATA,      //!< wait recv data
    I2C_ST_WAIT_RESTART_COND,   //!< wait restart condition
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
    i2c_send_callback   send_callback;  //!< send callback
    i2c_send_callback   recv_callback;  //!< recv callback
    void                *p_callback_par;//!< callback parameter
    i2c_state_t         state;          //!< i2c state
    uint8_t             mode;           //!< send or recv
} i2c_ctrl_blck_t;

static i2c_ctrl_blck_t i2c_ctl_blk[I2C_CH_MAX];
#define get_myself(ch)      (&i2c_ctl_blk[(ch)])

/*--------------------------------------------------------------------------------------*/
/*! @brief common ER interrupt handler
 */
static void common_er_inthdlr(uint32_t ch)
{

}

/*--------------------------------------------------------------------------------------*/
/*! @brief common EV interrupt handler
 */
static void common_ev_inthdler(uint32_t ch)
{

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

    // get control block 
    this = get_myself(ch);

    // save send data info

    // set mode

    // i2c module enable

    // issue start condition

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

    // get control block

    // save recv info

    // set mode

    // i2c module enable

    // issue critical section

    // end critical section
    critical_section_end();

    return ERR_OK;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief close
 */
int32_t i2c_close(uint32_t ch)
{

}
