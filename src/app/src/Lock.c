/*******************************************************************************
 * File Name:           Lock.c 
 * Function Describe:   
 * Relate Module:       ʹ��ǰ����
 * Writer:              fanqinghai
 * Date:                2015-11-14 
 * Date:                //ʹ��ǰ����һ���ܿ������ն��ϵ��ȼ��2��������ַ�Ƿ�Ϊ�趨��������jt1.gghypt.net,jt2.gghypt.net
                        //��������ն����������������
                        //ʹ��ǰ���������ܿ������ն��ϵ��ȼ��2��������ַ�Ƿ�Ϊ�趨��������jt1.gghypt.net,jt2.gghypt.net
                        //��������˳������������ٿ����ȴ�״̬���û��ڵ���������������Ӧ�Ĳ������ն˿�ʼ���ٿ���
*******************************************************************************/



#include "include.h"


static u8 Lock_flag =0;

/*******************************************************************************
* Function Name  : Lock_Before_Use_TimeTask(void)
* Description    : ʹ��ǰ����һ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

FunctionalState LockBeforeUseOne_TimeTask(void)
{
   // u8 MainIp[]="jt1.gghypt.net";
    u8 MainIp[]="jt1.gghypt.net";
    u8 BackIp[]="jt2.gghypt.net";
    u8 fBuffer[2];
    u8 eBuffer[2];
    u8 val;
    u8 MainIPbuf[16];
    u8 BackIpbuf[16];
    static u8 FirstUse =0;

    EepromPram_ReadPram(E2_USE_BEFOR_USE_ONE_ID,eBuffer);
    FRAM_BufferRead(fBuffer,FRAM_LOCK_ONE_FLAG_BACKUP_ADDR_LEN,FRAM_LOCK_ONE_FLAG_BACKUP_ADDR);
    if((eBuffer[0] == 0xaa) && (fBuffer[0] == 0xaa))
    {
        
        EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,MainIPbuf);
        EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID,BackIpbuf);
            val=strlen((char*)MainIp);
            if((strncmp((char*)MainIp,(char*)MainIPbuf,val)==0)&&(strncmp((char*)BackIp,(char*)BackIpbuf,val)==0))
            {

               //LcdClearScreen();
               if(Lock_flag)
               {
                   Lock_flag = 0;
                   LcdClearScreen();
                   LZM_KillTimer(TEST_TIMER);
                   LZM_SetTimer(REFRESH_TIMER,SECS(1));
                   LcdClearScreen();
                   Lcd_ClearDisplayCtrlFlag();
                   Lcd_SetMainRedrawFlag();
                   Lcd_MainInterface();
                   Communication_Open();
               }
						   SetTimerTask(LOCK_TWO,SECOND);   

               return DISABLE ;
               
            }
            else
            {
                MOD_CloseMainIp();//ģ�������Ͽ�����
                if(!FirstUse)
                {
                    FirstUse =1;
                    Lock_flag = 1 ;
                    LcdClearScreen();
                    LcdShowCaption("ʹ��ǰ����һ",24);
                    ClrTimerTask(TIME_MENU);    
                  //  MOD_CloseMainIp();//ģ�������Ͽ�����
                }
                return ENABLE ;
            }
    }
    if(Lock_flag)
    {
        Lock_flag = 0;
        LcdClearScreen();
        LZM_KillTimer(TEST_TIMER);
        LZM_SetTimer(REFRESH_TIMER,SECS(1));
        LcdClearScreen();
        Lcd_ClearDisplayCtrlFlag();
        Lcd_SetMainRedrawFlag();
        Lcd_MainInterface();
        Communication_Open();
    }
        SetTimerTask(TIME_MENU,1);   
     return ENABLE ;
}
/*******************************************************************************
* Function Name  : Lock_Before_Use_Two_TimeTask(void)
* Description    : ʹ��ǰ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState LockBeforeUseTwo_TimeTask(void)
{
  //  u8 MainIp[]="121.15.212.249";
    u8 MainIp[]="jt1.gghypt.net";
    u8 BackIp[]="jt2.gghypt.net";
    u8 val;
    u8 MainIPbuf[16];
    u8 BackIpbuf[16];
    static u8 FirstUse =0;
    u8 fBuffer[2];
    u8 eBuffer[2];
    EepromPram_ReadPram(E2_USE_BEFOR_USE_TWO_ID,eBuffer);
    FRAM_BufferRead(fBuffer,FRAM_LOCK_TWO_FLAG_BACKUP_ADDR_LEN,FRAM_LOCK_TWO_FLAG_BACKUP_ADDR);
    if((eBuffer[0] == 0xaa) && (fBuffer[0] == 0xaa))
    {

            EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,MainIPbuf);
            EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID,BackIpbuf);
            val=strlen((char*)MainIp);
            if((strncmp((char*)MainIp,(char*)MainIPbuf,val)==0)&&(strncmp((char*)BackIp,(char*)BackIpbuf,val)==0))
            {

                if(!FirstUse)
                {
                    FirstUse = 1 ;
                    Lock_flag = 1 ;
                   // MOD_CloseMainIp();              //ģ�������Ͽ�����
                    LcdClearScreen();
                    LcdShowCaption("ʹ��ǰ������",24);
                    ClrTimerTask(TIME_MENU);        
                  //  SetTimerTask(TIME_ACCOUNT,SECOND);          
                    OpenAccount_ClearLockStatus();       //�������ܣ��ȴ��û��ֶ���������
                    
                }
                /*��鿪������*/
                    if(!Check_First_Register_Flag())    //��һ�ο���
                    {
                        OpenAccount_SetLockStatus();   //�򿪿��ٿ�������
                       // NaviLcd_DisposeQueryWorkStatus(); //LCD�򿪿�������

                    }
               // Communication_Open();
                return ENABLE ;
               
            }
            else   
            {
               FirstUse = 0;
               Clear_First_Register_Flag();
               if(Lock_flag)
               {
                  Lock_flag = 0;
                  LcdClearScreen();
                  Communication_Open();
                  LZM_KillTimer(TEST_TIMER);
                  LZM_SetTimer(REFRESH_TIMER,SECS(1));
                  LcdClearScreen();
                  Lcd_ClearDisplayCtrlFlag();
                  Lcd_SetMainRedrawFlag();
                  Lcd_MainInterface();
                             }
                  SetTimerTask(TIME_MENU,1);   
                             return ENABLE ;
            }
    
    }
    FirstUse = 0;
    Clear_First_Register_Flag();
    if(Lock_flag)
    {
        Lock_flag = 0;
        LcdClearScreen();
        LZM_KillTimer(TEST_TIMER);
        LZM_SetTimer(REFRESH_TIMER,SECS(1));
        LcdClearScreen();
        Lcd_ClearDisplayCtrlFlag();
        Lcd_SetMainRedrawFlag();
        Lcd_MainInterface();
        Communication_Open();
    }
        SetTimerTask(TIME_MENU,1);   
        return ENABLE ;

}
/*******************************************************************************
* Function Name  : Check_Lock_Two_State(void)
* Description    : ���������������־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 Check_Lock_Two_State(void)
{

    u8 fBuffer[2];
    u8 eBuffer[2];
    EepromPram_ReadPram(E2_USE_BEFOR_USE_TWO_ID,eBuffer);
    FRAM_BufferRead(fBuffer,FRAM_LOCK_TWO_FLAG_BACKUP_ADDR_LEN,FRAM_LOCK_TWO_FLAG_BACKUP_ADDR);
    if((eBuffer[0] == 0xaa) && (fBuffer[0] == 0xaa))
    {
        return 1;
    }
    else
    {
        return 0 ;
    }
}

/*******************************************************************************
* Function Name  : Clear_First_Register_Flag(void)
* Description    : ����ע���־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Clear_First_Register_Flag(void)
{
    u8 Buffer[2];

    Buffer[0] = 0x00;   
    FRAM_BufferWrite(FRAM_QUICK_ACCOUNT_ADDR,Buffer, FRAM_QUICK_ACCOUNT_ADDR_LEN);
 
}
/*******************************************************************************
* Function Name  : Set_First_Register_Flag(void)
* Description    : ����ע���־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Set_First_Register_Flag(void)
{
    u8 Buffer[2];

    Buffer[0] = 0xaa;   
    FRAM_BufferWrite(FRAM_QUICK_ACCOUNT_ADDR,Buffer, FRAM_QUICK_ACCOUNT_ADDR_LEN);

}
/*******************************************************************************
* Function Name  : Check_First_Register_Flag(void)
* Description    : ��鿪����ɱ�־
* Input          : None
* Output         : None
* Return         : 1:������� 0:δ���
*******************************************************************************/
u8 Check_First_Register_Flag(void)
{
    u8 Buffer[2];

    Buffer[0] = 0xff;   
    FRAM_BufferRead(Buffer,FRAM_QUICK_ACCOUNT_ADDR_LEN,FRAM_QUICK_ACCOUNT_ADDR);

    if(!Buffer[0])
    {
        return 0;
    }
    else if(0xaa == Buffer[0])
    {
        return 1;
    }
        return 1;
}
/*******************************************************************************
* Function Name  : BackUp_OpenLockOneDataFlag(void)
* Description    : ��������һ������־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BackUp_OpenLockOneDataFlag(u32 PramID,u8 *pBuffer)
{
    u8 *Buffer;
    
    Buffer = pBuffer ;
    switch(PramID)
    {
            case 0xf28b :
            FRAM_BufferWrite(FRAM_LOCK_ONE_FLAG_BACKUP_ADDR,Buffer, FRAM_LOCK_ONE_FLAG_BACKUP_ADDR_LEN);
            break ;
            case 0xf28c :
            FRAM_BufferWrite(FRAM_LOCK_TWO_FLAG_BACKUP_ADDR,Buffer, FRAM_LOCK_TWO_FLAG_BACKUP_ADDR_LEN);
            break;
            default :break ;

    }

}





