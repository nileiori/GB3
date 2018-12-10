/*************************************************************************
* Copyright (c) 2013,�������������¼����������޹�˾
* All rights reserved.
* �ļ����� : JointechOiL_Protocol.c
* ��ǰ�汾 : 0.1
* ������   : Shigle
* �޸�ʱ�� : 2013��10��30�� 

* 2013��10��30��Ϊ���߰汾0.1 �������������������޸ģ���Ҫ������׷��˵��
* 1�����̼�����������ھ�ͨ����λ������JT606Э�鿪����
* 2���ն�ÿ10�루�ݶ� , ���ڲ������� )��ȡһ����λ����������������
*************************************************************************/


/***************   �����ļ���   ********************************/

#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"

/***************   ����ȫ�ֱ��� ������   ********************************/

STT_JOINTECH_OILCOST_PROTOCOL g_sttJointechOilCostA;
STT_JOINTECH_OILCOST_PROTOCOL g_sttJointechOilCostR;

STT_JOINTECH_COM     g_sttJointechComTx;
STT_JOINTECH_COM     g_sttJointechComRx;

STT_JOINTECH_OILCOST g_sttJointechVar ;

static STT_JOINTECH_OILCOST_CTRL g_sttJointechCtrl;
/***************   �ⲿ���� ������   ********************************/
extern u8  Uart1DataDownTransFlag;//����1����͸����־��
extern u16 Uart1DataDownTransCmd;//����1����͸�������е�������
extern u8  DelayBuffer[100];//��ʱ���壬�ɰ�Ҫ��ʱ���͵����ݿ��������ע����󳤶�����
extern u8  DelayBufferLen;//��ʱ�������ݳ���
extern u16     DelayAckCommand;//��ʱӦ������


/***************   ���ؾֲ����� ������   ********************************/


static void JointechOiLCost_StageMain_Set( u8 iStageMain );
static void Task_JointechOiLCost_Init(void);
static void JointechOiLCost_StateMachine(void);


static void JointechOiLCost_Operation_Damp( u8 InOpCode , u16 InDamp );
static void JointechOiLCost_Operation_TemperatureCompensation( u8 InOpCode );

/***************   ���غ��� ������   ********************************/

static LZM_TIMER g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAX ];



/*********************************************************************
*���� : JointechOiLCost_COM_Buf_Write
*���� : ���ھ�ͨ������λ������ COM ����д���ֽ���
*��ע :
*********************************************************************/
void JointechOiLCost_COM_Buf_Write(  u8 * pInBuf , u16 InLength )
{
  
  //TaximeterCommSendData( pInBuf, InLength );//dxl,2014.02.22,����ͷ���ͨ����485�ӿ�,��ǰ�ϵ�EGS701�Ľӿ�,�������½ӿں���
  //Usart3_SendData(pInBuf, InLength);,dxl,2014.5.12����,�ͺ���32PIN 485�ӿڸĵ�16PIN 232�ӿ�,�������½ӿں���
    COM2_WriteBuff(  pInBuf , InLength );
  
}

/*********************************************************************
*���� : JointechOiLCost_Protocol_Verify_Calc
*���� : ���ھ�ͨ������λ������ Э�����ͼ���
*��ע :
*********************************************************************/
u8 JointechOiLCost_Protocol_Verify_Calc( const u8 *pInBuf , u16 InLength )
{
  u8 verify = 0 ;
	u16 i = 0 ; 
  const u8 *p = pInBuf ;
  
  for( ;i < InLength ; i++ )
  {
    verify += p[i] ;
  }
  return verify ;

}



/*********************************************************************
*���� : JointechOiLCost_Is_ASCII_NumDec
*���� : �ж��Ƿ���ʮ������ASCII�� �� �Ƿ���1  �񷵻� 0 
*��ע : 
*********************************************************************/
u8 JointechOiLCost_Is_ASCII_NumDec( u8 pIn )
{
  
  u8 ret = 1 ;
  
  if( ( pIn < '0' )|| ( pIn > '9' ) )
  {
     ret =  0;
  }
  
  return ret ; 
    
  
}

/*********************************************************************
*���� : JointechOiLCost_Is_ASCII_NumHex
*���� : �ж��Ƿ���ʮ��������ASCII�� �� �Ƿ���1  �񷵻� 0 
*��ע : 
*********************************************************************/
u8 JointechOiLCost_Is_ASCII_NumHex( u8 pIn )
{
  
  u8 ret = 0 ;
  
  if( ( pIn >= '0' )&& ( pIn <='9' ) )
  {
     ret =  1; 
  }
  else if( ( pIn >= 'A' )&& ( pIn <='F' ) )
  {
     ret =  1; 
  }
  
  return ret ; 

}


