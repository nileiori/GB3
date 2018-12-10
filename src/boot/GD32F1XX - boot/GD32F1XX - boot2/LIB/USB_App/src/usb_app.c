/*******************************************************************************
 * File Name:			usb_app.c 
 * Function Describe:	
 * Relate Module:		������
 * Writer:				Joneming
 * Date:				2012-05-18
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "usb_app.h"
#include "usb_core.h"
#include "usbh_core.h"
#include "string.h"
#include "usbh_msc_core.h"
#include "usbh_usr.h"
#include "Public.h"
#include "EepromPram.h"
#include "taskschedule.h"
#include "SysTick.h"
#include	"Rtc.h"

/////////////////////////////////
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;

//////////////////////////////////////////////////
unsigned char s_ucUsbUpdataFlag;

#define USB_UPDATA_FLAG_VAL    0xbb
enum 
{
    USB_TIMER_TASK,						//
    USB_TIMER_SAVE,						//
    USB_TIMERS_MAX
}USBTIME;

static LZM_TIMER s_stUSBTimer[USB_TIMERS_MAX];
/*************************************************************
** ��������: USB_AppInitialize
** ��������: 
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void USB_ReadUsbUpdataFlag(void)
{
    EepromPram_ReadPram(E2_USB_UPDATA_FLAG_ID, &s_ucUsbUpdataFlag);
}
/*************************************************************
** ��������: USB_SetUsbUpdataFlag
** ��������: ��ΪU��������ϱ�־
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
*************************************************************/
void USB_SetUsbUpdataFlag(void)
{
    s_ucUsbUpdataFlag = USB_UPDATA_FLAG_VAL;
    EepromPram_WritePram(E2_USB_UPDATA_FLAG_ID, &s_ucUsbUpdataFlag,1);
}
/*************************************************************
** ��������: USB_ClearUsbUpdataFlag
** ��������: ���U��������ϱ�־
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
*************************************************************/
void USB_ClearUsbUpdataFlag(void)
{
    if(USB_UPDATA_FLAG_VAL != s_ucUsbUpdataFlag)return;
    s_ucUsbUpdataFlag = 0xFF-USB_UPDATA_FLAG_VAL;
    EepromPram_WritePram(E2_USB_UPDATA_FLAG_ID, &s_ucUsbUpdataFlag,1);
}
/*************************************************************
** ��������: USB_GetReadUsbUpdataFlag
** ��������: 
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 1:ΪU��������ϱ�־,0:����
*************************************************************/
unsigned char USB_GetReadUsbUpdataFlag(void)
{
   return (USB_UPDATA_FLAG_VAL == s_ucUsbUpdataFlag)?1:0;
}
/*************************************************************
** ��������: USB_AppInitialize
** ��������: 
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void USB_AppInitialize(void)
{
	
    LZM_PublicKillTimerAll(s_stUSBTimer,USB_TIMERS_MAX);
    ///////////////////
    USB_ReadUsbUpdataFlag();
    /////////////////////
    USBH_Init(&USB_OTG_Core, 
            USB_OTG_FS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_cb);
   /////////////////////////
   SetTimerTask(TIME_TASK_USB,LZM_TIME_BASE);
   ///////////////////////////////
   LZM_PublicSetOnceTimer(&s_stUSBTimer[USB_TIMER_TASK],PUBLICSECS(15),USB_ClearUsbUpdataFlag);

}

/*************************************************************
** ��������: Usb_SetCurrentUsbStatus
** ��������: 
** ��ڲ���: 	 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Usb_PromptStatusChange(unsigned char status)
{
    static unsigned char sucUpdataFlag=0;
    switch(status) 
    {
        case USB_WORK_STATUS_INSERT:
            //Public_ShowTextInfo("��⵽U��",100);
            break;
        case USB_WORK_STATUS_ERROR:
            break;
        case USB_WORK_STATUS_ENUM:
            break;
        case USB_WORK_STATUS_READLIST:
            Public_ShowTextInfo("��⵽U��",100);
            break;
        case USB_WORK_STATUS_DEL:
            USB_ClearUsbUpdataFlag();
            if(sucUpdataFlag)
            {
                sucUpdataFlag = 0;
                LZM_PublicSetOnceTimer(&s_stUSBTimer[USB_TIMER_TASK],PUBLICSECS(0.2),NVIC_SystemReset);
            }
            ////////////////////
            Public_ShowTextInfo("U�����Ƴ�",100);
            break;
        case USB_WORK_STATUS_UPDATA:
            //Public_ShowTextInfo("������������",100);
            //NaviLcdShowTempText("������������");
            break;
        case USB_WORK_STATUS_UPDATA_FINISH: 
            USB_SetUsbUpdataFlag();
            Public_ShowTextInfo("���������ɹ�,��γ�U��,�ϵ������ȴ�10��",100);
//            BEEP_ON();
//	    LCD_LIGHT_ON();
            sucUpdataFlag = 1;
            LZM_PublicSetOnceTimer(&s_stUSBTimer[USB_TIMER_TASK],PUBLICSECS(120),NVIC_SystemReset);
            break;
        case USB_WORK_STATUS_UPDATA_ERROR:
            Public_ShowTextInfo("������������,����",100);
            break;
        case USB_WORK_STATUS_PROTECTED:
            Public_ShowTextInfo("U���ܱ���,д����,����",100);
            break;
        case USB_WORK_STATUS_NO_U_DISK:
            Public_ShowTextInfo("���Ȳ���U��,������",100);
            break;
        case USB_WORK_STATUS_READ_FILE_ERROR:
            Public_ShowTextInfo("���ļ�����",100);
            break;
        case USB_WORK_STATUS_START_SAVE:
            Public_ShowTextInfo("���ڱ�������,���Ժ�",100);
//            NaviLcdShowTempText("���ڱ�������,���Ժ�...");
            break;
        case USB_WORK_STATUS_SAVE_FINISH:
            Public_ShowTextInfo("�������",100);
//            NaviLcdShowTempText("�������.");
            break;
        case USB_WORK_STATUS_SAVE_ERROR:
            Public_ShowTextInfo("�����ļ�����",100);
   //         NaviLcdShowTempText("�����ļ�����");
            break;
        default:
            break;
    }
}
/*************************************************************
** ��������: Usb_SetCurrentUsbStatus
** ��������: 
** ��ڲ���: 	 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Usb_SetCurrentUsbStatus(unsigned char status)
{
    Usb_PromptStatusChange(status);
}
/*************************************************************
** ��������: Usb_FindFileAndSaveToUdisk
** ��������: 
** ��ڲ���: 	 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Usb_FindSDcardFileAndSaveToUdisk(unsigned char type,unsigned char *startTime,unsigned char *endTime)
{
    unsigned char folder,folder1;
    unsigned char buffer1[128];
    unsigned char buffer2[128];  
    TIME_T *startT,*endT;
    unsigned long val,val1;
    
    ///////////////////////////////////
    if(!USB_USR_CheckInsertUsbDisk())
    {
        Usb_SetCurrentUsbStatus(USB_WORK_STATUS_NO_U_DISK);
        return 0;
    }
    ///////////
    memset(buffer1,0,128);
    memset(buffer2,0,128);
    /////////////
    val=ConverseGmtime((TIME_T *)startTime);
    val1=ConverseGmtime((TIME_T *)endTime);
    if(val<val1)
    {
        startT=(TIME_T*)startTime;
        endT=(TIME_T*)endTime;
    }
    else
    {
        startT=(TIME_T*)endTime;
        endT=(TIME_T*)startTime;
    }
    /////////////////////////////////
//    MediaSearch_Find(&folder,&folder1,buffer1,buffer2,type,startT,endT);
		startT = endT;//������Ϊ���벻����������,dxl,2015.8.21
		endT = startT;
    /////////////////////////////////
    if((0==folder)&&(0==folder1))
    {
        Public_ShowTextInfo("���������",100);
        return 0;
    }
    ////////////////////////
    if(folder==folder1)folder1 = 0;
    ////////////////////
    Usb_SetCurrentUsbStatus(USB_WORK_STATUS_START_SAVE);
    //////////////////////////////
    /////////////////////
    USB_USR_WriterDataToUsb(folder,folder1,buffer1,buffer2,type);
    
		
    return 1;
}
/*************************************************************
** ��������: Usb_AppTimeTask
** ��������: 
** ��ڲ���: 	 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState Usb_AppTimeTask(void)
{   
	
    USBH_Process(&USB_OTG_Core, &USB_Host);
    LZM_PublicTimerHandler(s_stUSBTimer,USB_TIMERS_MAX);
	
    return ENABLE;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
