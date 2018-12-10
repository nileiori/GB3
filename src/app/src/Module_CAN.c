/*************************************************************************
* Copyright (c) 2013,�������������¼����������޹�˾
* All rights reserved.
* �ļ����� : Module_CAN.c
* ��ǰ�汾 : 1.0
* ������   : Shigle
* �޸�ʱ�� : 2013��6��10��

* 2013��9��18��Ϊ���߰汾1.0, �������������������޸ģ���Ҫ������׷��˵��
* 1����ʱֻ������CAN1�Ĺ��ܣ�CAN2 �Ĺ��������� ���Ҫ�����������ļ�Module_CAN.h���޸ĺ� #define CAN_CH_MAX 1 //CANͨ������� 1:ֻ����CAN1 2:ͬʱ����CAN1��CAN2
* 2�����ǵ��ڴ�ռ����ƣ�Ŀǰ�ϱ�CAN����ͬʱ����ϱ����ڻ����δ�ϴ�����Ч�������ܺ�CAN_MAX_RECE_NUM���ƣ���
* 3��������Ҫ����808-2012��Э��������ϴ���δ���������������ͻ�Э��
*************************************************************************/

/***  �����ļ�  ***/ 

#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "modem_lib.h"
#include "modem_app_com.h"

/***  �ⲿ ���ú���  ***/

extern void RTC_GetCurTime(TIME_T *tt);


/***  ���� �����ȫ�ֱ���  ***/


CAN_REC  CanData;
PARAMETER_CAN   gCANParame[CAN_CH_MAX];
PARAMETER_CAN_SINGLE  gCANParameSingle;

u8  gIsCanOpen;

CAN_ISR_DATA    gCanIsrData[CAN_CH_MAX];
CAN_MASK_ID     gFilterIDArray[CAN_CH_MAX];

u16   gCANNum_Radio[CAN_CH_MAX];
u16    gCANNum_Rx[CAN_CH_MAX];


/********************************************************************
* ���� : CAN_PowerCtrl_Init_GPIO
* ���� : ��ʼ�� CAN�������Ӧ��IO��.
********************************************************************/

void CAN_PowerCtrl_Init_GPIO( void )
{
  
  GPIO_InitTypeDef   GPIO_InitStructure;
  
  //-----------------CAN��Դ����-----------------Weite
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
}



/********************************************************************
* ���� : DecodeCan_EvTask
* ���� : ����CAN���յ����ݰ�
********************************************************************/

void DecodeCan_EvTask()
{
  
}


/*
*******************************************************************
* ���� : Time_Now_Get_BCD
* ���� : ��ȡ��ǰ��RTCʱ��,ת����Э��Ҫ���BCD��ʽ
* ��� : ����BCDʱ���ָ��
*******************************************************************
*/
void Time_Now_Get_BCD( u8 * pTimer )
{
 
   TIME_T nowTime;
   RTC_GetCurTime(&nowTime);
   
   *(pTimer +0)=( ( nowTime.year/10 )<<4 )  + (( nowTime.year  %10)  &0x0F )            ; //��:00~99
   *(pTimer +1)=( ( nowTime.month/10)<<4 )  + (( nowTime.month %10)  &0x0F )            ; //��:1~12
   *(pTimer +2)=( ( nowTime.day/10  )<<4 )  + (( nowTime.day   %10)  &0x0F )            ; //��:1~31
   *(pTimer +3)=( ( nowTime.hour/10 )<<4 )  + (( nowTime.hour  %10)  &0x0F )            ; //ʱ:0~23
   *(pTimer +4)=( ( nowTime.min/10  )<<4 )  + (( nowTime.min   %10)  &0x0F )            ; //��:0~59
   *(pTimer +5)=( ( nowTime.sec/10  )<<4 )  + (( nowTime.sec   %10)  &0x0F )            ; //��:0~59
                
}


/*
*******************************************************************
* ���� : CAN_Isr_Rx
* ���� : �ɼ�CAN�����жϵ�����
* ���� :  * CANx  : STEM32 CAN�ṹ��ָ��
*******************************************************************
*/

