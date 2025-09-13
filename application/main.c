/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   app_main.c
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "error.h"
#include "port_drv.h"
#include "utility.h"
#include "task_manage.h"
#include "task_sync.h"
#include "cyc_hdlr.h"
#include "mailbox.h"
#include "mem_alloc.h"
#include "usart.h"
#include "i2c.h"
#include <stdio.h>
#include "int_hdlr.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    T_MSG   msghd;
    UW      lvl;
} msg_t;

static void test1(void)
{
    uint32_t lvl;
    msg_t *msg;

    while (1) {
        if (rcv_mbx(0, (T_MSG**)&msg, TMO_FEVR) == E_OK) {
            lvl = msg->lvl;
            port_drv_set_pin_lvl(PORTB4, lvl);
            mem_free(msg);
        }  
    } 
}

static void test2(void)
{
    uint32_t lvl = 0;
    msg_t *msg;

    while (1) {
        msg = (msg_t*)mem_alloc(sizeof(msg_t)); 
        msg->lvl = lvl;
        lvl = ~lvl & 1;
        snd_mbx(0, (T_MSG*)msg);
        dly_tsk(1000);
    }
}

int swait;
void send_callback(void *p_par)
{
    swait = 1;
}

int waitflag;
void recv_callback(void *p_par)
{
    waitflag = 1;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  app main
 */
void app_main(void)
{
    i2c_recovery();
    uint8_t i; 
    uint8_t data = 0x11;
    uint8_t rdata = 0;
    i2c_peri_config_t config = {
        I2C_MODE_I2C,
        10 * 1000,
        I2C_ANALOG_FILTER_ON,
        I2C_DEGITAL_FILTER_OFF,
    };
    i2c_init();
    if (i2c_config(I2C_CH_1, &config) != ERR_OK) {
        while (1);
    }
    if (i2c_open(I2C_CH_1, INT_PRI_I2C_CH1, send_callback, recv_callback, &i) != ERR_OK) {
        while (1);
    }
/* 
    // start oneshot measurement | increment register automatically
    swait = 0;
    i2c_send(I2C_CH_1, 0xD6, 0x11, &data, 1);
    while (swait != 1);
    // wait 
    while (1) {
        waitflag = 0;
        i2c_recv(I2C_CH_1, 0xBA, 0x27, &rdata, 1);
        while (waitflag != 1);
        if (rdata & 1) {
            break;
        }
    }
    uint8_t sensor_data[3] = {0};
    waitflag = 0;
    i2c_recv(I2C_CH_1, 0xBA, 0x28, sensor_data, 3); 
    while (waitflag != 1);
    uint32_t now = sensor_data[2] << 16 | sensor_data[1] << 8 | sensor_data[0];  
    now = now / 4096;
*/
    uint8_t senddata[2] = { 0x10, 0x10 };
    uint8_t recvdata;
    uint8_t sensor_data[6];
    swait = 0;
    i2c_send(I2C_CH_1, 0xD6, 0x10, senddata, 2);
    while (swait != 1);
    // wait
    while (1) {
        waitflag = 0;
        i2c_recv(I2C_CH_1, 0xD6, 0x1E, &recvdata, 1);
        while (waitflag != 1);
        if ((recvdata & 0x03) == 0x03) {
            break; 
        }
    }
    waitflag = 0;
    i2c_recv(I2C_CH_1, 0xD6, 0x28, sensor_data, 6);
    while (waitflag != 1);
    int16_t x = sensor_data[1]<<8 | sensor_data[0];
    int16_t y = sensor_data[3]<<8 | sensor_data[2];
    int16_t z = sensor_data[5]<<8 | sensor_data[4];




    //--------------------------------------------------------------------
    port_drv_set_pin_func(PORTB4, PORTB4_OUTPUT, PORT_LVL_HIGH, 0, 0, 0);

    T_CTSK init_ctsk;
    init_ctsk.tskatr = TA_HLANG|TA_ACT;
    init_ctsk.exinf = NULL;
    init_ctsk.task = test1;
    init_ctsk.itskpri = 0;
    init_ctsk.stksz = 256;
    init_ctsk.stk = NULL;
    if (cre_tsk(2, &init_ctsk)) {
        while (1);
    }
    init_ctsk.tskatr = TA_HLANG|TA_ACT;
    init_ctsk.exinf = NULL;
    init_ctsk.task = test2;
    init_ctsk.itskpri = 1;
    init_ctsk.stksz = 256;
    init_ctsk.stk = NULL;
    if (cre_tsk(3, &init_ctsk)) {
        while (1);
    }

    T_CMBX cmbx;
    cmbx.mbxatr = TA_TFIFO;
    if (cre_mbx(0, &cmbx)) {
        while (1);
    }
   
    // end app main
    slp_tsk(TMO_FEVR);

    // no reached 
    while (1); 
}
