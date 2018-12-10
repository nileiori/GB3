/*************************************************************************
* Copyright (c) 2013,深圳市伊爱高新技术开发有限公司
* All rights reserved.
* 文件名称 : Module_CAN.c
* 当前版本 : 1.0
* 开发者   : Shigle
* 修改时间 : 2013年6月10日

* 2013年9月18日为基线版本1.0, 后续功能如有增减或修改，将要求以下追加说明
* 1、暂时只启用了CAN1的功能，CAN2 的功能屏掉， 如果要开启，可在文件Module_CAN.h里修改宏 #define CAN_CH_MAX 1 //CAN通道最多数 1:只开启CAN1 2:同时开启CAN1和CAN2
* 2、考虑到内存空间限制，目前上报CAN数据同时兼顾上报周期或接收未上传的有效包数（受宏CAN_MAX_RECE_NUM控制）。
* 3、本版主要按照808-2012的协议解析和上传，未加载其它第三方客户协议
*************************************************************************/

/***  包含文件  ***/ 

#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "modem_lib.h"
#include "modem_app_com.h"

/***  外部 引用函数  ***/

extern void RTC_GetCurTime(TIME_T *tt);


/***  本地 定义的全局变量  ***/


CAN_REC  CanData;
PARAMETER_CAN   gCANParame[CAN_CH_MAX];
PARAMETER_CAN_SINGLE  gCANParameSingle;

u8  gIsCanOpen;

CAN_ISR_DATA    gCanIsrData[CAN_CH_MAX];
CAN_MASK_ID     gFilterIDArray[CAN_CH_MAX];

u16   gCANNum_Radio[CAN_CH_MAX];
u16    gCANNum_Rx[CAN_CH_MAX];


/********************************************************************
* 名称 : CAN_PowerCtrl_Init_GPIO
* 功能 : 初始化 CAN供电的相应的IO口.
********************************************************************/

void CAN_PowerCtrl_Init_GPIO( void )
{
  
  GPIO_InitTypeDef   GPIO_InitStructure;
  
  //-----------------CAN电源开关-----------------Weite
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
}



/********************************************************************
* 名称 : DecodeCan_EvTask
* 功能 : 解析CAN接收的数据包
********************************************************************/

void DecodeCan_EvTask()
{
  
}


/*
*******************************************************************
* 名称 : Time_Now_Get_BCD
* 功能 : 截取当前的RTC时间,转化成协议要求的BCD格式
* 输出 : 接收BCD时间的指针
*******************************************************************
*/
void Time_Now_Get_BCD( u8 * pTimer )
{
 
   TIME_T nowTime;
   RTC_GetCurTime(&nowTime);
   
   *(pTimer +0)=( ( nowTime.year/10 )<<4 )  + (( nowTime.year  %10)  &0x0F )            ; //年:00~99
   *(pTimer +1)=( ( nowTime.month/10)<<4 )  + (( nowTime.month %10)  &0x0F )            ; //月:1~12
   *(pTimer +2)=( ( nowTime.day/10  )<<4 )  + (( nowTime.day   %10)  &0x0F )            ; //日:1~31
   *(pTimer +3)=( ( nowTime.hour/10 )<<4 )  + (( nowTime.hour  %10)  &0x0F )            ; //时:0~23
   *(pTimer +4)=( ( nowTime.min/10  )<<4 )  + (( nowTime.min   %10)  &0x0F )            ; //分:0~59
   *(pTimer +5)=( ( nowTime.sec/10  )<<4 )  + (( nowTime.sec   %10)  &0x0F )            ; //秒:0~59
                
}


