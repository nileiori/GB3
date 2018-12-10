/**
  ******************************************************************************
  * @file    IcCardSocket.c
  * @author  miaoyahan
  * @version V1.0.0
  * @date    2014-3-30
  * @brief   驾驶员身份验证检测专用
  ******************************************************************************
  */
	
#include "my_typedef.h"
#include "stm32f10x.h"
#include "modem_app_com.h"	
#ifndef __ICCARD_SOCKET_H
#define __ICCARD_SOCKET_H




/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  EYENET_MAIN_SOC                         (0)//?IP??soc 0?
#define  EYENET_ICCARD_SOC                       (1)//IC???soc 1?
#define  EYENET_MAIN_SENDTRY                     (11)//????????????????????,???????????????

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/ 
typedef enum 
{
    IC_IDLE = 0,
    IC_MODTMOT,
    IC_MODDOWN,
    IC_MODDO,
    IC_MODOK,
    IC_MODER,
}IC_MODULE_TYPE;

typedef enum
{
  IC_MODULE_IDLE = (0x00),
  IC_MODULE_STAT ,
  IC_MODULE_OPEN ,
  IC_MODULE_LISTEN,  
  IC_MODULE_CLOSE ,
  IC_MODULE_DOWN,
  IC_MODULE_SEND ,
  IC_MODULE_FTP ,
  IC_MODULE_DELAY,
  IC_MODULE_MODEMON , 
  
  IC_MODULE_ERR ,

} IC_MODULE_STATE;

typedef  void (*ICAUC_FUN_READ)(u8*,u16);  //????????

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
FunctionalState ICAUC_TimeTask(void);
ICAUC_RetType   ICAUC_OpenLnk(void);
ICAUC_RetType   ICAUC_SendBuff(u8* buff,u16 len);
void ICAUC_RegReadFun(ICAUC_FUN_READ pFun);
u8   ICAUC_GetLnkStatus(void); 



void IcCardNet_Init(void);
void IcCardNet_Open(u16 delay);
void IcCardNet_SendClear(void);
void IcCardNet_SendValueToCom(u8 *pRec, u16 RecLen);
u8   IcCardNet_SendIs(void);
void IcCardNet_SendDriverWork(void);
void  ICCard_ParameterInitialize(void);
u8 Get_ICModuole_OnLine_fFlag(void);
#endif