void CAN_Isr_Rx( CAN_TypeDef* CANx )
{
  u8 NowTime[6];
  CanRxMsg  RxMessage;
  u8  num;
  u8  iCANCh ;
        
  if( CANx == CAN1 )
  {
    CAN_Receive( CANx, CAN_FIFO0, &RxMessage);
    iCANCh = iCAN1 ;
  }
  else
  {
    CAN_Receive( CANx, CAN_FIFO1, &RxMessage);
    iCANCh = iCAN2 ;
  }
  
  num = gCanIsrData[iCANCh].write;
 
  Time_Now_Get_BCD(  NowTime );
  gCanIsrData[iCANCh].receBuffer[num].CAN_Time[0] = NowTime[3]; //ʱ:0~23
  gCanIsrData[iCANCh].receBuffer[num].CAN_Time[1] = NowTime[4]; //��:0~59
  gCanIsrData[iCANCh].receBuffer[num].CAN_Time[2] = NowTime[5]; //��:0~59

  
  if(RxMessage.IDE == CAN_ID_STD)
  {
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_3 =0;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_2 = 0;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_1 = (RxMessage.StdId>>8)&0x07;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_0 = RxMessage.StdId;       

    memcpy((u8*)&gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_0, (u8*)(&RxMessage.StdId), 4);
    
    gCanIsrData[iCANCh].receBuffer[num].CAN_Frame_Type = 0; //bit 30: ��ʾ֡����: 0:��׼֡ 1:��չ֡
  }
  else
  {
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_3 =(RxMessage.ExtId>>24)&0x1F;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_2 = RxMessage.ExtId>>16;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_1 = RxMessage.ExtId>>8;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_0 = RxMessage.ExtId;    

 
  //  memcpy((u8*)&gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_0, (u8*)(&RxMessage.ExtId), 4);
    gCanIsrData[iCANCh].receBuffer[num].CAN_Frame_Type = 1; //bit 30: ��ʾ֡����: 0:��׼֡ 1:��չ֡
    
  }
  
   gCanIsrData[iCANCh].receBuffer[num].CAN_Data_Original = 0; //bit 29: ��ʾ���ݲɼ���ʽ: 0:ԭʼ���� 1: �ɼ�����ļ���ֵ
        

   gCanIsrData[iCANCh].receBuffer[num].CAN_Channel = iCANCh; //bit31: ��ʾCANͨ����, 0: CAN1 1: CAN2
  
   memcpy((u8*)&gCanIsrData[iCANCh].receBuffer[num].CAN_Data[0], RxMessage.Data, RxMessage.DLC );
  
    
    gCanIsrData[iCANCh].write++;
    gCanIsrData[iCANCh].write = gCanIsrData[iCANCh].write % CAN_MAX_RECE_NUM;
    
#if 0
    if(gCanIsrData[iCANCh].receNum >= CAN_MAX_RECE_NUM)
    {
        gCanIsrData[iCANCh].receNum = CAN_MAX_RECE_NUM;
        gCanIsrData[iCANCh].UnreadIndex = gCanIsrData[iCANCh].receIndex;
    }
    else
    {
        gCanIsrData[iCANCh].UnreadIndex = 0;
    }
#endif
    
}


/*******************************************************************
* ���� : CAN1_Sample_Sw
* ���� : CAN ��������
* ���� : 
*       iCANChannel:    0: CAN1ͨ��   1: CAN2ͨ��
*            bSwCtr:    0: �ر�CAN�����ж�  1: ����CAN1�����ж�   
*******************************************************************/
void CAN_Sample_Sw( u8 iCANChannel , u8 bSwCtr )
{
  
  if( iCAN1 == iCANChannel ){
    
      if(  bSwCtr ){
        
            CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//��CAN1�Ľ����ж�
        } else {
          
          CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);//�ر�CAN1�Ľ����ж� 
        } 
      
  } else if( iCAN2 == iCANChannel ) {

       if(  bSwCtr ){
         
            CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);//��CAN2�Ľ����ж�
            
        } else {
          
            CAN_ITConfig(CAN2, CAN_IT_FMP1, DISABLE);//�ر�CAN2�Ľ����ж�
        }    
     }
  
  
}
  

