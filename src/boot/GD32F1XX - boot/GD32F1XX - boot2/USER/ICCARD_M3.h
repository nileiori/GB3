/*************************************************************************
* Copyright (c) 2014 All rights reserved., 
* �ļ����� : ICCARD_M3.h
* ��ǰ�汾 : 0.1
* ������   : MARK
* �༭ʱ�� : 2014��03��17��
* ����ʱ�� ��
*************************************************************************/

#ifndef _ICCARD_M3_H
#define _ICCARD_M3_H

#include "stm32f10x.h"
#include <string.h>
#include <inttypes.h>


/********************�궨��ͨѶ�˿ڣ�I/0,CLK,RST,DETECT,POWER**************/

/*********************************I/0--->PF0*****************************/ 

#define RCC_ICCARD_IO    RCC_APB2Periph_GPIOF           /* GPIO��F��˿�ʱ�� */
#define GPIO_ICCARD_IO   GPIOF                          /* ѡ��F��˿�*/ 
#define PIN_ICCARD_IO    GPIO_Pin_0                     /* ѡ���˿�0*/ 

/********************************CLK--->PF1*****************************/ 

#define RCC_ICCARD_CLK    RCC_APB2Periph_GPIOF           /* GPIO��F��˿�ʱ�� */
#define GPIO_ICCARD_CLK   GPIOF                          /* ѡ��F��˿�*/ 
#define PIN_ICCARD_CLK    GPIO_Pin_1                     /* ѡ���˿�1*/   

/*******************************RST--->PG14****************************/

#define RCC_ICCARD_RST    RCC_APB2Periph_GPIOG           /* GPIO��G��˿�ʱ�� */
#define GPIO_ICCARD_RST   GPIOG                          /* ѡ��G��˿�*/ 
#define PIN_ICCARD_RST    GPIO_Pin_14                    /* ѡ���˿�14*/

/*****************************DETECT--->PD5****************************/

#define RCC_ICCARD_DETECT    RCC_APB2Periph_GPIOD           /* GPIO��D��˿�ʱ�� */
#define GPIO_ICCARD_DETECT   GPIOG                          /* ѡ��D��˿�*/ 
#define PIN_ICCARD_DETECT    GPIO_Pin_9                     /* ѡ���˿�5*/

/*****************************POWER--->PC9****************************/

#define RCC_ICCARD_POWER    RCC_APB2Periph_GPIOC           /* GPIO��C��˿�ʱ�� */
#define GPIO_ICCARD_POWER   GPIOC                          /* ѡ��C��˿�*/ 
#define PIN_ICCARD_POWER    GPIO_Pin_9                     /* ѡ���˿�9*/

/*****************************SHOCK_CS--->PD7****************************/

#define RCC_BMA250_SHOCK_CS    RCC_APB2Periph_GPIOD           /* GPIO��D��˿�ʱ�� */
#define GPIO_BMA250_SHOCK_CS   GPIOD                          /* ѡ��D��˿�*/ 
#define PIN_BMA250_SHOCK_CS    GPIO_Pin_7                     /* ѡ���˿�7*/

/******����ͨѶ�˿ڣ�I/0,CLK,RST,DETECT,POWER����λ�͸�λ**************/

/*********************************I/0--->PF0*****************************/ 

#define ICCARD_SDA_SET()  GPIO_SetBits(    GPIO_ICCARD_IO , PIN_ICCARD_IO )	    /* IO = 1 */
#define ICCARD_SDA_RESET()  GPIO_ResetBits(  GPIO_ICCARD_IO , PIN_ICCARD_IO )		    /* IO = 0 */
#define ICCARD_SDA_READ()  GPIO_ReadInputDataBit( GPIO_ICCARD_IO , PIN_ICCARD_IO )/* ��IO����״̬ */

/********************************CLK--->PF1*****************************/ 

#define ICCARD_CLK_SET()  GPIO_SetBits(    GPIO_ICCARD_CLK , PIN_ICCARD_CLK )		/* CLK = 1 */
#define ICCARD_CLK_RESET()  GPIO_ResetBits(  GPIO_ICCARD_CLK , PIN_ICCARD_CLK )     /* CLK = 0 */

/*******************************RST--->PG14****************************/

#define ICCARD_RST_SET()  GPIO_SetBits(    GPIO_ICCARD_RST , PIN_ICCARD_RST )		/* RST = 1 */
#define ICCARD_RST_RESET()  GPIO_ResetBits(  GPIO_ICCARD_RST , PIN_ICCARD_RST )     /* RST = 1 */

/*****************************DETECT--->PD5****************************/

#define ICCARD_DETECT_SET()  GPIO_SetBits(    GPIO_ICCARD_DETECT , PIN_ICCARD_DETECT )		/* DETECT = 1 */
#define ICCARD_DETECT_RESET()  GPIO_ResetBits(  GPIO_ICCARD_DETECT , PIN_ICCARD_DETECT )        /* DETECT = 0 */
#define ICCARD_DETECT_READ()  GPIO_ReadInputDataBit( GPIO_ICCARD_DETECT , PIN_ICCARD_DETECT )	/* ��IO����״̬ */

/*****************************POWER--->PC9****************************/

#define ICCARD_POWER_ON()  GPIO_SetBits(    GPIO_ICCARD_POWER , PIN_ICCARD_POWER )		   /* POWER = 1 */
#define ICCARD_POWER_OFF()  GPIO_ResetBits(  GPIO_ICCARD_POWER , PIN_ICCARD_POWER )          /* POWER = 0 */

/*****************************SHOCK_CS--->PD7****************************/

#define BMA250_SHOCK_CS_SET()  GPIO_SetBits(    GPIO_BMA250_SHOCK_CS , PIN_BMA250_SHOCK_CS )		   /* SHOCK_CS = 1 */
#define BMA250_SHOCK_CS_RESET()  GPIO_ResetBits(  GPIO_BMA250_SHOCK_CS , PIN_BMA250_SHOCK_CS )

/**************************�궨��I2C����λ***************************/

#define I2C_WR	0	               	/* д����bit */
#define I2C_RD	1	            	/* ������bit */

/*****************************����������****************************/ 

extern void   ICCARD_M3_Init(void);

extern void ICCARD_M3_I2C_Delay(void);

extern void ICCARD_M3_I2C_Start(void);

extern void ICCARD_M3_I2C_Stop(void);

extern void ICCARD_M3_I2C_SendByte(u8 Byte );

extern u8 ICCARD_M3_I2C_ReadByte(void);

extern u8 ICCARD_M3_I2C_CheckAck(void);

extern void ICCARD_M3_I2C_SendAck(void);

extern void ICCARD_M3_I2C_SendNAck(void);

extern u8 ICCARD_M3_I2C_CheckDevice(u8 Address);

extern u8 Power_On(void);

extern u8 InByte(void);

extern void OutByte(u8 ch);

extern void Start_Comm(void);

extern void Stop_Comm(void);

extern void SendComm(u8 a,u8 b,u8 c);

extern void Proce_Mod(void);

extern void Read_Mod(u8  *pt,u8 i);

extern void RstCard(void);

extern void Rmm(u8   cardAdd, u8    *pt,u8    i);

void ICCARD_M3_I2C_Delay(void);
void M3_I2C_Delays(unsigned char n);
#endif 

/*******************************************************************************
 *                             end of module
 *******************************************************************************/

