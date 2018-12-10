/*************************************************************************
* Copyright (c) 2014 All rights reserved., 
* �ļ����� : ICCARD_M3.c
* ��ǰ�汾 : 0.1
* ������   : MARK
* �༭ʱ�� : 2014��03��17��
* ����ʱ�� ��
*************************************************************************/



/****************************�ļ�Ԥ����**********************************/

 #include  "ICCARD_M3.h" //����IC�������������������õ�ͷ�ļ�
 #include  "ICCARD_E2.h"
 
 //#define NOP() asm("nop")
 
 #define NOP() __nop()
/***********************************************************************/


/***************************��ʼ������**********************************/ 

/***************************�ܽ�����************************************/ 
/*************************************************************
** ��������: ICCARD_M3_IO_SDA_OUT
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void ICCARD_M3_SDA_OUT(void)
{
	/*
    GPIO_InitTypeDef GPIO_InitStructure;
  
  
    RCC_AHB1PeriphClockCmd(  RCC_ICCARD_IO , ENABLE);	       

    GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_IO  ;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  	         
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;           
    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           
    
    GPIO_Init( GPIO_ICCARD_IO , &GPIO_InitStructure);
	*/
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_ICCARD_IO, ENABLE);
  GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_IO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_ICCARD_IO, &GPIO_InitStructure);
}
/*************************************************************
** ��������: ICCARD_M3_IO_SDA_IN
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void ICCARD_M3_SDA_IN(void)
{
	/*
    GPIO_InitTypeDef GPIO_InitStructure;
  
 
    RCC_AHB1PeriphClockCmd(  RCC_ICCARD_IO , ENABLE);	       

    GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_IO  ;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  	        
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;         
    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;           
    
    GPIO_Init( GPIO_ICCARD_IO , &GPIO_InitStructure);
	*/
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_ICCARD_IO, ENABLE);
  GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_IO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_ICCARD_IO, &GPIO_InitStructure);
	
}
/*************************************************************
** ��������: ICCARD_M3_IO_SDA_IN
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void ICCARD_M3_CLK_OUT(void)
{
	/*
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(  RCC_ICCARD_CLK  , ENABLE);	    
  
    GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_CLK ;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;      	  
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;          
    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           
    
    GPIO_Init( GPIO_ICCARD_CLK , &GPIO_InitStructure );
	*/
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_ICCARD_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_CLK;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_ICCARD_CLK, &GPIO_InitStructure);
}
/*************************************************************
** ��������: ICCARD_M3_IO_SDA_IN
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void ICCARD_M3_Init(void)
{
	
    GPIO_InitTypeDef GPIO_InitStructure;
  

    ICCARD_M3_SDA_OUT();
  
    ICCARD_M3_CLK_OUT();
     
	/*
    RCC_AHB1PeriphClockCmd(  RCC_ICCARD_RST , ENABLE);	   

    GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_RST  ;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  	        
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         
    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        
    
    GPIO_Init( GPIO_ICCARD_RST , &GPIO_InitStructure);
    

    RCC_AHB1PeriphClockCmd(  RCC_ICCARD_DETECT  , ENABLE);	
  
    GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_DETECT ;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;      
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          
    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         
    
    GPIO_Init( GPIO_ICCARD_DETECT , &GPIO_InitStructure );
    
   
    RCC_AHB1PeriphClockCmd(  RCC_ICCARD_POWER , ENABLE);	

    GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_POWER  ;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       	
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          
    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        
    
    GPIO_Init( GPIO_ICCARD_POWER , &GPIO_InitStructure);

     
    RCC_AHB1PeriphClockCmd(  RCC_BMA250_SHOCK_CS , ENABLE);	

    GPIO_InitStructure.GPIO_Pin = PIN_BMA250_SHOCK_CS ;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       	
    
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         
    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;         
    
    GPIO_Init( GPIO_BMA250_SHOCK_CS , &GPIO_InitStructure);
   */
	 
		RCC_APB2PeriphClockCmd(RCC_ICCARD_RST, ENABLE);
		GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_RST;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIO_ICCARD_RST, &GPIO_InitStructure);
		
		RCC_APB2PeriphClockCmd(RCC_ICCARD_DETECT, ENABLE);
		GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_DETECT;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIO_ICCARD_DETECT, &GPIO_InitStructure);
		
		RCC_APB2PeriphClockCmd(RCC_ICCARD_POWER, ENABLE);
		GPIO_InitStructure.GPIO_Pin = PIN_ICCARD_POWER;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIO_ICCARD_POWER, &GPIO_InitStructure);
		
		//RCC_APB2PeriphClockCmd(RCC_BMA250_SHOCK_CS, ENABLE);
		//GPIO_InitStructure.GPIO_Pin = PIN_BMA250_SHOCK_CS;
		//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
		//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		//GPIO_Init(GPIO_BMA250_SHOCK_CS, &GPIO_InitStructure);
    
    ICCARD_SDA_RESET();                          

    ICCARD_CLK_RESET();                         

    ICCARD_RST_RESET();                         

    ICCARD_DETECT_RESET();                      

    ICCARD_POWER_ON();                          

    //BMA250_SHOCK_CS_RESET();                    

    ICCARD_M3_I2C_Stop();		
		

}