/*******************************************************************
* ���� : CAN_Packet_Single_ID
* ���� : ���CAN����
* ���� : 
*       iCANCh:    0: CAN1ͨ��   1: CAN2ͨ��
* ��� :
*        pdata:    ���հ����ݵ�ָ��
*******************************************************************/
/*
u16 CAN_Packet_Single_ID( u8 iCANCh,  u8 *pdata )
{
  
  u8  readindex;
  u16 retlenght = 0 ;
  u8 Flag_First_Valid_CANPacket = 0 ;
  u16 CAN_Item_TotalNum = 0;
  u16 CAN_Item_TotalNum_ID0 = 0;
  u8  *dataAddr;
  u8  k ;

  //static u8 TempAA[15];
  
  assert_param((pdata != NULL));
  assert_param(( iCANCh < CAN_CH_MAX ));  

  dataAddr  =  pdata+2;//�������ֽ������ݰ� ��

  //���յ�CAN�������������
  
#if DEBUG_PRINT_CAN
  
      Print_RTC();
      LOG_PR( "CAN%d : receNum = %d ; readindex = %d ; " , iCANCh+1 , gCanIsrData[iCANCh].receNum, gCanIsrData[iCANCh].UnreadIndex  ); 
  //    LOG_PR_N( "\r\n");
#endif    
                
  
  Flag_First_Valid_CANPacket = 0;
  CAN_Item_TotalNum = gCanIsrData[iCANCh].receNum ;
  gCanIsrData[iCANCh].receNum =0;
  while( ( CAN_Item_TotalNum > 0 )&&( CAN_Item_TotalNum_ID0 < 50) )
  {
      readindex = gCanIsrData[iCANCh].UnreadIndex; //��δ����������ʼ��ȡCAN���ݰ�����
    
      for( k = 0; k < MAX_NUM_FILTER_ID ; k++ )
      {
        
        if(( !gCANParameSingle.CAN_Sample_Single[ k ].CAN_Sample_Interval )&&
           ( gCANParameSingle.CAN_Sample_Single[ k ].CAN_ID_BYTE_3 ==  gCanIsrData[0].receBuffer[readindex].CAN_ID_BYTE_0 )&& //����֡ID
           ( gCANParameSingle.CAN_Sample_Single[ k ].CAN_ID_BYTE_2 == gCanIsrData[0].receBuffer[readindex].CAN_ID_BYTE_1 )&&
           ( gCANParameSingle.CAN_Sample_Single[ k ].CAN_ID_BYTE_1 == gCanIsrData[0].receBuffer[readindex].CAN_ID_BYTE_2 )&&
           ( gCANParameSingle.CAN_Sample_Single[ k ].CAN_ID_BYTE_0 == gCanIsrData[0].receBuffer[readindex].CAN_ID_BYTE_3 ))
        {
              break; //һ�������������׵ģ�������������ѭ��
        } 
      }
      
      if( k !=  MAX_NUM_FILTER_ID  )
      {
            gCanIsrData[iCANCh].UnreadIndex = ( gCanIsrData[iCANCh].UnreadIndex +1)%CAN_MAX_RECE_NUM ; ////δ��ȡ�������ƶ�һ��
            CAN_Item_TotalNum--;
      }
      else
      {
             
          //��һ��CAN���ݽ��յ�ʱ��
          if( ! Flag_First_Valid_CANPacket)
          {
            
            Flag_First_Valid_CANPacket = 1;
            
            *dataAddr++ =   gCanIsrData[0].receBuffer[readindex].CAN_Time[0] ;
            *dataAddr++ =   gCanIsrData[0].receBuffer[readindex].CAN_Time[1] ;
            *dataAddr++ =   gCanIsrData[0].receBuffer[readindex].CAN_Time[2] ;
            *dataAddr++ =   gCanIsrData[0].receBuffer[readindex].CAN_Time[3] ; 
            *dataAddr++ =   gCanIsrData[0].receBuffer[readindex].CAN_Time[4] ;
            retlenght +=5;
           }
          
          memcpy( dataAddr, (u8*)&gCanIsrData[0].receBuffer[readindex], sizeof(CAN_ITEM)- 5 );//readindex, cpyLen,

          CAN_Item_TotalNum_ID0++ ; //���������++
          dataAddr += (sizeof(CAN_ITEM)-5); //�ֽڵ���
          retlenght += (sizeof(CAN_ITEM)- 5) ; //���ݳ�������
          gCANNum_Radio[iCANCh]++ ;  //ͳ�Ʒ��͵�CAN������
          
          gCanIsrData[iCANCh].UnreadIndex = (gCanIsrData[iCANCh].UnreadIndex+1)%CAN_MAX_RECE_NUM ; ////δ��ȡ�������ƶ�һ��
          CAN_Item_TotalNum--;
          
      }
      
  }
  
  gCanIsrData[iCANCh].receNum += CAN_Item_TotalNum ; //��δ����� ��Ŀ�� �����ۼ�
  
#if DEBUG_PRINT_CAN
  
   //   RTC_Print();
      LOG_PR( " CurItemNum = %d ; RadioSum = %d ; RxSum = %d ; " , CAN_Item_TotalNum_ID0,  gCANNum_Radio[iCANCh] ,  gCANNum_Rx[iCANCh]); 
      LOG_PR_N( "\r\n");
#endif    
               
  

  //���  ��������� ������
  if( CAN_Item_TotalNum_ID0)
  {
    
    dataAddr  =  pdata;//���ص����ֽ������ݰ� ��
    *dataAddr++ = CAN_Item_TotalNum_ID0>>8;
    *dataAddr++ = CAN_Item_TotalNum_ID0&0x0FF ;
    retlenght +=2;
  } 
  else 
  {
       retlenght = 0 ;
  }
  

  return( retlenght );
        
}
*/

