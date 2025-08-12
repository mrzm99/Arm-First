# 自作RTOS
ITRON風(?)の自作RTOS 
## Develepment Environment 
統合開発環境：STM32CubeIDE 1.14.0   
ボード：CQ出版社『STM32で始めるIoT実験教室』Arm-First(Cortex-M4)

## Application  

## Device Driver 
PORT Driver

## RTOS Service Call 
ER cre_tsk(ID tskid, T_CTSK *pk_ctsk);  
ER chg_pri(ID tskid, PRI tskpri);  
ER wup_tsk(ID tskid);  
ER slp_tsk(TMO tmout);  
ER dly_tsk(RELTIM dlytim);  
ER cre_cyc(ID cycid, T_CCYC *p_ccyc);  
ER sta_cyc(ID cycid);  
ER stp_cyc(ID cycid);  
ER cre_sem(ID semid, T_CSEM *p_csem);  
ER wai_sem(ID semid, TMO tmout);  
ER sig_sem(ID semid);  
ER del_sem(ID semid);  
ER cre_flg(ID evtflgid, T_CFLG *p_cflg);  
ER del_flg(ID evtflgid);  
ER wai_flg(ID evtflgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn, TMO tmout);  
ER set_flg(ID evtflgid, FLGPTN setptn);  
ER clr_flg(ID evtflgid, FLGPTN clrptn);  
ER cre_mpf(ID mpfid, T_CMPF *p_cmpf);  
ER del_mpf(ID mpfid);  
ER get_mpf(ID mpfid, VP *p_blk, TMO tmout);  
ER rel_mpf(ID mpfid, VP *p_blk);  

## Utility 
void *mem_alloc(uint32_t size);  
void mem_free(void *p_mbk);  
int32_t set_handler(uint32_t exp_no, INT_HDLR hdlr);  

## ToDo 
MPU 
Memory Pool (Variable)  
Mutex  
Message Buffer  
Mail Box  
Console Task  
timer driver  
UART driver  
SPI driver  
I2C driver  
SD card driver  