/*********************************��ʱ����****************************/
void ICCARD_M3_I2C_Delay(void)
{
	/*
	// dxl,2016.3.23����
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	*/
	
	
	u32 i;
  for(i=0; i<20; i++)
	{
	
	}
	
}

/*********************************��I2Cʱ����************************/ 

/**********************************I2C-->START*************************/
void ICCARD_M3_I2C_Start(void)
{
    ICCARD_M3_SDA_OUT();

    ICCARD_SDA_SET();    

    ICCARD_CLK_SET();

    ICCARD_M3_I2C_Delay();

    ICCARD_SDA_RESET();

    ICCARD_M3_I2C_Delay();

    ICCARD_CLK_RESET();

    ICCARD_M3_I2C_Delay();
}

/**********************************I2C-->STOP*************************/
void ICCARD_M3_I2C_Stop(void)
{
    ICCARD_M3_SDA_OUT();

    ICCARD_SDA_RESET();

    ICCARD_CLK_SET();

    ICCARD_M3_I2C_Delay();

    ICCARD_SDA_SET();
    /////////////////////
    ICCARD_M3_I2C_Delay();
}

/**********************************I2C-->SENDBYTE*************************/
void ICCARD_M3_I2C_SendByte(u8 Byte )
{

    u8 i;

    ICCARD_M3_SDA_OUT();
    

    for (i = 0; i < 8; i++)   /* �ȷ����ֽڵĸ�λbit7 */
    {		

        if ( Byte & 0x80)
        {
            ICCARD_SDA_SET();
        }
        else
        {
            ICCARD_SDA_RESET();
        }

        ICCARD_M3_I2C_Delay();

        ICCARD_CLK_SET();

        ICCARD_M3_I2C_Delay();

        ICCARD_CLK_RESET();

        //if (i == 7)
        //{
        //    ICCARD_SDA_SET();        /* �ͷ����� */
        //}

        Byte<<= 1;	             /* ����һ��bit */

        ICCARD_M3_I2C_Delay();
    }
    ////////////////////
    ICCARD_M3_I2C_Delay();
    ICCARD_M3_I2C_Delay();
    ICCARD_M3_I2C_Delay();
    ICCARD_M3_I2C_Delay();
}

/**********************************I2C-->READBYTE*************************/
u8 ICCARD_M3_I2C_ReadByte(void)
{
    u8 i;

    u8 value;

    ICCARD_M3_SDA_IN();

    ICCARD_CLK_RESET();
    /////////////////
    ICCARD_M3_I2C_Delay();

    value = 0;

    for (i = 0; i < 8; i++)    /* ������1��bitΪ���ݵ�bit7 */
    {
        ICCARD_CLK_SET();

        ICCARD_M3_I2C_Delay();

        value <<= 1;

        if (ICCARD_SDA_READ())
        {
            value |= 0x01;
        }
        else
        {
            value &= 0xFE;
        }

        ICCARD_CLK_RESET();

        ICCARD_M3_I2C_Delay();
    }

    ICCARD_M3_SDA_OUT();

    ICCARD_M3_I2C_Delay();

    return value;

}