/*******************************************************************
* ���� : Get_Buff_Item_Length
* ���� : ��û��λ��������ݳ���
* ���� : 
*       
* ��� : ��������can����֡����
*        
*******************************************************************/
u8 Get_Buff_Item_Length()
{
u8 CAN_Item_TotalNum;
    if(gCanIsrData[0].write  > gCanIsrData[0].read)
    {
        CAN_Item_TotalNum = gCanIsrData[0].write  - gCanIsrData[0].read ;
    }
    else if(gCanIsrData[0].write  == gCanIsrData[0].read)
    {
        CAN_Item_TotalNum = 0;
    }
    else
    {
        CAN_Item_TotalNum = (CAN_MAX_RECE_NUM - gCanIsrData[0].read) + gCanIsrData[0].write ;
    }

return CAN_Item_TotalNum;
}
/*******************************************************************
* ���� : Check_Filter_Frame
* ���� : �ҳ����˵������ݶ���
* ���� : 
*       iCANCh:    0: CAN1ͨ��   1: CAN2ͨ��
* ��� :0�޹���֡��1:���ֹ���֡
*       
*******************************************************************/
u8 Check_Filter_Frame(u8 iCANCh)
{

    u8 i =0;
    
    for( i = 0; i < MAX_NUM_FILTER_ID ; i++ )
    {
      
      if(( !gCANParameSingle.CAN_Sample_Single[ i ].CAN_Sample_Interval )&&
         ( gCANParameSingle.CAN_Sample_Single[ i ].CAN_ID_BYTE_3 ==  gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read].CAN_ID_BYTE_0 )&& //����֡ID
         ( gCANParameSingle.CAN_Sample_Single[ i ].CAN_ID_BYTE_2 == gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read].CAN_ID_BYTE_1 )&&
         ( gCANParameSingle.CAN_Sample_Single[ i ].CAN_ID_BYTE_1 == gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read].CAN_ID_BYTE_2 )&&
         ( gCANParameSingle.CAN_Sample_Single[ i ].CAN_ID_BYTE_0 == gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read].CAN_ID_BYTE_3 ))
      {
        return  1;
      } 
    }
    
    return 0;


}

/*******************************************************************
* ���� : Package_Can_Data
* ���� : ���CAN����
* ���� : 
*       iCANCh:    0: CAN1ͨ��   1: CAN2ͨ��
* ��� :
*        pdata:    ���հ����ݵ�ָ��
*******************************************************************/
u16 Package_Can_Data( u8 iCANCh,  u8 *pdata )