/*
*******************************************************************
* 名称 : CAN_Isr_Rx
* 功能 : 采集CAN接收中断的数据
* 输入 :  * CANx  : STEM32 CAN结构体指针
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
  gCanIsrData[iCANCh].receBuffer[num].CAN_Time[0] = NowTime[3]; //时:0~23
  gCanIsrData[iCANCh].receBuffer[num].CAN_Time[1] = NowTime[4]; //分:0~59
  gCanIsrData[iCANCh].receBuffer[num].CAN_Time[2] = NowTime[5]; //秒:0~59

  
  if(RxMessage.IDE == CAN_ID_STD)
  {
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_3 =0;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_2 = 0;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_1 = (RxMessage.StdId>>8)&0x07;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_0 = RxMessage.StdId;       

    memcpy((u8*)&gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_0, (u8*)(&RxMessage.StdId), 4);
    
    gCanIsrData[iCANCh].receBuffer[num].CAN_Frame_Type = 0; //bit 30: 表示帧类型: 0:标准帧 1:扩展帧
  }
  else
  {
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_3 =(RxMessage.ExtId>>24)&0x1F;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_2 = RxMessage.ExtId>>16;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_1 = RxMessage.ExtId>>8;
    gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_0 = RxMessage.ExtId;    

 
  //  memcpy((u8*)&gCanIsrData[iCANCh].receBuffer[num].CAN_ID_BYTE_0, (u8*)(&RxMessage.ExtId), 4);
    gCanIsrData[iCANCh].receBuffer[num].CAN_Frame_Type = 1; //bit 30: 表示帧类型: 0:标准帧 1:扩展帧
    
  }
  
   gCanIsrData[iCANCh].receBuffer[num].CAN_Data_Original = 0; //bit 29: 表示数据采集方式: 0:原始数据 1: 采集区间的计算值
        

   gCanIsrData[iCANCh].receBuffer[num].CAN_Channel = iCANCh; //bit31: 表示CAN通道号, 0: CAN1 1: CAN2
  
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
* 名称 : CAN1_Sample_Sw
* 功能 : CAN 采样开关
* 输入 : 
*       iCANChannel:    0: CAN1通道   1: CAN2通道
*            bSwCtr:    0: 关闭CAN接收中断  1: 开启CAN1接收中断   
*******************************************************************/
void CAN_Sample_Sw( u8 iCANChannel , u8 bSwCtr )
{
  
  if( iCAN1 == iCANChannel ){
    
      if(  bSwCtr ){
        
            CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//打开CAN1的接收中断
        } else {
          
          CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);//关闭CAN1的接收中断 
        } 
      
  } else if( iCAN2 == iCANChannel ) {

       if(  bSwCtr ){
         
            CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);//打开CAN2的接收中断
            
        } else {
          
            CAN_ITConfig(CAN2, CAN_IT_FMP1, DISABLE);//关闭CAN2的接收中断
        }    
     }
  
  
}
  

