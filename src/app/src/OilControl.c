/*******************************************************************************
 * File Name:			OilControl.c 
 * Function Describe:	
 * Relate Module:		
 * Writer:				Joneming
 * Date:				2012-12-04
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
#include "VDR.h"
////////////////////////
/*******************************************************************************/
/*******************************************************************************/
#define CUT_OIL_SPEED       20	//�ж���·�ٶȣ�С�ڸ��ٶ�ʱ�ſ����ж���·,20km/h
//////////////////////////////////////
typedef enum
{
   OIL_OPEN =0,          //ͨ��·,��ֵ����IO�ŵĵ�ƽ
   OIL_CLOSE,            //����·
   OIL_MAX               //
}OIL_STATUS;

typedef enum
{
    CTRL_OIL_EMPTY=0x55,       //��
    CTRL_OIL_OPEN,             //ͨ��·
    CTRL_OIL_CLOSE,            //����·    
    CTRL_OIL_MAX               //
}CTRL_OIL_STATUS;
typedef enum
{
    CTRLOIL_STEP_EMPTY,            //��
    CTRLOIL_STEP_OPEN,             //ͨ��·
    CTRLOIL_STEP_CLOSE,            //����·
    CTRLOIL_STEP_CHECK_OPEN,       //����Ƿ�Ϊͨ��·
    CTRLOIL_STEP_CHECK_CLOSE,      //����Ƿ�Ϊ����·
    CTRLOIL_STEP_MAX               //
}CTRLOIL_STEP;
///////////////////////////////////
static	unsigned char	s_ucOilControlFlag = CTRL_OIL_EMPTY;//��·�򿪱�־,0x00:��ͨ��·,0x01:�Ѷ���·
static 	unsigned char	s_ucOilControlStepFlag = CTRLOIL_STEP_EMPTY;//��·���Ʋ���,0x00:ͨ��·,0x01:����·
extern  u8	SpeedFlag;//�ٶ�����,0Ϊ����,1ΪGPS
/*************************************************************OK
** ��������: OilControl_IoInit
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void OilControl_IoInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}  
/*******************************************************************************
**  ��������  : OilControl_GetOilWayStatus
**  ��������  : 
**  ��    ��  : 				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
unsigned char OilControl_GetOilWayStatus(void)
{
    return GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_2);   
}
/*******************************************************************************
**  ��������  : OilControl_CtrlOilWay
**  ��������  : 
**  ��    ��  : 				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void OilControl_CtrlOilWay(unsigned char status)
{
    if(0 == status)//
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_2);//
    }
    else//
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_2);//
    }
}
/*******************************************************************************
**  ��������  : OilControl_ParameterInitialize
**  ��������  : ����·��ʼ��
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void OilControl_ParameterInitialize(void)
{
    OilControl_IoInit();
    s_ucOilControlStepFlag = CTRLOIL_STEP_EMPTY;
    FRAM_BufferRead((unsigned char *)&s_ucOilControlFlag,1,FRAM_OIL_CTRL_ADDR);
    if(CTRL_OIL_CLOSE == s_ucOilControlFlag)
    {
        s_ucOilControlFlag=CTRL_OIL_EMPTY;
        OilControl_OffOil();
    }
    else//�����ľ�ͨ��
    {
        OilControl_OpenOil();
    }
}
/*******************************************************************************
**  ��������  : OilControl_OpenOil
**  ��������  : ͨ��
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void OilControl_OpenOil(void)
{
    unsigned char temp;
    temp = CTRL_OIL_OPEN;
    FRAM_BufferWrite(FRAM_OIL_CTRL_ADDR,(unsigned char *)&temp,1);
    s_ucOilControlStepFlag =CTRLOIL_STEP_OPEN;
    SetTimerTask(TIME_OIL_CTRL, 2*SYSTICK_0p1SECOND);
}
/*******************************************************************************
**  ��������  : OilControl_OffOil
**  ��������  : ����
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void OilControl_OffOil(void)
{
    unsigned char temp;
    temp = CTRL_OIL_CLOSE;
    FRAM_BufferWrite(FRAM_OIL_CTRL_ADDR,(unsigned char *)&temp,1);
    s_ucOilControlStepFlag = CTRLOIL_STEP_CLOSE;
    SetTimerTask(TIME_OIL_CTRL, SYSTICK_1SECOND);
}
/*******************************************************************************
**  ��������  : ControlOil_TimeTask
**  ��������  : ��·����������Ƚӿں���
**  ��    ��  : ��				
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
FunctionalState ControlOil_TimeTask(void)
{
    unsigned char OilCtrlFlag = 0;//��·���Ʊ�־
    unsigned char Speed = 0;//��ǰ�ٶ�
    unsigned char temp;//
    static  unsigned char   sucTryCtrlCnt = 0;
    static  unsigned char   sucCtrlOffStepCnt = 0; 

	
    if(CTRLOIL_STEP_CHECK_OPEN  == s_ucOilControlStepFlag)//ͨ��
    {
        sucCtrlOffStepCnt =0;
        if(OIL_OPEN == OilControl_GetOilWayStatus())
        {
            sucTryCtrlCnt = 0;
        }
        else
        if(sucTryCtrlCnt<3)
        {
            sucTryCtrlCnt++;
            OilControl_CtrlOilWay(OIL_OPEN);//ͨ��
            return ENABLE;
        }
    }
    else
    if(CTRLOIL_STEP_OPEN  == s_ucOilControlStepFlag)//ͨ��
    {
        sucCtrlOffStepCnt =0;
        s_ucOilControlFlag = CTRL_OIL_OPEN;        
        FRAM_BufferWrite(FRAM_OIL_CTRL_ADDR,(unsigned char *)&s_ucOilControlFlag,1);
        /////////////////////////////////
        OilControl_CtrlOilWay(OIL_OPEN);//ͨ��
        s_ucOilControlStepFlag =CTRLOIL_STEP_CHECK_OPEN;
        FRAM_BufferRead((unsigned char *)&OilCtrlFlag,1,FRAM_OIL_CTRL_ADDR);
        if(OilCtrlFlag!=CTRL_OIL_OPEN)
        {
            FRAM_BufferWrite(FRAM_OIL_CTRL_ADDR,(unsigned char *)&s_ucOilControlFlag,1);
        }
        //////////////////////////////////
        sucTryCtrlCnt =0;
        SetTimerTask(TIME_OIL_CTRL, 3*SYSTICK_1SECOND);
        ////////////////////
        return ENABLE;
    }    
    else
    if(CTRLOIL_STEP_CHECK_CLOSE  == s_ucOilControlStepFlag)//����
    {
        sucCtrlOffStepCnt =0;
        if(OIL_OPEN != OilControl_GetOilWayStatus())
        {
            sucTryCtrlCnt = 0;
        }
        else
        if(sucTryCtrlCnt<3)
        {
            sucTryCtrlCnt++;
            OilControl_CtrlOilWay(OIL_CLOSE);//����
            return ENABLE;
        }
    }
    else    
    if(CTRLOIL_STEP_CLOSE  == s_ucOilControlStepFlag)//����
    {
        if(CTRL_OIL_CLOSE == s_ucOilControlFlag)//
        {
            s_ucOilControlStepFlag =CTRLOIL_STEP_CHECK_CLOSE;
            sucTryCtrlCnt =0;
            SetTimerTask(TIME_OIL_CTRL, 3*SYSTICK_1SECOND);
            return ENABLE;
        }
	    //��ȡ�ٶ�ѡ��
        if(0 == (SpeedFlag&0x01))
        {
       	   Speed = VDRPulse_GetSecondSpeed();
        }
        else
        {
	         Speed = Gps_ReadSpeed();
        }
	if(Speed > CUT_OIL_SPEED)//��ʻ�ٶȴ���20km/hʱ���ܶ���·
	{
	    return ENABLE;
	}
        //////////////////////////////
	//if(sucCtrlOffStepCnt<20)
        if(sucCtrlOffStepCnt<10)//dxl,2012.12.19,��5������,ֻ�����ٶȵ���
        {
            sucCtrlOffStepCnt++;
            if(sucCtrlOffStepCnt%2==1)
            {
                OilControl_CtrlOilWay(OIL_CLOSE);//����
            }
            else
            {
                OilControl_CtrlOilWay(OIL_OPEN);//ͨ��
            }
        }
        else
        {
            OilControl_CtrlOilWay(OIL_CLOSE);//����
            sucCtrlOffStepCnt =0;
            s_ucOilControlFlag = CTRL_OIL_CLOSE;
            FRAM_BufferWrite(FRAM_OIL_CTRL_ADDR,(unsigned char *)&s_ucOilControlFlag,1);
            /////////////////////////////////
            OilControl_CtrlOilWay(OIL_CLOSE);//����
            s_ucOilControlStepFlag = CTRLOIL_STEP_CHECK_CLOSE;
            FRAM_BufferRead((unsigned char *)&temp,1,FRAM_OIL_CTRL_ADDR);
            if(temp != CTRL_OIL_CLOSE)
            {
                FRAM_BufferWrite(FRAM_OIL_CTRL_ADDR,(unsigned char *)&s_ucOilControlFlag,1);
            }
            //////////////////////////////////
            sucTryCtrlCnt = 0;
            SetTimerTask(TIME_OIL_CTRL, 3*SYSTICK_1SECOND);
        }       
        /////////////////////
        return ENABLE;
    }
    //////////////////////////////
    sucTryCtrlCnt =0;
    sucCtrlOffStepCnt =0;
    temp =OilControl_GetOilWayStatus();
    if(OIL_OPEN==temp)
    {
        Io_WriteStatusBit(STATUS_BIT_OIL_CUT,RESET) ;     //��·ͨ��
    }
    else
    {
        Io_WriteStatusBit(STATUS_BIT_OIL_CUT,SET) ;     //��·�Ͽ�
    }
    ///////////////////////////////////
    s_ucOilControlStepFlag = CTRLOIL_STEP_MAX;
    return DISABLE;
}
/******************************************************************************
**                            End Of File
******************************************************************************/