{
    u16 retlength = 0 ;
    u8  *dataAddr;   
    u8 First_Fram = 0 ;
    assert_param((pdata != NULL));
    assert_param(( iCANCh < CAN_CH_MAX ));  
    
    dataAddr = pdata+2 ;

    
    if( (gCanIsrData[iCANCh].read == gCanIsrData[iCANCh].write ) )  //����Ϊ��
    {
        return 0;
    }
    else 
    {
    u8 Packet_Nmb =0;
        while(Get_Buff_Item_Length()&& (Packet_Nmb < CAN_MAX_SEND_NMB))
        {


            if(! First_Fram)    //��һ��֡ǰ��ʱ��
            {
                First_Fram = 1;
                *dataAddr++ =   gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read ].CAN_Time[0] ;
                *dataAddr++ =   gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read ].CAN_Time[1] ;
                *dataAddr++ =   gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read ].CAN_Time[2] ;
                *dataAddr++ =   gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read ].CAN_Time[3] ; 
                *dataAddr++ =   gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read ].CAN_Time[4] ;
                retlength +=5;
            }

            if(!Check_Filter_Frame(iCANCh))
            {
                memcpy( dataAddr, (u8*)&gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read ], sizeof(CAN_ITEM)- 5 );
                dataAddr += (sizeof(CAN_ITEM)-5); //�ֽڵ���
                retlength += (sizeof(CAN_ITEM)- 5) ; //���ݳ�������
                Packet_Nmb ++;

            }

            gCanIsrData[iCANCh].read ++ ;
            gCanIsrData[iCANCh].read = gCanIsrData[iCANCh].read % CAN_MAX_RECE_NUM;
               
            
            
        }

        dataAddr = pdata ;
        *dataAddr++ = Packet_Nmb>>8;
        *dataAddr++ = Packet_Nmb&0x0FF ;
        retlength +=2;


    }
    return retlength ;

}
/*******************************************************************
* ���� : ����ID0x0110~0x01FF  ����ID �����ɼ���������
* ���� : CAN ����ID �����ɼ�����
* ���� :  CANParmamerID : CAN����ID  Ŀǰֻ��ȡֵ0x0110�� 0x0111
*******************************************************************/
void UpdataOnePram_Sample_Single(  u32 CANParmamerID  )
{
  
  u8 BufferTemp[10];
  u32 Temp32;
//  u8 iCANChannel;
  u32 TempID ;
  u8 iItem;
  
  
#if 1 //����ʱֱ���޸Ĳ���
  
  u8 CANParam0[] ={ 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 , 0xcc };
  u8 CANParam1[] ={ 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 , 0xcd };

  EepromPram_WritePram( E2_CAN_SET_ONLY_GATHER_0_ID , CANParam0,  sizeof(CANParam0) );  
  EepromPram_WritePram( E2_CAN_SET_ONLY_GATHER_1_ID ,  CANParam1,  sizeof(CANParam1) );  
#endif
  
  
  
    //����������
  if( 
      ( E2_CAN_SET_ONLY_GATHER_0_ID != CANParmamerID )&&
      ( E2_CAN_SET_ONLY_GATHER_1_ID != CANParmamerID )
     ) return;
  
  
    iItem = CANParmamerID - E2_CAN_SET_ONLY_GATHER_0_ID ;

  if(  E2_CAN_SET_ONLY_GATHER_LEN == EepromPram_ReadPram( CANParmamerID, BufferTemp  )    )
 // if( E2_CAN_SET_ONLY_GATHER_LEN ==  EepromPram_ReadPram(E2_CAN_SET_ONLY_GATHER_ID, (u8 *)&gCANParame[iCAN1].CAN_Sample_Single) )
  {
    
    
    
      //�ҳ�������ĸ�ͨ����CAN 
      //bit31: ��ʾCANͨ����, 0: CAN1 1: CAN2
#if 0
      if( BufferTemp[4]&0x80 ){
        
          iCANChannel = 1 ;
          
      } else {

          iCANChannel = 0 ;
      }
#endif
      
      
     //Bit28~Bit0 ��ʾCAN����ID
      
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_0 = BufferTemp[4]&0x1F;
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_1 = BufferTemp[5];
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_2 = BufferTemp[6];
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_3 = BufferTemp[7]; 
    
    
   //bit 29: ��ʾ���ݲɼ���ʽ: 0:ԭʼ���� 1: �ɼ�����ļ���ֵ 
    
    if(  BufferTemp[4]&0x20 ){
      
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Data_Original   = 1; 
    
  } else {
    
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Data_Original   = 0 ;
    
  }
  
  
  //bit 30: ��ʾ֡����: 0:��׼֡ 1:��չ֡
  
  if( BufferTemp[4]&0x40 ){
    
      gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Frame_Type      = 1 ;
      
  } else {
    
      gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Frame_Type     = 0 ;
  }

  
  //Bit63~Bit32 ��ʾ��ID�����ɼ�ʱ����(ms), 0 ��ʾ���ɼ� 
  
    Temp32  =  BufferTemp[3];
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Sample_Interval =  Temp32 ;
    
    Temp32  =  BufferTemp[2];
    Temp32 = Temp32<<8 ;
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Sample_Interval +=  Temp32 ;
    
    Temp32  =  BufferTemp[1];
    Temp32 = Temp32<<16 ;
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Sample_Interval +=  Temp32 ;

    Temp32  =  BufferTemp[0];
    Temp32 = Temp32<<24 ;
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Sample_Interval +=  Temp32 ;

    
    //���ݲɼ�ʱ���� ������Ӧͨ���� �����ж�
  //          CAN_Sample_Sw( 0 ,  1) ;  
            
#if 0
    if( gCANParame[iCANChannel].CAN_Sample_Single[ gCANParame[iCANChannel].CAN_Sample_Single_Num ].CAN_Sample_Interval ){
      
        CAN_Sample_Sw( iCANChannel ,  1) ;  
        
    } else {
      
        CAN_Sample_Sw( iCANChannel ,  0);
    }
#endif
    //===========================================================================
    gFilterIDArray[0].iMaskID = 1 ;
    
    TempID = gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_0;
    TempID <<= 24;
    gFilterIDArray[0].MaskID = TempID;
    
    TempID = gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_1;
    TempID <<= 16;
    gFilterIDArray[0].MaskID += TempID;
    
    TempID = gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_2;
    TempID <<= 8;
    gFilterIDArray[0].MaskID += TempID;
    
    TempID = gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_3;
    TempID <<= 0;
    gFilterIDArray[0].MaskID += TempID;

        
 //=======================================================================
#if 0   
    gCANParame[iCANChannel].CAN_Filter_ID_Index++ ;
    
    if( iCANChannel == 0){
      
  //    CAN1_ID_Filter_Set(  gCANParame[iCANChannel].CAN_Filter_ID_Index , gFilterIDArray[gCANParame[iCANChannel].CAN_Filter_ID_Index].MaskID , 0xFFFFFFFF );
      
    } else {
      
 //     CAN2_ID_Filter_Set(  gCANParame[iCANChannel].CAN_Filter_ID_Index , gFilterIDArray[gCANParame[iCANChannel].CAN_Filter_ID_Index].MaskID , 0xFFFFFFFF );
    }
      
#endif

    
 //=======================================================================   

  } else {
    
#if 0
        if( E2_CAN_SET_ONLY_GATHER_0_ID == CANParmamerID ){
          
              CAN1_ID_Filter_Set(  gCANParame[iCANChannel].CAN_Filter_ID_Index , 0x00000000, 0x00000000 );
              
        } else if ( E2_CAN_SET_ONLY_GATHER_1_ID == CANParmamerID ){
          
              CAN2_ID_Filter_Set(  gCANParame[iCANChannel].CAN_Filter_ID_Index , 0x00000000, 0x00000000 );
        }
#endif
        
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_0 = 0;
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_1 = 0;
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_2 = 0;
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_3 = 0;     // : 4;     //28�ֽ�CAN����ID
    
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Data_Original = 0; // :1; //bit 29: ��ʾ���ݲɼ���ʽ: 0:ԭʼ���� 1: �ɼ�����ļ���ֵ
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Frame_Type = 1; //: 1; //bit 30: ��ʾ֡����: 0:��׼֡ 1:��չ֡
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Channel =0 ; //: 1; //bit31: ��ʾCANͨ����, 0: CAN1 1: CAN2
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Sample_Interval = 15L*1000L ;

    }

  //======================��ӡ�����־ ===========================================
  
#if DEBUG_PRINT_CAN
  
    LOG_PR_N( "\r\n" );
  
    Print_RTC();
  
    LOG_PR( " Num = %d \r\n", gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Channel );
    
    LOG_PR_N( "====================================== \r\n" );

  //bit31: ��ʾCANͨ����, 0: CAN1 1: CAN2
    if( gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Channel ){
      
      LOG_PR_N( ">> CAN Channel: CAN2 \r\n" );
      
    } else {

      LOG_PR_N( ">> CAN Channel: CAN1 \r\n" );
    }
    
      
  //bit 30: ��ʾ֡����: 0:��׼֡ 1:��չ֡
      if( gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Frame_Type ){
        
        LOG_PR_N( ">> FrameType: Extend \r\n" );
        
      } else {
        
         LOG_PR_N( ">> FrameType: Standard \r\n" );
         
      }
      
    //bit 29: ��ʾ���ݲɼ���ʽ: 0:ԭʼ���� 1: �ɼ�����ļ���ֵ 
      
      if( gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Data_Original ){
        
       LOG_PR_N( ">> Data Sample Method: CalcValue\r\n" );
              
      } else {
        
        LOG_PR_N( ">> Data Sample Method: OriginalValue\r\n" );
        
      }
     

    LOG_PR( ">> Sample Internval = %d (ms)\r\n" ,  gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Sample_Interval  );

      
     LOG_PR( ">> ID(Hex) = %02x %02x %02x %02x\r\n" ,
            gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_0 ,
            gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_1 ,
            gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_2 ,
            gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_3
      );
     
     
LOG_PR_N( "\r\n" );
    
    
    

#endif   
   
  
   
    gCANParameSingle.CAN_Sample_Single_Num = (gCANParameSingle.CAN_Sample_Single_Num + 1 )%MAX_NUM_FILTER_ID ; 
   
  
  
}

