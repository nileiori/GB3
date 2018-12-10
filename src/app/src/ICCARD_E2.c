/*************************************************************************
* Copyright (c) 2014 All rights reserved., 
* �ļ����� : ICCARD_E2.c
* ��ǰ�汾 : 0.1
* ������   : MARK
* �༭ʱ�� : 2014��03��18��
* ����ʱ�� ��
*************************************************************************/



/****************************�ļ�Ԥ����**********************************/

 #include  "ICCARD_E2.h"                /*����IC�������������������õ�ͷ�ļ�*/
 #include 	"ICCARD_M3.h"
/***********************************************************************/
//#define EEPROM_24XX_USE_ALL_PAGE 
/***********************************************************************/
typedef struct tagSTT_AT24C128_CTRL{

    #ifdef EEPROM_24XX_USE_ALL_PAGE    
    u8 buf[ EE_24XX_HL_SIZE ];
    #else
    u8 buf[ 128 ];
    #endif

    u8 IsValid;

    u16 BTID_Addr;

    u8  BTID_len;

    u8  BTID_Start[ 6 ];

    u8  BTID_End[ 6 ] ;

    u16 BTID_STEPS_Total;

    u16 BTID_STEPS_Finished;

    u16 BTID_STEPS_unFinished;

    u16 BTID_STEPS_Successed;

}STT_AT24C128_CTRL;
 
STT_AT24C128_CTRL   g_sttAT24C128Ctrl;

/********************************************************************************************************
*	�� �� ��: E2_Check_I2C_Ack
*	����˵��:
*	�� �� ֵ: 1 ��ʾʧ�ܣ�0 ��ʾ�ɹ�
********************************************************************************************************/
u8 E2_Check_I2C_Ack(void)
{
    if(ICCARD_M3_I2C_CheckAck())
	{
        ICCARD_M3_I2C_Stop();
		return 1;	/* EEPROM������Ӧ�� */
	}
    return 0;
}