/**********************************I2C-->WAITACK*************************/
u8 ICCARD_M3_I2C_CheckAck(void)
{
    u32 count;
    u8 Ack;

    ICCARD_M3_SDA_IN();

    ICCARD_SDA_SET();            	/* CPU�ͷ�SDA���� */

    ICCARD_M3_I2C_Delay();

    ICCARD_CLK_SET();            	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */

    ICCARD_M3_I2C_Delay();
    /*
    if (ICCARD_SDA_READ())	        // CPU��ȡSDA����״̬  dxl,2016.3.23����
    {
        Ack = 1;
    }
    else
    {
        Ack = 0;
    } 
		*/
		
		count = 0;
		Ack = 0;
		do
		{
			 count++;
		   Ack = ICCARD_SDA_READ();	        // CPU��ȡSDA����״̬ 
		}while((count < 100)&&(1 == Ack));
    
    ICCARD_CLK_RESET();

    ICCARD_M3_I2C_Delay();

    return Ack;

}

/**********************************I2C-->ACK*************************/ 
void ICCARD_M3_I2C_SendAck(void)
{
    ICCARD_M3_SDA_OUT();
    ////////////
    ICCARD_SDA_RESET();	            /* CPU����SDA = 0 */

    ICCARD_M3_I2C_Delay();

    ICCARD_CLK_RESET();

    ICCARD_M3_I2C_Delay();

    ICCARD_CLK_SET();	            /* CPU����1��ʱ�� */

    ICCARD_M3_I2C_Delay();

    ICCARD_CLK_RESET();

    ICCARD_M3_I2C_Delay();

    ICCARD_SDA_SET();	            /* CPU�ͷ�SDA���� */

    ICCARD_M3_I2C_Delay();
}

/**********************************I2C-->NACK*************************/ 
void ICCARD_M3_I2C_SendNAck(void)
{
    ICCARD_M3_SDA_OUT();

    ICCARD_SDA_SET();	           /* CPU����SDA = 1 */

    ICCARD_M3_I2C_Delay();

    ICCARD_CLK_RESET();

    ICCARD_M3_I2C_Delay();

    ICCARD_CLK_SET();	           /* CPU����1��ʱ�� */

    ICCARD_M3_I2C_Delay();

    ICCARD_CLK_RESET();  

    ICCARD_M3_I2C_Delay();
}

/*************************I2C-->CHECKDEVICE************************/
u8 ICCARD_M3_I2C_CheckDevice(u8 Address)
{  
    u8 Ack;

    ICCARD_M3_Init();		                      /* ����GPIO */

    ICCARD_M3_I2C_Stop();			              /* ����ֹͣ�ź� */

    ICCARD_M3_I2C_Start();		              /* ���������ź� */

    ICCARD_M3_I2C_SendByte( Address | I2C_WR);

    Ack = ICCARD_M3_I2C_CheckAck();           	  /* ����豸��ACKӦ�� */

    ICCARD_M3_I2C_Stop();			              /* ����ֹͣ�ź� */

    return Ack;
}
/**************************SPI--->POWER_ON***********************/
void M3_I2C_Delays(unsigned char n)
{
		u8	i;
    for(i=0;i<n;i++)
    ICCARD_M3_I2C_Delay();  
}
/**************************SPI--->POWER_ON***********************/
u8 Power_On(void)
{

    ICCARD_SDA_SET();

    ICCARD_SDA_SET();

    ICCARD_RST_SET();

    M3_I2C_Delays(50);

    ICCARD_RST_SET();


    M3_I2C_Delays(10);

    ICCARD_CLK_SET();

    M3_I2C_Delays(20);

    ICCARD_CLK_RESET();

    M3_I2C_Delays(10);

    ICCARD_RST_RESET();

    M3_I2C_Delays(20);              //������λʱ��

    InByte();
    InByte();
    InByte();
    InByte();

    InByte();
    InByte();

    ICCARD_CLK_SET();

    M3_I2C_Delays(20); 

    ICCARD_CLK_RESET();

    M3_I2C_Delays(20);

    ICCARD_CLK_SET();

    M3_I2C_Delays(20);

    ICCARD_CLK_RESET();
    
    M3_I2C_Delays(20);

    return 1;

}