/********************************************************************
* ���� : UpdataOnePram_SamplePeriod_CAN1
* ���� : CAN����ͨ��1 �ɼ�ʱ��������ˢ��
* ˵�� : ����ID0x0100  CAN����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ� 
********************************************************************/
void UpdataOnePram_SamplePeriod_CAN1( void )
{

  u8 BufferTemp[6];
  if( E2_CAN1_GATHER_TIME_LEN == EepromPram_ReadPram( E2_CAN1_GATHER_TIME_ID , BufferTemp ) )
  {
      gCANParame[iCAN1].CAN_SamplePeriodxMS = 0 ;
      gCANParame[iCAN1].CAN_SamplePeriodxMS = BufferTemp[3];
      gCANParame[iCAN1].CAN_SamplePeriodxMS |= BufferTemp[2]<<8 ; 
      gCANParame[iCAN1].CAN_SamplePeriodxMS |= BufferTemp[1]<<16 ;
      gCANParame[iCAN1].CAN_SamplePeriodxMS |= BufferTemp[0]<<24 ;

  }else{
    
      gCANParame[iCAN1].CAN_SamplePeriodxMS =20; //CAN����ͨ��1�ɼ�ʱ����(ms) , 0��ʾ���ɼ�
  }
  
  	
	
#if DEBUG_PRINT_CAN
  
     Print_RTC();
     LOG_PR( "UpdataOnePram_SamplePeriod_CAN1():  CAN1 Sample Period = %d (ms) \r\n" , gCANParame[iCAN1].CAN_SamplePeriodxMS ); 

#endif   
  
  
  
  
}