/*********************************************************************
*���� : JointechOiLCost_2ASCII_To_1Hex
*���� : �������ֽڵ�ʮ������ASCII�ַ� ��ת����1���ֽڵ�ʮ��������
*��ע : 
*********************************************************************/
u8 JointechOiLCost_2ASCII_To_1Hex( const u8 *pIn )
{
  u8 ret = 0 ;
  u8 HexAscii[ 16 ] = { '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , 'A', 'B' , 'C', 'D' , 'E' , 'F' };
  u8 i = 0 ;
  const u8 * p = pIn ;
  
  
  for( i = 0 ; i < 16 ; i++ )
  {
    if( *p == HexAscii[ i ])
      break;
  }
  
  ret = i ;
  ret <<= 4;

   p++ ;
   for( i = 0 ; i < 16 ; i++ )
  {
    if( *p == HexAscii[ i ])
      break;
  }
  
  ret += i ;
  
  return ret;

 }



/*********************************************************************
*���� : JointechOiLCost_2ASCII_To_1Hex
*���� : ��1���ֽڵ�ʮ��������ת���������ֽڵ�ʮ������ASCII�ַ�
*��ע : 
*********************************************************************/
u8 JointechOiLCost_1Hex_To_2ASCII( u8 pIn , u8 *pOutBuf )
{
  u8 HexAscii[ 16 ] = { '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , 'A', 'B' , 'C', 'D' , 'E' , 'F' };
  u8 ret = 2 ;
  u8 * p = pOutBuf ;
  
  *p++ =  HexAscii[ pIn / 16 ];
  *p   =  HexAscii[ pIn % 16 ];
  
  return ret;

 }



/*********************************************************************
*���� : JointechOiLCost_2ASCII_To_1Dec
*���� : �������ֽڵ�ʮ����ASCII�ַ� ��ת����1���ֽڵ�ʮ������
*��ע : 
*********************************************************************/
u8 JointechOiLCost_2ASCII_To_1Dec( const u8 *pIn )
{
  u8 ret = 0 ;
  u8 DecAscii[ 10 ] = { '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9'  };
  u8 i = 0 ;
  const u8 * p = pIn ;
  
  
  for( i = 0 ; i < 10 ; i++ )
  {
    if( *p == DecAscii[ i ])
      break;
  }
  
  ret = i ;
  ret <<= 4;
  
  
   p++ ;
   for( i = 0 ; i < 10 ; i++ )
  {
    if( *p == DecAscii[ i ])
      break;
  }
  
  ret += i ;
  
  return ret;

 }

/*********************************************************************
*���� : JointechOiLCost_2ASCII_To_1Hex
*���� : ��1���ֽڵ�ʮ������ת���������ֽڵ�ʮ����ASCII�ַ�
*��ע : 
*********************************************************************/
u8 JointechOiLCost_1Dec_To_2ASCII( u8 pIn , u8 *pOutBuf )
{
  u8 HexAscii[ 16 ] = { '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' };
  u8 ret = 2 ;
  u8 * p = pOutBuf ;
  
  *p++ =  HexAscii[ pIn / 10 ];
  *p   =  HexAscii[ pIn % 10 ];
  
  return ret;

 }
/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� A
*��ע : ָ���ʶ A  �豸������
*********************************************************************/
u16 JointechOiLCost_Protocol_Extract( const u8 *pInBuf, u16 InLength, u8 *pOutBuf   )
{
   
  u16  i = 0 ;
  u8  j = 0;
  u8  iPosition[2]={ 0, 0 };
  u16 iTempLength = 0 ;
  u8  flag = 0;
  u8  cnt = 0 ;
  u8 verify0;
  u8 verify1;

  if( InLength<8){
    return iTempLength;
    
  }
  
  
  for( i = 0 ; i < InLength ; i++ ){

     /* �Ұ�ͷ'$" */
      if( '@' == pInBuf[ i ] ){
          
            flag = 1;
            cnt = 0 ;
			      iPosition[j] = i ;
            j++;
            continue;
		    }
      
        
      /* �Ұ�β'$" */ 
      if( 1 == flag ){
        
        cnt++;
        
        /* ��ʱδ�ҵ�ƥ��İ�β����������  */ 
        if( cnt > ( MAX_JOINTECH_DATA_LEN+7) ){
            flag = 1;
            cnt = 0 ;
            j = 0 ;
            break;
        }
        
        /* �ҵ���β'#" */ 
        if( '#' == pInBuf[ i ] ){
            iPosition[j] = i ;
            j++;
        }
          
        
        
      }
        
                
      if( j == 2){
            break;
      }
  } 

   if( 0 == iPosition[1]){
     
      return iTempLength;
   }
  

   iTempLength = iPosition[1]-iPosition[0]-1 ;
  /* ȥ��ͷ��β������Ϊ7�ֽ�( 2�ֽ�ID + 1�ֽ����� + 2�ֽڳ��� + 2�ֽ�У��� ) */
   if( iTempLength < 7 ){
     iTempLength = 0 ;
     return iTempLength;
   } 
  
   memcpy( pOutBuf ,  pInBuf+iPosition[0]+1, iTempLength );
   
   /* У��� �ж� */
   verify0 = JointechOiLCost_Protocol_Verify_Calc( pOutBuf , iTempLength-2 );
   verify1 = JointechOiLCost_2ASCII_To_1Hex( ( u8 * )( pOutBuf+( iTempLength-2) ) );
   if( verify0 != verify1 ){
      iTempLength = 0;
    }
/*���ⲿ�ִ���Ų�������ж���
   //���ͺĴ�������Ӧ�����ݷ��͸�ƽ̨,dxl,2014.3.14
   if((0 != iTempLength)&&(1 == Uart1DataDownTransFlag))
   {
        if(Uart1DataDownTransCmd == *(pInBuf+iPosition[0]+3))
        {
             Uart1DataDownTransFlag = 0;
             Buffer[0] = 1;
             memcpy(Buffer+1,pInBuf+iPosition[0],iTempLength+2);
             RadioProtocol_OriginalDataUpTrans(0x41, Buffer, iTempLength+3);
        }
       
   }
   */
  return iTempLength;
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� A
*��ע : ָ���ʶ A  �豸������
*********************************************************************/
u8 JointechOiLCost_Protocol_Parser( const u8 *pInBuf ,  u16 InLength , STT_JOINTECH_OILCOST_PROTOCOL *pOut )
{
  u8  ret = 0 ;
  u8  i = 0 ;

  const u8 *p = pInBuf ;
  
  /* ���������� */
  if( ( NULL == pOut ) ||( NULL == pInBuf ) || (InLength < 7 ) )
  {
    return ret ;
  }
  
  /* ȡ ID */   

  pOut->id = JointechOiLCost_2ASCII_To_1Hex( p ) ;   
  p += 2;
    
  /* ȡ ���� */ 
  pOut->cmd = *p ;
  p += 1 ;
   
  /* ȡ ���� */
  pOut->len = JointechOiLCost_2ASCII_To_1Hex( p ) ;   
  p += 2;
  
  /* �ж� ���� */
  if(  InLength != ( pOut->len + 7 ) )
  {
    ret = 0; 
    return ret ;
  }
  
   /* ȡ���� */
  if(  pOut->len < 1 )
  {

  }
  else if ( pOut->len > MAX_JOINTECH_DATA_LEN )
  {
    
    ret = 0;
    
  }
  else
  {
      for( i = 0 ; i < pOut->len ; i++ )
      {
          pOut->data[ i ] = *p++;
      }
  }

  /* ȡУ���ֽ� */
  pOut->verify = JointechOiLCost_2ASCII_To_1Hex( p );
  
  ret = 1;
  
  return ret ;
  
}

/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� A
*��ע : ָ���ʶ A  �豸������
*********************************************************************/
u16 JointechOiLCost_Protocol_Packet( STT_JOINTECH_OILCOST_PROTOCOL *pIn ,  u8 *pOutBuf  )
{

  u8 *p = pOutBuf ;
	u8 i = 0 ;
	u8 verify = 0;
 
  /* ��ͷ */
  *p++ = '@' ;
  
  /*  ID �� */
  JointechOiLCost_1Dec_To_2ASCII( pIn->id , p )  ;
  p += 2 ;
  
  /* ������ */
  *p++ = pIn->cmd ;
  
  /* ����� */
  JointechOiLCost_1Hex_To_2ASCII(  pIn->len , p );
  p += 2 ;
  
  /* �������� */
  for(i = 0 ; i < pIn->len ; i++ )
  {
    *p++ = pIn->data[ i ] ; 
  }
  
  /* ����У��� */
  
  verify = JointechOiLCost_Protocol_Verify_Calc(  pOutBuf+1 , pIn->len +5 ) ;
  JointechOiLCost_1Hex_To_2ASCII(  verify , p );
  p += 2 ;
  
  /* ��β */
  *p++ = '#' ;
  
  return (   pIn->len + 9  );
  
}

/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� A
*��ע : ָ���ʶ A  �豸������
*********************************************************************/
u16 JointechOiLCost_Process_CMD_A( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;
  u8 tempbuf[10];
  memset( tempbuf , 0 , sizeof( tempbuf ) );
  memcpy( tempbuf , &pIn->data[ 1 ] , pIn->len-1 );

  if(  NULL != strstr( ( char const *)tempbuf , "4800" ) )
  {
    g_sttJointechVar.iBaud = 0 ;
   
  }
  else if(  NULL !=  strstr( ( char const *)tempbuf , "9600" ) )
  {
    g_sttJointechVar.iBaud = 1 ;
   
  }
  else if(  NULL !=  strstr( ( char const *)tempbuf , "57600" ) )
  {
    g_sttJointechVar.iBaud = 2 ;
   
  }
  else if(  NULL !=  strstr( ( char const *)tempbuf , "115200" ) )
  {
    g_sttJointechVar.iBaud = 3 ;
   
  }
  else
  {
    g_sttJointechVar.iBaud = 0xFF ;
  }

 

  return retlen ;
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� B
*��ע : ָ���ʶ B  ��ѯ���õ���ֵ
*********************************************************************/
u16 JointechOiLCost_Process_CMD_B( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;
  u8 i = 0 ;
  u16 iWord = 0 ;
  
  
  for( i = 0 ; i < pIn->len ; i++ )
  {
     iWord *= 10 ;
     iWord +=  ( pIn->data[ i ]- '0' ) ;
     
  }
  
  g_sttJointechVar.TankZero = iWord ;
  

  return retlen ;
  
}



/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� C
*��ע : ָ���ʶ C  ��ѯ���õ���ֵ
*********************************************************************/
u16 JointechOiLCost_Process_CMD_C( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  
  u16 retlen = 0 ;
  u8 i = 0 ;
  u16 iWord = 0 ;
  
  
  for( i = 0 ; i < pIn->len ; i++ )
  {
     iWord *= 10 ;
     iWord +=  ( pIn->data[ i ]- '0' ) ;

  }
  
  g_sttJointechVar.TankFull = iWord ;
  return retlen ;
  
}

/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� D
*��ע : ָ���ʶ D  �����������
*********************************************************************/
u16 JointechOiLCost_Process_CMD_D( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;
  u8 i = 0 ;
  u16 iWord = 0 ;
  
  
  iWord = 0 ;
  for( i = 0 ; i < pIn->len ; i++ )
  {
     iWord *= 10 ;
     iWord +=  ( pIn->data[ i ]- '0' ) ;
     
  }
  
  g_sttJointechVar.TankVolume = iWord*10 ;
  
  return retlen ;
  
}




/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� E
*��ע : ָ���ʶ E  ��ȡ��ǰ��λֵ
*********************************************************************/
u16 JointechOiLCost_Process_CMD_E( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;
  u8 i = 0 ;
  u16 iWord = 0 ;
  
  /* ȡ ��ǰλ�߶���ֱ� �� ���ֵΪ10000 */
  iWord = 0 ;
  for( i = 0 ; i < 5 ; i++ )
  {
     iWord *= 10 ;
     iWord +=  ( pIn->data[ 0+i ]- '0' ) ;
     
  }

  g_sttJointechVar.LiquidLevelPercent = iWord ;
  
  /* ȡ ��ǰ�������������λֵ ��0 ~ 4095 */
  iWord = 0 ;
  for( i = 0 ; i < 4 ; i++ )
  {
     iWord *= 10 ;
     iWord +=  ( pIn->data[ 5+i ]- '0' ) ;
     
  }

  g_sttJointechVar.OilVolumeAdjust = iWord ;
  
  
  /* ȡ ����100 Ϊ��ǰ����λ���� �� ��038409 ��ʾΪ 384.09L  */
  iWord = 0 ;
  for( i = 0 ; i < 6 ; i++ )
  {
     iWord *= 10 ;
     iWord +=  ( pIn->data[ 9 + i ]- '0' ) ;
     
  }

  g_sttJointechVar.OilVolumeCur = iWord ;
  
  
  /* ȡ �¶� �� 0 ��ʾ-40���϶� �� ��068 ��ʾΪ28�� */
  iWord = 0 ;
  for( i = 0 ; i < 3 ; i++ )
  {
     iWord *= 10 ;
     iWord +=  ( pIn->data[ 16 + i ]- '0' ) ;
     
  }
  g_sttJointechVar.Temperature = iWord  ;
  
  
  return retlen ;
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� G
*��ע : ָ���ʶ G  ���ã���ѯ��̽��ID��
*********************************************************************/
u16 JointechOiLCost_Process_CMD_G( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;

  
  /* ȡ ��װ��� */
  g_sttJointechVar.iSetup = JointechOiLCost_2ASCII_To_1Dec( &pIn->data[ 0 ] );

  /* ȡ �汾��  */
  memcpy( (u8 *)g_sttJointechVar.Version , &pIn->data[ 2 ] , 8 );

  return retlen ;
  
}

/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� H
*��ע : ָ���ʶ H  ��ѯ��������
*********************************************************************/
u16 JointechOiLCost_Process_CMD_H( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;
  u8 i = 0 ;
  u16 iWord = 0 ;
  
  /* ȡ ���� �� ��λ����  */
  iWord = 0 ;
  for( i = 0 ; i < 4 ; i++ )
  {
     iWord *= 10 ;
     iWord +=  ( pIn->data[ 0+i ]- '0' ) ;
     
  }

  g_sttJointechVar.Damp = iWord ;
  
  return retlen ;
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� I
*��ע : ָ���ʶ I  �����Ƿ�����Ʈ����
*********************************************************************/
u16 JointechOiLCost_Process_CMD_I( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;

  
  /* ȡ �Ƿ��� ��Ʈ����  0��������  1: ����  0xFF : ��Ч */
  if(  '0' == pIn->data[ 0 ] )
  {
     g_sttJointechVar.IsTempCompensation = 0x00 ;
  }
  else  if(  '1' == pIn->data[ 0 ] )
  {
     g_sttJointechVar.IsTempCompensation = 0x01 ;    
  }
  else
  {
     g_sttJointechVar.IsTempCompensation = 0xFF ;    
  }


  return retlen ;
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� N
*��ע : ָ���ʶ N  ��������
*********************************************************************/
u16 JointechOiLCost_Process_CMD_N( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;

  u8 ErrorStatus = 0 ;
  
  ErrorStatus = pIn->data[ 0 ];
  
  if( ErrorStatus & 0x01 )
  {
      g_sttJointechVar.FlagErrorFrequency = 1 ; //JT606��������  0:����  1:����  
  }
  else
  {
      g_sttJointechVar.FlagErrorFrequency = 0 ; //JT606��������  0:����  1:����   
  }

  if( ErrorStatus & 0x02 )
  {
      g_sttJointechVar.FlagErrorTankZero = 1; //��ֵ���  0:����  1:����     
  }
  else
  {
      g_sttJointechVar.FlagErrorTankZero = 0 ; //��ֵ���  0:����  1:���� 
  }

  if( ErrorStatus & 0x04 )
  {
      g_sttJointechVar.FlagErrorTankFull = 1 ; //��ֵ���  0:����  1:����     
  }
  else
  {
      g_sttJointechVar.FlagErrorTankFull = 0; //��ֵ���  0:����  1:����     
  }

  return retlen ;
  
}



/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� R
*��ע : ָ���ʶ R  ��ȡ�ؼ�����
*********************************************************************/
u16 JointechOiLCost_Process_CMD_R( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{

  u16 retlen = 0 ;
  u16 iWord = 0 ;
  
  /* ȡ ����ֵ�� ���ֵΪ10000 */
  iWord = (u16 )JointechOiLCost_2ASCII_To_1Hex( &pIn->data[ 0 ] );
  iWord <<= 8 ;
  iWord += (u16 )JointechOiLCost_2ASCII_To_1Hex( &pIn->data[ 2 ] );

  g_sttJointechVar.SampleDamp = iWord ;


  /* ȡ Ƶ��ֵ�� ʮ������ASCII , �磺 007A ��ʾʮ����122  */
  iWord = (u16 )JointechOiLCost_2ASCII_To_1Hex( &pIn->data[ 4 ] );
  iWord <<= 8 ;
  iWord += (u16 )JointechOiLCost_2ASCII_To_1Hex( &pIn->data[ 6 ] );

  g_sttJointechVar.SampleFrequence = iWord ;
  

  /* ȡ �ɼ�������ֵ�� ʮ������ASCII  */
  iWord = (u16 )JointechOiLCost_2ASCII_To_1Hex( &pIn->data[ 8 ] );
  iWord <<= 8 ;
  iWord += (u16 )JointechOiLCost_2ASCII_To_1Hex( &pIn->data[ 10 ] );

  g_sttJointechVar.SampleTankZero = iWord ;
  
  /* ȡ �ɼ�������ֵ�� ʮ������ASCII  */
  iWord = (u16 )JointechOiLCost_2ASCII_To_1Hex( &pIn->data[ 12 ] );
  iWord <<= 8 ;
  iWord += (u16 )JointechOiLCost_2ASCII_To_1Hex( &pIn->data[ 14 ] );

  g_sttJointechVar.SampleTankFull = iWord ;
    
 
  
  
  return retlen ;
  
}

/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� 04H
*��ע : 04H �ɼ���¼������ϵ��
*********************************************************************/
u16 JointechOiLCost_Process_CMD(  STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf  )
{
  u16 retlen = 0 ;
  
  /* ���������� */
  if( NULL == pIn )
  {
     return retlen ;
  }
  
  switch( pIn->cmd )
  {


    case 'A': //ָ���ʶ A  �豸������
      retlen = JointechOiLCost_Process_CMD_A( pIn , pOutBuf );
    break;

    case 'B': //ָ���ʶ B  ��ѯ���õ���ֵ
      retlen = JointechOiLCost_Process_CMD_B( pIn , pOutBuf );
    break;

    case 'C': //ָ���ʶ C  ��ѯ���õ���ֵ
      retlen = JointechOiLCost_Process_CMD_C( pIn , pOutBuf );
    break;

    case 'D': //ָ���ʶ D  �����������
      retlen = JointechOiLCost_Process_CMD_D( pIn , pOutBuf );
    break;
    
    case 'E': //ָ���ʶ E  ��ȡ��ǰ��λֵ
      retlen = JointechOiLCost_Process_CMD_E( pIn , pOutBuf );
    break;

    case 'G': //ָ���ʶ G  ���ã���ѯ��̽��ID��
      retlen = JointechOiLCost_Process_CMD_G( pIn , pOutBuf );
    break;

    case 'H': //ָ���ʶ H  ��ѯ��������
      retlen = JointechOiLCost_Process_CMD_H( pIn , pOutBuf );
    break;

    case 'I': //ָ���ʶ I  �����Ƿ�����Ʈ����
      retlen = JointechOiLCost_Process_CMD_I( pIn , pOutBuf );
    break;

    case 'N': //ָ���ʶ N  ��������
      retlen = JointechOiLCost_Process_CMD_N( pIn , pOutBuf );
    break;

    case 'R': //ָ���ʶ R  ��ȡ�ؼ�����
      retlen = JointechOiLCost_Process_CMD_R( pIn , pOutBuf );
    break;

   
  }

  return retlen ;
  
}
 


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� A
*��ע : ָ���ʶ A  �豸������
*********************************************************************/
u16 JointechOiLCost_Request_CMD_A( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{

  u16 retlen = 0 ;
  u8 iBaud = 1;
  u8 iMode = 0;

 char a_baud[][8] = 
 {
    {"4800"},//0
    {"9600"},//1
    {"57600"},//2
    {"115200"},//3
 };


  pIn->id = 0x01  ;
  pIn->cmd = 'A' ;
  
  pIn->len = 1 + strlen( a_baud[ iBaud ] ) ; 
  pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����
  memcpy( &pIn->data[ 1 ] , a_baud[ iBaud ] , strlen( a_baud[ iBaud ] ) );
          
  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;
  
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� B
*��ע : ָ���ʶ B  ��ѯ���õ���ֵ
*********************************************************************/
u16 JointechOiLCost_Request_CMD_B( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;
  u8 iMode = 1;

  
  pIn->id = 0x01  ;
  pIn->cmd = 'B' ;
  
  pIn->len = 1  ; 
  pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����


  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;

}



/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� C
*��ע : ָ���ʶ C  ��ѯ���õ���ֵ
*********************************************************************/
u16 JointechOiLCost_Request_CMD_C( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;
  u8 iMode = 1;

  
  pIn->id = 0x01  ;
  pIn->cmd = 'C' ;
  
  pIn->len = 1  ; 
  pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����


  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;

  
}

/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� D
*��ע : ָ���ʶ D  �����������
*********************************************************************/
u16 JointechOiLCost_Request_CMD_D( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{

 u16 retlen = 0 ;
  u8 iMode = 1;
  u16 iTankVolume = 531 ;
  u8 iByte = 0 ;
  

  pIn->id = 0x01  ;
  pIn->cmd = 'D' ;
  
  
  if( 0x00 == iMode  )
  {
    pIn->len = 1  ; 
    pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����
  }
  else
  {
    pIn->len = 1 + 4 ; 
    pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����
  
    iByte = iTankVolume/100;
    JointechOiLCost_1Dec_To_2ASCII( iByte , &pIn->data[ 1 ] );
 
    iByte = iTankVolume%100;
    JointechOiLCost_1Dec_To_2ASCII( iByte , &pIn->data[ 3 ] );
    
  }
  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� E
*��ע : ָ���ʶ E  ��ȡ��ǰ��λֵ
*********************************************************************/
u16 JointechOiLCost_Request_CMD_E( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  
  u16 retlen = 0 ;

  pIn->id = 0x01  ;
  pIn->cmd = 'E' ; //
  pIn->len = 0x00 ;
  
  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� G
*��ע : ָ���ʶ G  ���ã���ѯ��̽��ID��
*********************************************************************/
u16 JointechOiLCost_Request_CMD_G( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  u16 retlen = 0 ;
  u8 iMode = 0;
  u8 id = 1;

  pIn->id = 0x01  ;
  pIn->cmd = 'G' ;
  
  pIn->len = 1+2; 
  pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����
  
  JointechOiLCost_1Hex_To_2ASCII( id , &pIn->data[ 1 ] );
     
  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;
  
  
}

/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� H
*��ע : ָ���ʶ H  ��ѯ��������
*********************************************************************/
u16 JointechOiLCost_Request_CMD_H( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{

  u16 retlen = 0 ;
//  u8 iMode = 1;
//  u16 iDamp = 345 ;
//  u8 iByte = 0 ;
  

  pIn->id = 0x01  ;
  pIn->cmd = 'H' ;
  
#if 0
  
  if( 0x00 == iMode  )
  {
    pIn->len = 1  ; 
    pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����
  }
  else
  {
    pIn->len = 1 + 4 ; 
    pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����
  
    iByte = iDamp/100;
    JointechOiLCost_1Dec_To_2ASCII( iByte , &pIn->data[ 1 ] );
 
    iByte = iDamp%100;
    JointechOiLCost_1Dec_To_2ASCII( iByte , &pIn->data[ 3 ] );
    
  }
  
#endif
  
  
  
  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� I
*��ע : ָ���ʶ I  �����Ƿ�����Ʈ����
*********************************************************************/
u16 JointechOiLCost_Request_CMD_I( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{

  u16 retlen = 0 ;
//  u8 iMode = 0;


  pIn->id = 0x01  ;
  pIn->cmd = 'I' ;
  
  pIn->len = 1 ; 

#if 0 

  pIn->data[ 0 ] = iMode+'0' ; // 0: ��ʾ��ѯ 1: ��ʾ����Ϊ���� 2:��ʾ�ر�

#endif
          
  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;
  
  
}


/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� N
*��ע : ָ���ʶ N  ��������
*********************************************************************/
u16 JointechOiLCost_Request_CMD_N( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{
  
  
  u16 retlen = 0 ;
  

  pIn->id = 0x01  ;
  pIn->cmd = 'N' ;
  pIn->len = 0x00 ;
  

  
  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;
  
  
}



/*********************************************************************
*���� : JointechOiLCost_Process_CMD
*���� : ���ھ�ͨ������λ�����������  ���� R
*��ע : ָ���ʶ R  ��ȡ�ؼ�����
*********************************************************************/
u16 JointechOiLCost_Request_CMD_R( STT_JOINTECH_OILCOST_PROTOCOL *pIn , u8 *pOutBuf )
{

  u16 retlen = 0 ;
  

  pIn->id =  0x01  ;
  pIn->cmd = 'R' ;
  pIn->len = 0x00 ;
  
  retlen =  JointechOiLCost_Protocol_Packet( pIn , pOutBuf  );

  return retlen ;
  
  
}



/*********************************************************************
*���� : Task_JointechOiLCost
*���� : ���ھ�ͨ������λ������ ����
*********************************************************************/
FunctionalState Task_JointechOiLCost( )
{

//  static u8 cnt = 0;
   u16 len = 0 ;
   static u8  gJointechBuf[30];
  
   if(!ReadPeripheral2TypeBit(2))//�����ͨ������λ������
   {
        return ENABLE;
   }

  Task_JointechOiLCost_Init();
  
 // JointechOiLCost_StateMachine();
  
  LZM_PublicTimerHandler( g_ArrayTimeTask_JointechOil , STAGE_JOINTECHOIL_MAX );
  
  
  
/*********************************���¿�������ʱʹ�� *********************************************/
#if 0
  
  if( cnt++ > 10 )
  {
    cnt = 0 ;
    g_sttJointechComTx.buflen = JointechOiLCost_Request_CMD_E( &g_sttJointechOilCostR , g_sttJointechComTx.buf  );
    JointechOiLCost_COM_Buf_Write( g_sttJointechComTx.buf , g_sttJointechComTx.buflen );

  }

#endif

   /****** ���մ��� *******/
  // g_sttJointechComRx.buflen = COM1_ReadBuff( g_sttJointechComRx.buf , sizeof( g_sttJointechComRx.buf ) );
  
   if( g_sttJointechComRx.buflen )
   {
     
       len = JointechOiLCost_Protocol_Extract( g_sttJointechComRx.buf , g_sttJointechComRx.buflen , gJointechBuf  );
      
      if( len ){

         memset( ( u8 * )&g_sttJointechOilCostA , 0 , sizeof ( g_sttJointechOilCostA ) );
         len = JointechOiLCost_Protocol_Parser( gJointechBuf, len , &g_sttJointechOilCostA );
         
         if( len ){

             g_sttJointechComTx.buflen = JointechOiLCost_Process_CMD( &g_sttJointechOilCostA ,  g_sttJointechComTx.buf );
             
             if( STAGE_JOINTECHOIL_E_OILCURRENT == g_sttJointechCtrl.StageMain )
             {
                 if( 'E' == g_sttJointechOilCostA.cmd )
                 {
                     g_sttJointechCtrl.IsAckOK = 1 ; //�յ���ȷ��Ӧ��
                 } 
             }
             else if( STAGE_JOINTECHOIL_H_DAMP == g_sttJointechCtrl.StageMain )
             {
                 if( 'H' == g_sttJointechOilCostA.cmd )
                 {
                     g_sttJointechCtrl.IsAckOK = 1 ; //�յ���ȷ��Ӧ��
                 }              
             }
             else if( STAGE_JOINTECHOIL_I_TEMPERATURE_COMPENSATION == g_sttJointechCtrl.StageMain )
             {
                 if( 'I' == g_sttJointechOilCostA.cmd )
                 {
                     g_sttJointechCtrl.IsAckOK = 1 ; //�յ���ȷ��Ӧ��
                 }              
             }
             else if( STAGE_JOINTECHOIL_R_CRITICALPARAMETERS == g_sttJointechCtrl.StageMain )
             {
                 if( 'R' == g_sttJointechOilCostA.cmd )
                 {
                     g_sttJointechCtrl.IsAckOK = 1 ; //�յ���ȷ��Ӧ��
                 }              
             }             
             
             else if( STAGE_JOINTECHOIL_NULL == g_sttJointechCtrl.StageMain )
             {
                g_sttJointechCtrl.IsAckOK = 0 ; //�յ���ȷ��Ӧ��
             }
         
         }
      }
      
      memset( g_sttJointechComRx.buf, 0 , sizeof( g_sttJointechComRx.buf ) );
  
  }
  
  
  /*
  
  if( JointechOiLCost_Online_Get() )
  {
     Io_WriteAlarmBit( ALARM_BIT_RESERVER30 , RESET );
  }
  else
  {
     Io_WriteAlarmBit( ALARM_BIT_RESERVER30 , SET );    
  }

  */
  
  
  return ENABLE;
  
}


/********/

FunctionalState JointechOiLCost_TimeTask( )
{
//#ifdef JIUTONG_OIL_VERSION,dxl,2014.5.12����,�ͺ���32PIN 485�ӿڸĵ�16PIN 232�ӿ�
    return  Task_JointechOiLCost();
//#else
    //return DISABLE;
//#endif
}

/*********************************************************************
*���� : JointechOiLCost_OilVolumeCur_Get
*���� : ��ȡ�����ڵ�ǰʣ������ֵ��
*��ע : �ֱ���: 0.01��/λ ,ƫ����: 0
*********************************************************************/
u16 JointechOiLCost_OilVolumeCur_Get( void )
{
  return g_sttJointechVar.OilVolumeCur;
}
/*********************************************************************
*���� : JointechOiLCost_OilVolumeCur_Get
*���� : ��ȡ�����ڵ�ǰҺλ�߶���ֱȣ� ���ֵΪ10000��
*��ע : �ֱ���: 0.01% ,ƫ����: 0
*********************************************************************/
u16 JointechOiLCost_OilLevelPercentCur_Get( void )
{
  return g_sttJointechVar.LiquidLevelPercent;
}

/********************************************************************
* ���� : JointechOiLCost_CopyBuf
* ���� : �Ӵ��ڽ���Buf��������
* ˵��  
********************************************************************/
void  JointechOiLCost_CopyBuf( u8 * pInBuf, u16 InLength )
{
       /*****************2013-09-14 ***********************/
    if( InLength > sizeof( g_sttJointechComRx.buf ) ){
      
      g_sttJointechComRx.buflen = sizeof( g_sttJointechComRx.buf );
      
    }else {
      
      g_sttJointechComRx.buflen = InLength ;
    }
    
    memcpy( g_sttJointechComRx.buf , pInBuf ,  g_sttJointechComRx.buflen );
    

}



/********************************************************************
* ���� : JointechOiLCost_Error_Set
* ���� : ��ͨ������λ���������ϱ�־ ��λ
* ˵��  
********************************************************************/
void  JointechOiLCost_Error_Set( void )
{
  g_sttJointechCtrl.ErrorFlag = 1 ;
}


/********************************************************************
* ���� : JointechOiLCost_Error_Clr
* ���� : ��ͨ������λ���������ϱ�־ ���
* ˵��  
********************************************************************/
void  JointechOiLCost_Error_Clr( void )
{
  g_sttJointechCtrl.ErrorFlag = 0 ;
}

/********************************************************************
* ���� : JointechOiLCost_Error_Set
* ���� : ��ͨ������λ���������ϱ�־ ��λ
* ˵��  
********************************************************************/
u8  JointechOiLCost_Error_Get( void )
{
  return g_sttJointechCtrl.ErrorFlag ;
}




/********************************************************************
* ���� : JointechOiLCost_Error_Set
* ���� : ��ͨ������λ������ ����
* ˵��  
********************************************************************/
void  JointechOiLCost_Online_Set( void )
{
  g_sttJointechCtrl.IsExist = 1 ;
}


/********************************************************************
* ���� : JointechOiLCost_Error_Clr
* ���� : ��ͨ������λ������  ����
* ˵��  
********************************************************************/
void  JointechOiLCost_Online_Clr( void )
{
  g_sttJointechCtrl.IsExist = 0 ;
}

/********************************************************************
* ���� : JointechOiLCost_Error_Set
* ���� : ��ѯ��ͨ������λ�������Ƿ� ����
* ˵��  
********************************************************************/
u8  JointechOiLCost_Online_Get( void )
{
  return g_sttJointechCtrl.IsExist ;
}




/*********************************************************************
*���� : JointechOiLCost_StateMachine_E
*���� : ���ھ�ͨ������λ������  ��״̬�� ; ����ǰ��λ
*��ע : 
*********************************************************************/
void JointechOiLCost_StateMachine_E( )
{
  if( 0 == g_sttJointechCtrl.StageSub )
  {
    
      g_sttJointechComTx.buflen = JointechOiLCost_Request_CMD_E( &g_sttJointechOilCostR , g_sttJointechComTx.buf  );
      JointechOiLCost_COM_Buf_Write( g_sttJointechComTx.buf , g_sttJointechComTx.buflen );

      g_sttJointechCtrl.StageSub = 1;    
      LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(9), JointechOiLCost_StateMachine ) ;
      
  }
  else if( g_sttJointechCtrl.ReCheckCnt < 3 )
  {
      if( 1 == g_sttJointechCtrl.IsAckOK )
      {
          g_sttJointechCtrl.IsExist = 1 ;  //��1��������
//          Io_WriteAlarmBit( ALARM_BIT_RESERVER30 , RESET );
          JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );
      }
      else
      {
          g_sttJointechCtrl.ReCheckCnt++;
          g_sttJointechCtrl.StageSub = 0 ;
          LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(1), JointechOiLCost_StateMachine ) ;
      }

  }
  else
  {
    
    JointechOiLCost_Error_Set();
    
    if( g_sttJointechCtrl.ErrorCnt > 3 )
    {
      g_sttJointechCtrl.IsExist = 0;    //�Ƿ����
//      Io_WriteAlarmBit( ALARM_BIT_RESERVER30 , SET ); 
    }

    JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );
    
  }
  

  
}




/*********************************************************************
*���� : JointechOiLCost_StateMachine_H
*���� : ���ھ�ͨ������λ������  ��״̬�� ; ��ѯ/��������
*********************************************************************/
void JointechOiLCost_StateMachine_H( )
{
  /*
  if( 0 == g_sttJointechCtrl.StageSub )
  {
    
      g_sttJointechComTx.buflen = JointechOiLCost_Request_CMD_H( &g_sttJointechOilCostR , g_sttJointechComTx.buf  );
      JointechOiLCost_COM_Buf_Write( g_sttJointechComTx.buf , g_sttJointechComTx.buflen );

      g_sttJointechCtrl.StageSub = 1;    
      LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(6), JointechOiLCost_StateMachine ) ;
      
  }
  else if( g_sttJointechCtrl.ReCheckCnt < 3 )
  {
      if( 1 == g_sttJointechCtrl.IsAckOK )
      {
          g_sttJointechCtrl.IsExist = 1 ;  //��1��������
          JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );//�ٴλص� ��ȡ ��ǰ��λֵ״̬
      }
      else
      {
          g_sttJointechCtrl.ReCheckCnt++;
          g_sttJointechCtrl.StageSub = 0 ;
          LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(1), JointechOiLCost_StateMachine ) ;
      }

  }
  else
  {
    
    JointechOiLCost_Error_Set();
    
    if( g_sttJointechCtrl.ErrorCnt > 3 )
    {
      g_sttJointechCtrl.IsExist = 0;    //�Ƿ����
    }

    JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );
    
  }
  */
    JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );
}

/*********************************************************************
*���� : JointechOiLCost_StateMachine_I
*���� : ���ھ�ͨ������λ������  ��״̬�� ; �����Ƿ�����Ʈ����
*********************************************************************/
void JointechOiLCost_StateMachine_I( )
{
  /*
  if( 0 == g_sttJointechCtrl.StageSub )
  {
    
      g_sttJointechComTx.buflen = JointechOiLCost_Request_CMD_I( &g_sttJointechOilCostR , g_sttJointechComTx.buf  );
      JointechOiLCost_COM_Buf_Write( g_sttJointechComTx.buf , g_sttJointechComTx.buflen );

      g_sttJointechCtrl.StageSub = 1;    
      LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(6), JointechOiLCost_StateMachine ) ;
      
  }
  else if( g_sttJointechCtrl.ReCheckCnt < 3 )
  {
      if( 1 == g_sttJointechCtrl.IsAckOK )
      {
          g_sttJointechCtrl.IsExist = 1 ;  //��1��������
        //  JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );//�ٴλص� ��ȡ ��ǰ��λֵ״̬
          JointechOiLCost_Operation_Damp( 1 , DAMP_DEFAULT );
      }
      else
      {
          g_sttJointechCtrl.ReCheckCnt++;
          g_sttJointechCtrl.StageSub = 0 ;
          LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(1), JointechOiLCost_StateMachine ) ;
      }

  }
  else
  {
    
    JointechOiLCost_Error_Set();
    
    if( g_sttJointechCtrl.ErrorCnt > 3 )
    {
      g_sttJointechCtrl.IsExist = 0;    //�Ƿ����
    }

    //JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );
    
    JointechOiLCost_Operation_Damp( 1 , DAMP_DEFAULT );
    
  }
  */
  JointechOiLCost_Operation_Damp( 1 , DAMP_DEFAULT );

}



/*********************************************************************
*���� : JointechOiLCost_StateMachine_R
*���� : ���ھ�ͨ������λ������  ��״̬�� ; ���ؼ�����
*��ע : 
*********************************************************************/
void JointechOiLCost_StateMachine_R( )
{
  if( 0 == g_sttJointechCtrl.StageSub )
  {
    
      g_sttJointechComTx.buflen = JointechOiLCost_Request_CMD_R( &g_sttJointechOilCostR , g_sttJointechComTx.buf  );
      JointechOiLCost_COM_Buf_Write( g_sttJointechComTx.buf , g_sttJointechComTx.buflen );

      g_sttJointechCtrl.StageSub = 1;    
      LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(6), JointechOiLCost_StateMachine ) ;
      
  }
  else if( g_sttJointechCtrl.ReCheckCnt < 3 )
  {
      if( 1 == g_sttJointechCtrl.IsAckOK )
      {
          g_sttJointechCtrl.IsExist = 1 ;  //��1��������
          JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );//�ٴλص� ��ȡ ��ǰ��λֵ״̬
      }
      else
      {
          g_sttJointechCtrl.ReCheckCnt++;
          g_sttJointechCtrl.StageSub = 0 ;
          LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(6), JointechOiLCost_StateMachine ) ;
      }

  }
  else
  {
    
    JointechOiLCost_Error_Set();
    
    if( g_sttJointechCtrl.ErrorCnt > 3 )
    {
      g_sttJointechCtrl.IsExist = 0;    //�Ƿ����
    }

    JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );
    
  }
  

  
}


/*********************************************************************
*���� : Task_JointechOiLCost
*���� : ���ھ�ͨ������λ������ ����
*��ע : 
*********************************************************************/
static void Task_JointechOiLCost_Init( )
{

 static u8 FlagInit = 0 ;

 if( 0 == FlagInit )
 {
   
   FlagInit = 1 ;
   
   //Comm485PowerOn( POWER_STATUS_CARD ); dxl,2014.02.22,δʹ�õ�
   
   memset( ( u8 * ) &g_sttJointechOilCostA , 0 , sizeof( STT_JOINTECH_OILCOST_PROTOCOL ) );
   memset( ( u8 * ) &g_sttJointechOilCostR , 0 , sizeof( STT_JOINTECH_OILCOST_PROTOCOL ) );
   
   memset( ( u8 * ) &g_sttJointechComTx , 0 , sizeof( STT_JOINTECH_COM ) );
   memset( ( u8 * ) &g_sttJointechComRx , 0 , sizeof( STT_JOINTECH_COM ) );
   memset( ( u8 * ) &g_sttJointechVar , 0 , sizeof( STT_JOINTECH_OILCOST ) );
   memset( ( u8 * ) &g_sttJointechCtrl , 0 , sizeof( STT_JOINTECH_OILCOST_CTRL ) );
   
   SetTimerTask( TIME_JOINTECHOILCOST , LZM_TIME_BASE );
   LZM_PublicKillTimerAll( g_ArrayTimeTask_JointechOil , TASK_JOINTECHOIL_MAX  );
   
  // JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_E_OILCURRENT );
   
  // JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_R_CRITICALPARAMETERS );
   
   JointechOiLCost_Operation_TemperatureCompensation( 1 );
   
 }


}


/*********************************************************************
*���� : JointechOiLCost_StageMain_Set
*���� : ���ھ�ͨ������λ������ ״̬�� ��״̬����
*********************************************************************/
static void JointechOiLCost_StageMain_Set( u8 iStageMain )
{

  g_sttJointechCtrl.StageMain =  iStageMain ; //��״̬
  g_sttJointechCtrl.StageSub = 0 ; //��״̬
  g_sttJointechCtrl.ReCheckCnt = 0 ; //�ط�������
  g_sttJointechCtrl.IsAckOK = 0 ;   //�Ƿ��յ���ȷ��Ӧ��
  

  if( STAGE_JOINTECHOIL_E_OILCURRENT == iStageMain )
  {
    LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(1), JointechOiLCost_StateMachine ) ;
  }
  else if( STAGE_JOINTECHOIL_H_DAMP == iStageMain )
  {
    LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(1), JointechOiLCost_StateMachine ) ;
  } 
  else if( STAGE_JOINTECHOIL_I_TEMPERATURE_COMPENSATION == iStageMain )
  {
    LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(1), JointechOiLCost_StateMachine ) ;
  } 
  else if( STAGE_JOINTECHOIL_R_CRITICALPARAMETERS == iStageMain )
  {
    LZM_PublicSetOnceTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAIN ], PUBLICSECS(1), JointechOiLCost_StateMachine ) ;
  }  
  else if( STAGE_JOINTECHOIL_NULL == iStageMain )
  {
      LZM_PublicKillTimer( &g_ArrayTimeTask_JointechOil[ TASK_JOINTECHOIL_MAX ] );
   //     LZM_PublicKillTimer( &s_stLEDTimer[LED_TIMER_CHECK]);
  }

}


/*********************************************************************
*���� : JointechOiLCost_StateMachine
*���� : ���ھ�ͨ������λ������  ��״̬��
*��ע : 
*********************************************************************/
void JointechOiLCost_StateMachine( )
{

  switch( g_sttJointechCtrl.StageMain )
  {
      case STAGE_JOINTECHOIL_E_OILCURRENT:
      JointechOiLCost_StateMachine_E();
      break;

      case STAGE_JOINTECHOIL_H_DAMP:
      JointechOiLCost_StateMachine_H();
      break;       

      case STAGE_JOINTECHOIL_I_TEMPERATURE_COMPENSATION:
      JointechOiLCost_StateMachine_I();
      break;    
    
      case STAGE_JOINTECHOIL_R_CRITICALPARAMETERS:
      JointechOiLCost_StateMachine_R();
      break;   
      
      
      
      
  
      default:
      break;

  }
  
}




/*********************************************************************
*���� : JointechOiLCost_StateMachine_I
*���� : ���ھ�ͨ������λ������  ��״̬�� ; ��ѯ/���� ����
*��ע : 
*********************************************************************/
void JointechOiLCost_Operation_Damp( u8 InOpCode , u16 InDamp )
{
  u8 iByte = 0 ;
  
  if( InOpCode > 1 )
  {
      InOpCode = 0 ;
  }
  
  if( InDamp < 5 )
  {
    InDamp = 5 ;
  }
  
  if( InDamp > 600 )
  {
    InDamp = 600 ;
  }
  

  g_sttJointechOilCostR.data[ 0 ] = InOpCode + '0' ; 

  iByte = InDamp/100;
  JointechOiLCost_1Dec_To_2ASCII( iByte , &g_sttJointechOilCostR.data[ 1 ] );
  
  iByte = InDamp%100;
  JointechOiLCost_1Dec_To_2ASCII( iByte , &g_sttJointechOilCostR.data[ 3 ]  );
  
  
  g_sttJointechOilCostR.len = 5 ; 

  /* ������״̬ Ϊ ��Ʈ��������״̬ */
  JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_H_DAMP );
}




/*********************************************************************
*���� : JointechOiLCost_StateMachine_I
*���� : ���ھ�ͨ������λ������  ��״̬�� ; �����Ƿ�����Ʈ����
*��ע : 
*********************************************************************/
void JointechOiLCost_Operation_TemperatureCompensation( u8 InOpCode )
{
  
  if( InOpCode > 2 )
  {
      InOpCode = 0 ;
  }
  
  g_sttJointechOilCostR.data[ 0 ] = InOpCode + '0' ; 
  
  /* ������״̬ Ϊ ��Ʈ��������״̬ */
  JointechOiLCost_StageMain_Set( STAGE_JOINTECHOIL_I_TEMPERATURE_COMPENSATION );
}
/*********************************************************************
//��������	:JointechOiLCost_OilProtocolCheck
//����		:��ͨ�ͺ�Э����,��ȷ�Ļ�����ú���JointechOiLCost_CopyBuf,
//          :���ͺ�ָ�����g_sttJointechComRx.buf�У�Ȼ�����
//��ע		:,dxl,2014.5.12����,�ͺ���32PIN 485�ӿڸĵ�16PIN 232�ӿ�
//          :����ֵΪ1��ʾ���ж�Ϊ�ͺ�ָ�Ϊ0��ʾ�����ͺ�ָ��
*********************************************************************/
u8 JointechOiLCost_OilProtocolCheck(u8 *pBuffer, u16 BufferLen)
{
    u16 i = 0;
    u16 j = 0;
    u16 k = 0;
    u8  flag = 0;
    u8  HighByte = 0;
    u8  LowByte = 0;
    u8  sum = 0;
    u8  *p = NULL;
    
    if(!ReadPeripheral2TypeBit(2))return 0;//bit2:δ�����ͨ������λ������,lzm,2014.9.24
    
    if((NULL != p)||(BufferLen >= 100))
    {
        return 0;
    }
    p = pBuffer;
    for(i=0; i<BufferLen; i++)
	{
		if('@' == *(p+i))
		{
				break;
		}
	}
    for(j=i; j<BufferLen; j++)
	{
        if('#' == *(p+j))
		{
            flag = 1;
            k = j+1;//����
		    break;
		}
	}
	if((1 == flag)&&(k > 3))
    {
        sum = 0;
        for(j=i+1;j<k-3;j++)
        {
            sum += *(p+j);
        }
        
        HighByte = *(p+k-3);
        if((HighByte >= '0')&&(HighByte <= '9'))
        {
            HighByte -= '0';
        }
        else if((HighByte >= 'A')&&(HighByte <= 'F'))
        {
            HighByte -= 55;
        }
        else if((HighByte >= 'a')&&(HighByte <= 'f'))
        {
            HighByte -= 87;
        }
        else
        {
            flag = 0;//�Ƿ��ַ�
        }
            
        LowByte = *(p+k-2);
        if((LowByte >= '0')&&(LowByte <= '9'))
        {
            LowByte -= '0';
        }
        else if((LowByte >= 'A')&&(LowByte <= 'F'))
        {
            LowByte -= 55;
        }
        else if((LowByte >= 'a')&&(LowByte <= 'f'))
        {
            LowByte -= 87;
        }
        else
        {
            flag = 0;//�Ƿ��ַ�
        }
            
        if((sum == ((HighByte<<4)|LowByte))&&(1 == flag))
        {
            //���͵�ƽ̨
            if((Uart1DataDownTransCmd == *(p+i+3))&&(1 == Uart1DataDownTransFlag))
            {
                Uart1DataDownTransFlag = 0;
                DelayBuffer[0] = 1;
                memcpy(DelayBuffer+1,p+i,k-i);
                DelayBufferLen = k-i+1;
                DelayAckCommand = 13;//��һ����Ӧ���͵�ƽ̨
                SetTimerTask(TIME_DELAY_TRIG, 1*SECOND);//������ʱ����
            }
            JointechOiLCost_CopyBuf(p+i,k-i); 
            
        }
        else
        {
            flag = 0;
        }
    }
    return flag;
	
}
/*******************************************************************************
** ��������: JointechOiLCost_GetSubjoinInfoCurOilVolume
** ��������: ȡ�õ�ǰ����������Ϣ��(����������ϢID�����ȡ�������Ϣ��)
** ��ڲ���: ��Ҫ��������������Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: ����������Ϣ�����ܳ���
*******************************************************************************/
unsigned char JointechOiLCost_GetSubjoinInfoCurOilVolume(unsigned char *data)
{
    unsigned char buffer[10]={0};
    unsigned char len;
    unsigned short usVal;
    if(!ReadPeripheral2TypeBit(2))return 0;//δ�����ͨ������λ������
    len = 0;
    usVal = JointechOiLCost_OilVolumeCur_Get()/10;
    buffer[len++] = 0x02;//������ϢID
    buffer[len++] = 2;   //���ӳ���
    Public_ConvertShortToBuffer(usVal,&buffer[len]);//������Ϣ��
    //////////////////////////
    len += 2;
    ///////////////
    memcpy(data,buffer,len);
    /////////////////
    return len;
}
/*******************************************************************************
** ��������: JointechOiLCost_GetSubjoinInfoCurOilLevelPercent
** ��������: ȡ�õ�ǰ�����ٷֱȸ�����Ϣ��(����������ϢID�����ȡ�������Ϣ��)
** ��ڲ���: ��Ҫ���������ٷֱȸ�����Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: �����ٷֱȸ�����Ϣ�����ܳ���
*******************************************************************************/
unsigned char JointechOiLCost_GetSubjoinInfoCurOilLevelPercent(unsigned char *data)
{
    unsigned char buffer[10]={0};
    unsigned char len;
    unsigned short usVal;
    if(!ReadPeripheral2TypeBit(2))return 0;//δ�����ͨ������λ������
    len = 0;
    usVal = JointechOiLCost_OilLevelPercentCur_Get();
    buffer[len++] = 0xE4;//������ϢID
    buffer[len++] = 2;   //���ӳ���
    Public_ConvertShortToBuffer(usVal,&buffer[len]);//������Ϣ��
    //////////////////////////
    len += 2;
    ///////////////
    memcpy(data,buffer,len);
    /////////////////
    return len;
}