/*******************************SPI--->INBYTE******************/
u8 InByte(void)
{
    u8 i,a;

    a=0;

    ICCARD_SDA_SET();//��Ϊ����״̬

    ICCARD_SDA_SET();//��Ϊ����״̬

    for (i=8;i>0;i--)
    {

        a=a>>1;

        if(ICCARD_SDA_READ())
        {
            a|=0x80;
        }

        ICCARD_CLK_SET();

        M3_I2C_Delays(20);

        ICCARD_CLK_RESET();

        M3_I2C_Delays(20);

    }

    return(a);

}

/****************************SPI--->OUTBIT**********************/
void OutByte(u8 ch)
{
    u8 i;

    for (i=8;i>0;i--)
    {

        if(ch&0x01)
        {
            ICCARD_SDA_SET();
        }   
        else
        {
            ICCARD_SDA_RESET();
        }

        ICCARD_CLK_SET();

        M3_I2C_Delays(20);

        ICCARD_CLK_RESET();

        M3_I2C_Delays(20);
        
        ch=ch>>1;                 //����һλ

    }

}

/****************************SPI--->STARTCOMM*******************/
void Start_Comm(void)
{

    ICCARD_SDA_SET();

  	ICCARD_SDA_SET();

    M3_I2C_Delays(20);
    
    ICCARD_CLK_SET();

    M3_I2C_Delays(20);

    ICCARD_SDA_RESET();

  	ICCARD_SDA_RESET();

    M3_I2C_Delays(20);

    ICCARD_CLK_RESET();

    M3_I2C_Delays(20);
    
}

/****************************SPI--->STOPCOMM********************/
void Stop_Comm(void)
{

    ICCARD_SDA_RESET();
    
    M3_I2C_Delays(40);

    ICCARD_CLK_SET();

    M3_I2C_Delays(10);
    
    ICCARD_SDA_SET();

    M3_I2C_Delays(10);

}

/****************************SPI--->SENDCOMM********************/
void SendComm(u8 a,u8 b,u8 c)
{
    Start_Comm();     //��ʼ��������
    OutByte(a);          //��������
    OutByte(b);          //����ַ
    OutByte(c);          //������
    Stop_Comm();      //������������
}

/***************************SPI--->PRO_MOD*********************/
void Proce_Mod(void)
{

    u8 i;

    ICCARD_CLK_RESET();

    M3_I2C_Delays(10);

    ICCARD_SDA_SET();

    for (i=250;i>0;i--)
    {

        ICCARD_CLK_SET();

        M3_I2C_Delays(20);

        ICCARD_CLK_RESET();

        M3_I2C_Delays(20);

        if(ICCARD_SDA_READ()) //����Ϊ�߾�����

        break;

    }
    //�������10ms  INDATA δ�ָ��ߣ���δ���
}

/*************************SPI--->READ_MOD************************/
void Read_Mod(u8  *pt,u8 i)
{
    u8  temp;

    ICCARD_CLK_RESET();

    M3_I2C_Delays(20);

    ICCARD_CLK_SET();

    do
    {
        temp=InByte();    //����һ���ֽ�

        *pt=temp;    //����һ���ֽ�

        pt++;            //ָ���һ

    }while(--i);          //��������һ,�ж�

}

/*************************SPI--->RSTCARD************************/
void RstCard(void)
{

    ICCARD_RST_SET();

    M3_I2C_Delays(10);

    ICCARD_CLK_SET();

    M3_I2C_Delays(20);

    ICCARD_CLK_RESET();

    M3_I2C_Delays(10);

    ICCARD_RST_RESET();

    M3_I2C_Delays(10);    //������λʱ��

    InByte();

    InByte();

    InByte();

    InByte();  //�ն�4��

}

/***********************SPI--->Rmm******************************/
void Rmm(u8   cardAdd, u8    *pt,u8    i)
{
    SendComm(RMM_COMM,cardAdd, 0 );

    Read_Mod(pt,i);

    RstCard();

}
/******************************************************************************
**                            End Of File
******************************************************************************/