/********************************************************************
* ���� : UpdataOnePram_UploadPeriod_CAN1
* ���� : CAN����ͨ��1 �ϴ�ʱ����(s)����ˢ��
* ˵�� : ����ID0x0101  CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
********************************************************************/
void UpdataOnePram_UploadPeriod_CAN1( void )
{
  
  u8 BufferTemp[4];
  
  if( E2_CAN1_UPLOAD_TIME_LEN ==  EepromPram_ReadPram( E2_CAN1_UPLOAD_TIME_ID ,  BufferTemp ) )
  {
    
      gCANParame[iCAN1].CAN_UploadPeriodxS = 0;
      gCANParame[iCAN1].CAN_UploadPeriodxS = BufferTemp[1];
      gCANParame[iCAN1].CAN_UploadPeriodxS |= BufferTemp[0]<<8 ;
      
  } else {

      gCANParame[iCAN1].CAN_UploadPeriodxS = 30; //CAN����ͨ��1�ϴ�ʱ����(s), 0��ʾ���ϴ�
      
  }
  
  
#if DEBUG_PRINT_CAN
  
     Print_RTC();
     LOG_PR( "UpdataOnePram_UploadPeriod_CAN1():  CAN1 Upload Period = %d (s) \r\n" , gCANParame[iCAN1].CAN_UploadPeriodxS ); 

#endif   
  
  
  
}


/********************************************************************
* ���� : Can_TimeTask
* ���� : ��ʼ��ϵͳ����֮ CAN
********************************************************************/
void CAN_TimeTask_Init(void)
{
  
#if DEBUG_PRINT_CAN
  
     Print_RTC();
     LOG_PR_N( "CAN_TimeTask_Init() \r\n" ); 
     
#endif 

     
 // CAN_PowerCtrl_Init_GPIO( );
 // CAN_PWR_ON(); //��CAN��Դ 
  
 // CAN1_Init();
  
 // CAN2_Init();
    
    CanHwInit(); 
    Can_Data_Struct_Iinit();
//����ID0x0110,CAN ����ID �����ɼ�����BYTE[8] 
#if 1
  UpdataOnePram_Sample_Single(  E2_CAN_SET_ONLY_GATHER_0_ID  );
#endif
  
//����ID0x0111,CAN ����ID �����ɼ�����BYTE[8] 
#if 1
  UpdataOnePram_Sample_Single(  E2_CAN_SET_ONLY_GATHER_1_ID  );
#endif
  
  
}