/********************************************************************************************************
*	�� �� ��: E2_ReadOneByte
*	����˵��: �Ӵ���EEPROMָ����ַ����ʼ��ȡһ������
*	��    �Σ�_pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*			 _ucDevAddr : �豸��ַ
*			 _usAddress : ��ʼ��ַ
*			 _pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
********************************************************************************************************/
u8 E2_ReadOneByte(u8 *data, u8 _ucDevAddr, u8 _usAddress)
{	
	/* ��1��������I2C���������ź� */
	ICCARD_M3_I2C_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	ICCARD_M3_I2C_SendByte( _ucDevAddr | I2C_WR);	/* �˴���дָ�� */
	
	/* ��3��������ACK */
    
	if (E2_Check_I2C_Ack())
	{
		return 0;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	ICCARD_M3_I2C_SendByte((u8)( _usAddress )&0x0FF  );
	
	/* ��5��������ACK */
	if (E2_Check_I2C_Ack())
	{
		return 0;	/* EEPROM������Ӧ�� */
	}
  	
	/* ��6������������I2C���ߡ�ǰ��Ĵ����Ŀ����EEPROM���͵�ַ�����濪ʼ��ȡ���� */
	ICCARD_M3_I2C_Start();
	
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	ICCARD_M3_I2C_SendByte( _ucDevAddr | I2C_RD);	/* �˴��Ƕ�ָ�� */
	
	/* ��8��������ACK */
	if (E2_Check_I2C_Ack())
	{
		return 0;	/* EEPROM������Ӧ�� */
	}	
	
	/* ��9����ѭ����ȡ���� */    
	*data = ICCARD_M3_I2C_ReadByte() ;	/* ��1���ֽ� */
    
    ICCARD_M3_I2C_SendNAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
	/* ����I2C����ֹͣ�ź� */
	ICCARD_M3_I2C_Stop();
    ////////////
    return 1;	/* ִ�гɹ� */
}
/********************************************************************************************************
*	�� �� ��: E2_ReadBytes
*	����˵��: �Ӵ���EEPROMָ����ַ����ʼ��ȡ��������
*	��    �Σ�_pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*			 _ucDevAddr : �豸��ַ
*			 _usAddress : ��ʼ��ַ
*			 _usSize : ���ݳ��ȣ���λΪ�ֽ�
*			 _pReadBuf : ��Ŷ��������ݵĻ�����ָ��
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
********************************************************************************************************/
u8 E2_ReadBytes(u8 *_pReadBuf, u8 _ucDevAddr, u8 _usAddress, u16 _usSize)
{
	u16 i;
	
	/* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */
	
	/* ��1��������I2C���������ź� */
	ICCARD_M3_I2C_Start();
	
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	ICCARD_M3_I2C_SendByte( _ucDevAddr | I2C_WR);	/* �˴���дָ�� */
	
	/* ��3��������ACK */
    
	if (E2_Check_I2C_Ack())
	{
		return 0;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	ICCARD_M3_I2C_SendByte((u8)( _usAddress )&0x0FF  );
	
	/* ��5��������ACK */
	if (E2_Check_I2C_Ack())
	{
		return 0;	/* EEPROM������Ӧ�� */
	}
  	
	/* ��6������������I2C���ߡ�ǰ��Ĵ����Ŀ����EEPROM���͵�ַ�����濪ʼ��ȡ���� */
	ICCARD_M3_I2C_Start();
	
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	ICCARD_M3_I2C_SendByte( _ucDevAddr | I2C_RD);	/* �˴��Ƕ�ָ�� */
	
	/* ��8��������ACK */
	if (E2_Check_I2C_Ack())
	{
		return 0;	/* EEPROM������Ӧ�� */
	}	
	
	/* ��9����ѭ����ȡ���� */
	for (i = 0; i < _usSize; i++)
	{
		_pReadBuf[i] = ICCARD_M3_I2C_ReadByte() ;	/* ��1���ֽ� */
		
		/* ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack */
		if (i != _usSize - 1)
		{
			ICCARD_M3_I2C_SendAck();	/* �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) */
		}
		else
		{
			ICCARD_M3_I2C_SendNAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		}
	}
	/* ����I2C����ֹͣ�ź� */
	ICCARD_M3_I2C_Stop();
    ////////////
    return 1;	/* ִ�гɹ� */
}
/**********************************************************************************************************
*	�� �� ��: E2_WriteOneByte
*	����˵��: ����EEPROMָ����ַд��һ������
*	��    �Σ�data : ����
*			  _ucDevAddr : �豸��ַ
*			  _ucAddress : д���ַ
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
**********************************************************************************************************/
u8 E2_WriteOneByte(u8 data, unsigned char _ucDevAddr, unsigned char _ucAddress)
{
    unsigned char m;

    /*���ڣ�������ֹͣ�źţ������ڲ�д������*/
    ICCARD_M3_I2C_Stop();

    for (m = 0; m < 100; m++)
	{
		/* ��1��������I2C���������ź� */
		ICCARD_M3_I2C_Start();
		
		/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
		ICCARD_M3_I2C_SendByte( _ucDevAddr | I2C_WR );	/* �˴���дָ�� */
		
		/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
		if (ICCARD_M3_I2C_CheckAck() == 0)
		{
			break;
		}
	}
	if (m  == 100)
	{
        ICCARD_M3_I2C_Stop();
        return 0;
	}

    /* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
    ICCARD_M3_I2C_SendByte(_ucAddress & 0xFF );

    /* ��5��������ACK */
    if (E2_Check_I2C_Ack())
    {
        return 0;	/* EEPROM������Ӧ�� */
    }	
    /* ��6������ʼд������ */
    ICCARD_M3_I2C_SendByte(data);

    /* ��7��������ACK */
    if (E2_Check_I2C_Ack())
    {
        return 0;	/* EEPROM������Ӧ�� */
    }

    /* ����ִ�гɹ�������I2C����ֹͣ�ź� */
    ICCARD_M3_I2C_Stop();
    ////////////
    return 1;
}
/**********************************************************************************************************
*	�� �� ��: E2_WriteBytes
*	����˵��: ����EEPROMָ����ַд���������ݣ�����ҳд�������д��Ч��
*	��    �Σ�_pWriteBuf : ��Ŷ��������ݵĻ�����ָ��
*			  _ucDevAddr : �豸��ַ
*			  _usAddress : ��ʼ��ַ
*			  _usSize : ���ݳ��ȣ���λΪ�ֽ�
*	�� �� ֵ: 0 ��ʾʧ�ܣ�1��ʾ�ɹ�
**********************************************************************************************************/
u8 E2_WriteBytes(u8 *_pWriteBuf, u16 _ucDevAddr, u16 _usAddress, u16 _usSize)
{

	u16 i,m;

	u16 usAddr;
	
	/* 
		д����EEPROM�������������������ȡ�ܶ��ֽڣ�ÿ��д����ֻ����ͬһ��page��
		����24xx02��page size = 8
		�򵥵Ĵ�����Ϊ�����ֽ�д����ģʽ��ÿд1���ֽڣ������͵�ַ
		Ϊ���������д��Ч��: ����������page wirte������
	*/

	usAddr = _usAddress;	

	for (i = 0; i < _usSize; i++)
	{
		/* �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ */
		if ((i == 0) || (usAddr & ( 64 - 1)) == 0)
		{
			/*���ڣ�������ֹͣ�źţ������ڲ�д������*/
			ICCARD_M3_I2C_Stop();
			
			/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms 			
			CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������*/
			for (m = 0; m < 100; m++)
			{
				/* ��1��������I2C���������ź� */
				ICCARD_M3_I2C_Start();
				
				/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
				ICCARD_M3_I2C_SendByte( _ucDevAddr | I2C_WR );	/* �˴���дָ�� */
				
				/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
				if (ICCARD_M3_I2C_CheckAck() == 0)
				{
					break;
				}

			}
			if (m  == 100)
			{
                ICCARD_M3_I2C_Stop();
                return 0;
			}
		
			/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
			ICCARD_M3_I2C_SendByte((u8)( usAddr >> 8 )&0x0FF   );
			
			/* ��5��������ACK */
			if (E2_Check_I2C_Ack())
			{
                return 0;	/* EEPROM������Ӧ�� */
			}
      
  			/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
			ICCARD_M3_I2C_SendByte((u8)usAddr & 0x0FF );
			
			/* ��5��������ACK */
			if (E2_Check_I2C_Ack())
			{
                return 0;	/* EEPROM������Ӧ�� */
			}     
		}	
		/* ��6������ʼд������ */
		ICCARD_M3_I2C_SendByte(_pWriteBuf[i]);
	
		/* ��7��������ACK */
		if (E2_Check_I2C_Ack())
		{
            return 0;	/* EEPROM������Ӧ�� */
		}
        
		usAddr++;	/* ��ַ��1 */
	}
	
	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	ICCARD_M3_I2C_Stop();
    ////////////
    return 1;
}
/*********************************************************************************************************
*	�� �� ��: ee_CheckOk
*	����˵��: �жϴ���EERPOM�Ƿ�����
*	��    �Σ���
*	�� �� ֵ: 1 ��ʾ������ 0 ��ʾ������
*********************************************************************************************************/
u8 EEPROM_HL_CheckOk(void)
{	
    if(ICCARD_M3_I2C_CheckDevice(EE_24XX_HL_DEV_ADDR ) == 0)
    {	
        return 1;	
    }
    else
    {		
        ICCARD_M3_I2C_Stop();           /* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */				
        return 0;
    }
}
/*********************************************************/
u8 EEPROM_24XX_HL_Byte_Write(const u16 Address, const u8 data)
{
    if( Address > EE_24XX_HL_SIZE ) 
    {
        assert_param(0);
        return(0);
    }
    //////////////
    return E2_WriteBytes( (u8 *) &data,EE_24XX_HL_DEV_ADDR, Address, 1 ); 
}

/*********************************************************/

u16 EEPROM_24XX_HL_Buf_Read(const u16 InAddr, u8 * pOutBuf,  u16 OutLength )
{
  if(( pOutBuf == NULL) || ( OutLength == 0 ) || ( InAddr > EE_24XX_HL_SIZE ) || ( InAddr + OutLength  > EE_24XX_HL_SIZE  )  )
  {
      assert_param(0);
      return(0);
  }  
  return E2_ReadBytes(pOutBuf,EE_24XX_HL_DEV_ADDR, InAddr, OutLength );
}

/*********************************************************/

u16 EEPROM_24XX_HL_Buf_Write( const u16 InAddr, u8 * pInBuf,  u16 InLength  )
{
  if(( pInBuf == NULL) || (InLength == 0) || ( InAddr > EE_24XX_HL_SIZE ) || ( InAddr+ InLength > EE_24XX_HL_SIZE ) )
  {
      assert_param(0);
      return(0);
  }
  return E2_WriteBytes( pInBuf,EE_24XX_HL_DEV_ADDR ,InAddr , InLength ); 
}

#ifdef EEPROM_24XX_USE_ALL_PAGE
/*********************************************************/

u16 EEPROM_24XX_HL_Buf_Write_By_Page( const u16 InPage, u8 * pInBuf , u16 InLength   )
{
    u8 *pIn ;

    u16 addr;
    
    addr = InPage*EE_24XX_HL_PAGE_SIZE;
    
    if( InLength < EE_24XX_HL_PAGE_SIZE )
    {
        return  0 ;
    }
    
    pIn = pInBuf ; 

    EEPROM_24XX_HL_Buf_Write( addr , pIn , EE_24XX_HL_PAGE_SIZE  );
 
    return 1;
}

/*********************************************************/

u8 EEPROM_24XX_HL_Init(void )
{
    if (ICCARD_M3_I2C_CheckDevice( EE_24XX_HL_DEV_ADDR ) == 0)
    {
	   return 1;
    }
    else
    {
    	ICCARD_M3_I2C_Stop();    /* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */		
		return 0;
    }
}

/*********************************************************/

u8 EEPROM_24XX_HL_SelCheck( void )
{
  
  u8 temp[ EE_24XX_HL_SELTEST_LEN ];

  u8 i;
    
  for( i = 0 ; i < sizeof( temp ) ; i++ )
  {

      temp[ i ] = i + 'A' ;

  }
  
  EEPROM_24XX_HL_Buf_Write(  EE_24XX_HL_SELTEST_ADDR  , temp ,  sizeof( temp )  );
  
  memset( temp , 0 , sizeof( temp ) );
  
  EEPROM_24XX_HL_Buf_Read(  EE_24XX_HL_SELTEST_ADDR  , temp ,  sizeof( temp )  );
  
  for( i = 0 ; i < sizeof( temp ) ; i++ )
  {

     if(  temp[ i ]  != ( i + 'A' ) )break;

  }
  
  if( i == sizeof( temp ) )
  {

      return 0; 

  }
  else
  {

      return 1; 

  }
  
}

/*********************************************************/

u16 EEPROM_24XX_HL_Buf_Read_By_Page( const u16 InPage, u8 * pOutBuf , u16 OutLength   )
{

    u8 *pOut ;

    u16 addr;
    
    addr = InPage*EE_24XX_HL_PAGE_SIZE;
    
    if( OutLength < EE_24XX_HL_PAGE_SIZE )
    {
        return  0 ;
    }
    
    pOut = pOutBuf ; 

    EEPROM_24XX_HL_Buf_Read( addr , pOut , EE_24XX_HL_PAGE_SIZE  );

    return 1;
}

/*********************************************************/

u8 EEPROM_24C128_AllChip_Read( void )
{  

    u16 iPage;
  
    u8 buf[EE_24XX_HL_PAGE_SIZE];
   
    if( buf == NULL )
    {
      return 0; 
    }
    
    for( iPage = 0 ; iPage < EE_24C128_PAGE_TOTAL ; iPage++ )
    {
        EEPROM_24XX_HL_Buf_Read_By_Page( iPage , buf ,  sizeof( buf ) );
     }    
    
    return 0 ;

}

u8 EEPROM_24C128_AllChip_Read_From_EEPROM2RAM( void  )
{  

    u16 iPage;

    memset(  g_sttAT24C128Ctrl.buf , 0 ,  sizeof( g_sttAT24C128Ctrl.buf )   );
    
    for( iPage = 0 ; iPage < EE_24C128_PAGE_TOTAL ; iPage++ )
    {

        EEPROM_24XX_HL_Buf_Read_By_Page( iPage , &g_sttAT24C128Ctrl.buf[ iPage*EE_24XX_HL_PAGE_SIZE ] , EE_24XX_HL_PAGE_SIZE );

    }
  
    g_sttAT24C128Ctrl.IsValid = 1;

    return 0 ;

}

u8 EEPROM_24C128_AllChip_Write_From_RAM2EEPROM( void  )
{  

    u16 iPage;    
   
    if( 1 != g_sttAT24C128Ctrl.IsValid )
    {

        return 0 ;

    }
    
    for( iPage = 0 ; iPage < EE_24C128_PAGE_TOTAL ; iPage++ )
    {

        EEPROM_24XX_HL_Buf_Write_By_Page( iPage , &g_sttAT24C128Ctrl.buf[ iPage*EE_24XX_HL_PAGE_SIZE ] , EE_24XX_HL_PAGE_SIZE );

    }  

    return 0 ;

}
#endif

u8 SL4442_Buffer_Read(  u8 *pBuf, u16  LengthBuf )
{

    u8 sl4442pwd[3]={ 0xFF, 0xFF, 0xFF };

    u8 ret;

    u8 i;

    LengthBuf = LengthBuf;

    ret =Power_On();

    if( ret == 1)
    {
        for(i=1;i<4;i++) //У��3�ֽڵ�����
        {
            SendComm(CVD_COMM,i,sl4442pwd[i-1] ); //����У������
            Proce_Mod();              //����
        }

        Rmm( 0x00 , pBuf , 128 );

        return 	1;
    } 
    else 
    {
        return 0;
    }
}
/******************************************************************************
**                            End Of File
******************************************************************************/