/*******************************************************************
* 名称 : CAN_Packet_Single_ID
* 功能 : 打包CAN数据
* 输入 : 
*       iCANCh:    0: CAN1通道   1: CAN2通道
* 输出 :
*        pdata:    接收包数据的指针
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

  dataAddr  =  pdata+2;//跳过两字节总数据包 域

  //接收的CAN总线数据项个数
  
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
      readindex = gCanIsrData[iCANCh].UnreadIndex; //从未数的序数开始读取CAN数据包解析
    
      for( k = 0; k < MAX_NUM_FILTER_ID ; k++ )
      {
        
        if(( !gCANParameSingle.CAN_Sample_Single[ k ].CAN_Sample_Interval )&&
           ( gCANParameSingle.CAN_Sample_Single[ k ].CAN_ID_BYTE_3 ==  gCanIsrData[0].receBuffer[readindex].CAN_ID_BYTE_0 )&& //过滤帧ID
           ( gCANParameSingle.CAN_Sample_Single[ k ].CAN_ID_BYTE_2 == gCanIsrData[0].receBuffer[readindex].CAN_ID_BYTE_1 )&&
           ( gCANParameSingle.CAN_Sample_Single[ k ].CAN_ID_BYTE_1 == gCanIsrData[0].receBuffer[readindex].CAN_ID_BYTE_2 )&&
           ( gCANParameSingle.CAN_Sample_Single[ k ].CAN_ID_BYTE_0 == gCanIsrData[0].receBuffer[readindex].CAN_ID_BYTE_3 ))
        {
              break; //一旦发现条件配套的，立即跳出本层循环
        } 
      }
      
      if( k !=  MAX_NUM_FILTER_ID  )
      {
            gCanIsrData[iCANCh].UnreadIndex = ( gCanIsrData[iCANCh].UnreadIndex +1)%CAN_MAX_RECE_NUM ; ////未读取包序数移动一格
            CAN_Item_TotalNum--;
      }
      else
      {
             
          //第一条CAN数据接收的时间
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

          CAN_Item_TotalNum_ID0++ ; //数据项个数++
          dataAddr += (sizeof(CAN_ITEM)-5); //字节调整
          retlenght += (sizeof(CAN_ITEM)- 5) ; //数据长度增加
          gCANNum_Radio[iCANCh]++ ;  //统计发送的CAN项总数
          
          gCanIsrData[iCANCh].UnreadIndex = (gCanIsrData[iCANCh].UnreadIndex+1)%CAN_MAX_RECE_NUM ; ////未读取包序数移动一格
          CAN_Item_TotalNum--;
          
      }
      
  }
  
  gCanIsrData[iCANCh].receNum += CAN_Item_TotalNum ; //将未打包的 项目数 重新累加
  
#if DEBUG_PRINT_CAN
  
   //   RTC_Print();
      LOG_PR( " CurItemNum = %d ; RadioSum = %d ; RxSum = %d ; " , CAN_Item_TotalNum_ID0,  gCANNum_Radio[iCANCh] ,  gCANNum_Rx[iCANCh]); 
      LOG_PR_N( "\r\n");
#endif    
               
  

  //填充  数据项个数 域内容
  if( CAN_Item_TotalNum_ID0)
  {
    
    dataAddr  =  pdata;//返回到两字节总数据包 域
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
* 名称 : Get_Buff_Item_Length
* 功能 : 获得环形缓冲区数据长度
* 输入 : 
*       
* 输出 : 缓冲区中can数据帧总数
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
* 名称 : Check_Filter_Frame
* 功能 : 找出过滤掉的数据丢弃
* 输入 : 
*       iCANCh:    0: CAN1通道   1: CAN2通道
* 输出 :0无过滤帧，1:发现过滤帧
*       
*******************************************************************/
u8 Check_Filter_Frame(u8 iCANCh)
{

    u8 i =0;
    
    for( i = 0; i < MAX_NUM_FILTER_ID ; i++ )
    {
      
      if(( !gCANParameSingle.CAN_Sample_Single[ i ].CAN_Sample_Interval )&&
         ( gCANParameSingle.CAN_Sample_Single[ i ].CAN_ID_BYTE_3 ==  gCanIsrData[iCANCh].receBuffer[gCanIsrData[iCANCh].read].CAN_ID_BYTE_0 )&& //过滤帧ID
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
* 名称 : Package_Can_Data
* 功能 : 打包CAN数据
* 输入 : 
*       iCANCh:    0: CAN1通道   1: CAN2通道
* 输出 :
*        pdata:    接收包数据的指针
*******************************************************************/
u16 Package_Can_Data( u8 iCANCh,  u8 *pdata )

{
    u16 retlength = 0 ;
    u8  *dataAddr;   
    u8 First_Fram = 0 ;
    assert_param((pdata != NULL));
    assert_param(( iCANCh < CAN_CH_MAX ));  
    
    dataAddr = pdata+2 ;

    
    if( (gCanIsrData[iCANCh].read == gCanIsrData[iCANCh].write ) )  //队列为空
    {
        return 0;
    }
    else 
    {
    u8 Packet_Nmb =0;
        while(Get_Buff_Item_Length()&& (Packet_Nmb < CAN_MAX_SEND_NMB))
        {


            if(! First_Fram)    //第一条帧前加时间
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
                dataAddr += (sizeof(CAN_ITEM)-5); //字节调整
                retlength += (sizeof(CAN_ITEM)- 5) ; //数据长度增加
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
* 名称 : 参数ID0x0110~0x01FF  总线ID 单独采集参数更新
* 功能 : CAN 总线ID 单独采集设置
* 输入 :  CANParmamerID : CAN参数ID  目前只能取值0x0110和 0x0111
*******************************************************************/
void UpdataOnePram_Sample_Single(  u32 CANParmamerID  )
{
  
  u8 BufferTemp[10];
  u32 Temp32;
//  u8 iCANChannel;
  u32 TempID ;
  u8 iItem;
  
  
#if 1 //调试时直接修改参数
  
  u8 CANParam0[] ={ 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 , 0xcc };
  u8 CANParam1[] ={ 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 , 0xcd };

  EepromPram_WritePram( E2_CAN_SET_ONLY_GATHER_0_ID , CANParam0,  sizeof(CANParam0) );  
  EepromPram_WritePram( E2_CAN_SET_ONLY_GATHER_1_ID ,  CANParam1,  sizeof(CANParam1) );  
#endif
  
  
  
    //输入参数检查
  if( 
      ( E2_CAN_SET_ONLY_GATHER_0_ID != CANParmamerID )&&
      ( E2_CAN_SET_ONLY_GATHER_1_ID != CANParmamerID )
     ) return;
  
  
    iItem = CANParmamerID - E2_CAN_SET_ONLY_GATHER_0_ID ;

  if(  E2_CAN_SET_ONLY_GATHER_LEN == EepromPram_ReadPram( CANParmamerID, BufferTemp  )    )
 // if( E2_CAN_SET_ONLY_GATHER_LEN ==  EepromPram_ReadPram(E2_CAN_SET_ONLY_GATHER_ID, (u8 *)&gCANParame[iCAN1].CAN_Sample_Single) )
  {
    
    
    
      //找出是针对哪个通道的CAN 
      //bit31: 表示CAN通道号, 0: CAN1 1: CAN2
#if 0
      if( BufferTemp[4]&0x80 ){
        
          iCANChannel = 1 ;
          
      } else {

          iCANChannel = 0 ;
      }
#endif
      
      
     //Bit28~Bit0 表示CAN总线ID
      
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_0 = BufferTemp[4]&0x1F;
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_1 = BufferTemp[5];
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_2 = BufferTemp[6];
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_3 = BufferTemp[7]; 
    
    
   //bit 29: 表示数据采集方式: 0:原始数据 1: 采集区间的计算值 
    
    if(  BufferTemp[4]&0x20 ){
      
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Data_Original   = 1; 
    
  } else {
    
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Data_Original   = 0 ;
    
  }
  
  
  //bit 30: 表示帧类型: 0:标准帧 1:扩展帧
  
  if( BufferTemp[4]&0x40 ){
    
      gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Frame_Type      = 1 ;
      
  } else {
    
      gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Frame_Type     = 0 ;
  }

  
  //Bit63~Bit32 表示该ID单独采集时间间隔(ms), 0 表示不采集 
  
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

    
    //根据采集时间间隔 开关相应通道的 接收中断
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
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_ID_BYTE_3 = 0;     // : 4;     //28字节CAN总线ID
    
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Data_Original = 0; // :1; //bit 29: 表示数据采集方式: 0:原始数据 1: 采集区间的计算值
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Frame_Type = 1; //: 1; //bit 30: 表示帧类型: 0:标准帧 1:扩展帧
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Channel =0 ; //: 1; //bit31: 表示CAN通道号, 0: CAN1 1: CAN2
    gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Sample_Interval = 15L*1000L ;

    }

  //======================打印输出日志 ===========================================
  
#if DEBUG_PRINT_CAN
  
    LOG_PR_N( "\r\n" );
  
    Print_RTC();
  
    LOG_PR( " Num = %d \r\n", gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Channel );
    
    LOG_PR_N( "====================================== \r\n" );

  //bit31: 表示CAN通道号, 0: CAN1 1: CAN2
    if( gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Channel ){
      
      LOG_PR_N( ">> CAN Channel: CAN2 \r\n" );
      
    } else {

      LOG_PR_N( ">> CAN Channel: CAN1 \r\n" );
    }
    
      
  //bit 30: 表示帧类型: 0:标准帧 1:扩展帧
      if( gCANParameSingle.CAN_Sample_Single[ iItem ].CAN_Frame_Type ){
        
        LOG_PR_N( ">> FrameType: Extend \r\n" );
        
      } else {
        
         LOG_PR_N( ">> FrameType: Standard \r\n" );
         
      }
      
    //bit 29: 表示数据采集方式: 0:原始数据 1: 采集区间的计算值 
      
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
* 名称 : UpdataOnePram_SamplePeriod_CAN1
* 功能 : CAN总线通道1 采集时间间隔参数刷新
* 说明 : 参数ID0x0100  CAN总线通道1 采集时间间隔(ms)，0 表示不采集 
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
    
      gCANParame[iCAN1].CAN_SamplePeriodxMS =20; //CAN总线通道1采集时间间隔(ms) , 0表示不采集
  }
  
  	
	
#if DEBUG_PRINT_CAN
  
     Print_RTC();
     LOG_PR( "UpdataOnePram_SamplePeriod_CAN1():  CAN1 Sample Period = %d (ms) \r\n" , gCANParame[iCAN1].CAN_SamplePeriodxMS ); 

#endif   
  
  
  
  
}

/********************************************************************
* 名称 : UpdataOnePram_UploadPeriod_CAN1
* 功能 : CAN总线通道1 上传时间间隔(s)参数刷新
* 说明 : 参数ID0x0101  CAN 总线通道1 上传时间间隔(s)，0 表示不上传
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

      gCANParame[iCAN1].CAN_UploadPeriodxS = 30; //CAN总线通道1上传时间间隔(s), 0表示不上传
      
  }
  
  
#if DEBUG_PRINT_CAN
  
     Print_RTC();
     LOG_PR( "UpdataOnePram_UploadPeriod_CAN1():  CAN1 Upload Period = %d (s) \r\n" , gCANParame[iCAN1].CAN_UploadPeriodxS ); 

#endif   
  
  
  
}


/********************************************************************
* 名称 : Can_TimeTask
* 功能 : 初始化系统任务之 CAN
********************************************************************/
void CAN_TimeTask_Init(void)
{
  
#if DEBUG_PRINT_CAN
  
     Print_RTC();
     LOG_PR_N( "CAN_TimeTask_Init() \r\n" ); 
     
#endif 

     
 // CAN_PowerCtrl_Init_GPIO( );
 // CAN_PWR_ON(); //开CAN电源 
  
 // CAN1_Init();
  
 // CAN2_Init();
    
    CanHwInit(); 
    Can_Data_Struct_Iinit();
//参数ID0x0110,CAN 总线ID 单独采集设置BYTE[8] 
#if 1
  UpdataOnePram_Sample_Single(  E2_CAN_SET_ONLY_GATHER_0_ID  );
#endif
  
//参数ID0x0111,CAN 总线ID 单独采集设置BYTE[8] 
#if 1
  UpdataOnePram_Sample_Single(  E2_CAN_SET_ONLY_GATHER_1_ID  );
#endif
  
  
}

/********************************************************************
* 名称 : Can_Data_Struct_Iinit
* 功能 : 初始化can数据结构
********************************************************************/
void Can_Data_Struct_Iinit(void)
{
    u8 i =0;

    for (i =0 ;i < 2 ;i++)
    {
        gCanIsrData[i].read = 0 ;
        gCanIsrData[i].write = 0 ;
        gCanIsrData[i].receIndex = 0 ;   //接收指标; 每次接收了一个完整的消息或者将消息取出 都会加1;
        gCanIsrData[i].UnreadIndex =0 ;  //接收到的数据但未读指示;
        gCanIsrData[i].receNum =0 ;      //接收数据包个数(单位:13的倍数)
        gCanIsrData[i].sendLen =0;       //发送的数据长度(单位:13的倍数)
        gCanIsrData[i].sendIndex =0;     //发送数据的索引(单位:13的倍数)
    }
}

/********************************************************************
* 名称 : Can_TimeTask
* 功能 : 系统任务之 CAN
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
        
        //计数清0
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