/********************************************************************
* ���� : Can_Data_Struct_Iinit
* ���� : ��ʼ��can���ݽṹ
********************************************************************/
void Can_Data_Struct_Iinit(void)
{
    u8 i =0;

    for (i =0 ;i < 2 ;i++)
    {
        gCanIsrData[i].read = 0 ;
        gCanIsrData[i].write = 0 ;
        gCanIsrData[i].receIndex = 0 ;   //����ָ��; ÿ�ν�����һ����������Ϣ���߽���Ϣȡ�� �����1;
        gCanIsrData[i].UnreadIndex =0 ;  //���յ������ݵ�δ��ָʾ;
        gCanIsrData[i].receNum =0 ;      //�������ݰ�����(��λ:13�ı���)
        gCanIsrData[i].sendLen =0;       //���͵����ݳ���(��λ:13�ı���)
        gCanIsrData[i].sendIndex =0;     //�������ݵ�����(��λ:13�ı���)
    }
}

/********************************************************************
* ���� : Can_TimeTask
* ���� : ϵͳ����֮ CAN
********************************************************************/

FunctionalState  Can_TimeTask(void)
{


  
  u8 PacktetBuf[1024];
  static u16 PacketLength;
  static u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
  u8 iCANCh = 0;
  u8 Packet_Buff_Nmb =0 ;

    UpdataOnePram_UploadPeriod_CAN1();
    if( gCANParame[iCANCh].CAN_UploadPeriodxS )
    {
       gCANParame[iCANCh].UploadTimeCnt++;
      

    Packet_Buff_Nmb = Get_Buff_Item_Length();

    if(( gCANParame[iCANCh].UploadTimeCnt>= ( gCANParame[iCANCh].CAN_UploadPeriodxS*SYSTICK_1SECOND) ) 
        ||Packet_Buff_Nmb > CAN_MAX_SEND_NMB)
    {
        
        //������0
        gCANParame[iCANCh].UploadTimeCnt = 0;
                
#if DEBUG_PRINT_CAN
         Print_RTC();
         LOG_PR( "CAN%d Upload Time Is Arrived" , iCANCh+1 ); 
         LOG_PR_N( "\r\n");     
#endif                  
                
        if(  Packet_Buff_Nmb)
        {
             
            PacketLength = Package_Can_Data(  iCAN1, (u8 *) PacktetBuf  );
            
#if DEBUG_PRINT_CAN
            if( PacketLength) 
            {
                Print_RTC();
                if(  communicatio_GetMudulState( COMMUNICATE_STATE ) )
                {
                    LOG_PR_N( "T->S OK ! ");  
                } 
                    
                    LOG_PR_N( "CAN_Packet_Single_ID():  "); 
                    LOG_DB( "", (u8 *) PacktetBuf , PacketLenght );
                    LOG_PR_N( "\r\n");    
             }
#endif       
                         
                                       
            if( PacketLength > 0)    
            {
                RadioProtocol_CAN_UpTrans( channel, (u8 *) PacktetBuf , PacketLength );
            }
            
        }

      }

    }
    else 
    {
      
        gCANParame[iCANCh].UploadTimeCnt = 0;
        
    }


/*
        CanID++;
        CanBuffer[3] = (CanID&0xFF000000) >> 24;
        CanBuffer[2] = (CanID&0xFF0000) >> 16;
        CanBuffer[1] = (CanID&0xFF00) >> 8;
        CanBuffer[0] = CanID&0xFF;
        CanBuffer[4] = 8;
        CanBuffer[5] = (CanID&0xFF000000) >> 24;
        CanBuffer[6] = (CanID&0xFF0000) >> 16;
        CanBuffer[7] = (CanID&0xFF00) >> 8;
        CanBuffer[8] = CanID&0xFF;
        CanBuffer[9] = (CanID&0xFF000000) >> 24;
        CanBuffer[10] = (CanID&0xFF0000) >> 16;
        CanBuffer[11] = (CanID&0xFF00) >> 8;
        CanBuffer[12] = CanID&0xFF;
        CanBus_Send(CanBuffer, 13);
    */    

        return ENABLE;
   
   
}

