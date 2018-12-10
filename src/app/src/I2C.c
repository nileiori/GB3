/******************************************************************** 
//��Ȩ˵��  :
//�ļ�����  :I2C.c      
//����      :I2C�ӿڲ�������
//�汾��    :
//������    :yjb
//����ʱ��  :2012.6
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      : 1) ����ʱ,�ظ��򿪽��ᵼ�µڶ��δ򿪺��д���ݳ���.
//              : 2)ʹ������:
//              :   u8 tmp = 0x7B;
//              :       u8 ret = 0;
//              :       ret = I2C_Config();
//              :        if(ret == 1)                          //��ʼ�ɹ�
//              :        {
//              :          ret = I2C_ReadDat(0x00,&tmp);        //��һ���ֽ�,��Ҫ�жϷ���ֵ�Ƿ�ʧ��        
//              :          ret = I2C_WriteDat(0x0A,0x7B);       //дһ���ֽ�,��Ҫ�жϷ���ֵ�Ƿ�ʧ��
//              :          ret = I2C_ReadDat(0x0A,&tmp);        //��һ���ֽ�,��Ҫ�жϷ���ֵ�Ƿ�ʧ��
//              :        }
//              :    ע��:��ʼ��,��д�������з���ֵ��˵������ȷ��100%�����ɹ�����Ҫ�жϷ���ֵ.
***********************************************************************/
//****************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "HAL.h"
#include <stdio.h>

#define     true    1
#define     false   0
//****************�궨��********************
#define   G_SENSOR_ADDR       (0x18<<1)        //BMA250��ַ

#define   TIME_OUT_COUNT      1000      //1000*1000       //��ʱ����ʱ��(ҪС�ڿ��Ź�ʱ��)
//****************�ⲿ����*******************

//****************ȫ�ֱ���*******************
u8  g_I2C_status;
//****************��������*******************

//****************��������******************
//����״̬
void    I2c_SetStatus(u8 status)
{
        g_I2C_status=status;
}

//��ȡI2C����״̬,����0 ��������������1����������
u8 I2c_GetStatus(void)
{
    return  g_I2C_status;
}
//Ӳ��ʱ
//
//
void I2C_Delay(__IO uint32_t nCount)
{
    while(nCount--)
    {
    }    
}

/****************************************************************
//��������  :I2C_Config
//����      :I2C�ӿ�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :ʧ�ܷ���0,�ɹ�����1
//��ע      :
*********************************************************************/
u8 I2C_Config(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure; 
        //u16 timeoutmain = 0 ;  //����ʱ����
        u16 timeout = 0;       //�μ���ʱ����
    
        //���õ�ַģʽ
        GpioOutInit(SHOCK_CS);
        GpioOutOff(SHOCK_CS);       
        
        //ʹ��I2Cʱ��
        RCC_APB1PeriphClockCmd(I2Cx_CLK,ENABLE);
        
        //��λI2Cʱ�� - ��ֹ�ظ���ʱ�򲻶�
        RCC_APB1PeriphResetCmd(I2Cx_CLK, ENABLE);
        RCC_APB1PeriphResetCmd(I2Cx_CLK, DISABLE);        
        
        RCC_APB2PeriphClockCmd(I2Cx_SDA_GPIO_CLK | I2Cx_SCL_GPIO_CLK,DISABLE);
        RCC_APB2PeriphClockCmd(I2Cx_SDA_GPIO_CLK | I2Cx_SCL_GPIO_CLK,ENABLE);
    
        /* PF0,1 SCL and SDA */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        //GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        //GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
        
        GPIO_InitStructure.GPIO_Pin =  I2Cx_SCL_PIN;
        GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin =  I2Cx_SDA_PIN;                    //�����ڳ�ʼ��SCLǰ��ʼ��SDA IO��.
        GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);             //I2C�ӿ�ʹ�õ�GPIO�ܽų�ʼ��
        
        //ӳ�� I2C_SCL , I2C_SDA
       // GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_SOURCE, I2Cx_SCL_AF);
       // GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_SOURCE, I2Cx_SDA_AF); 
        GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
             
        
        I2C_DeInit(I2Cx);
        I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;         //����I2C�ӿ�ģʽ 
        I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; //����I2C�ӿڵĸߵ͵�ƽ����
        I2C_InitStructure.I2C_OwnAddress1 = 0x30;          //����I2C�ӿڵ�������ַ
        I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;        //�����Ƿ���ACK��Ӧ          
        I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //��ַģʽ,7�ֽ�ģʽ
        I2C_InitStructure.I2C_ClockSpeed = 100000;        //100K�ٶ�
    
        I2C_Cmd(I2Cx, ENABLE);                            //ʹ��I2C�ӿ�
        I2C_Init(I2Cx, &I2C_InitStructure);               //I2C�ӿڳ�ʼ��
       
        
        //�ȴ�I2C���߿���
        do
        {     
            /*��ʼλ*/
            I2C_GenerateSTART(I2Cx, ENABLE);
            I2C_ReadRegister(I2Cx, I2C_Register_SR1);  //��״̬
            I2C_Delay(300*1000);                      //��������ӳ�,��֪���͵�ַ�����.
            I2C_Delay(300*1000); 
            I2C_Delay(300*1000); 
            I2C_Delay(300*1000); 
            I2C_Send7bitAddress(I2Cx, G_SENSOR_ADDR, I2C_Direction_Transmitter);
            if(timeout++ > 3)
            {
                break;
            }
        }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
        
        I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
        /*ֹͣλ*/    
        I2C_GenerateSTOP(I2Cx, ENABLE);      
        
        if(timeout > 3)                                 //���ǳ�ʱ,˵����ʼ������.
        {
            return 0;
        }else{
             return 1;
        }
}
/****************************************************************
//��������  :I2C_ReadDat
//����      :�ӵ�ַaddr��ȡһ���ֽ�
//����      :addr ��ַ
//���      :dat   �����ֽ�����ָ��
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :ʧ�ܷ���0,�ɹ�����1
//��ע      :
*********************************************************************/
u8 I2C_ReadDat(u8 addr,u8 *dat)
{
        u32 timeout = 0;
        
        timeout = 0;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                            /*ֹͣλ*/    
                    I2C_GenerateSTOP(I2Cx, ENABLE);                 
                return 0;
            }
        };          

    /* ������ʼλ */
        timeout = 0;
        I2C_GenerateSTART(I2Cx, ENABLE);
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))/*EV5,��ģʽ*/
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };           
      
        /*����������ַ(д)*/
        timeout = 0;
        I2C_Send7bitAddress(I2Cx,  G_SENSOR_ADDR, I2C_Direction_Transmitter);      //Ĭ��
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };       
        /*���͵�ַ*/
        timeout = 0;
        I2C_SendData(I2Cx, addr);
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))/*�����ѷ���*/
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };           
                
        /*��ʼλ*/
        timeout = 0;
        I2C_GenerateSTART(I2Cx, ENABLE);
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };           
        
        /*������*/
        timeout = 0;
        I2C_Send7bitAddress(I2Cx, G_SENSOR_ADDR, I2C_Direction_Receiver);
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };             
    
        I2C_GenerateSTOP(I2Cx, ENABLE);     //����ֹͣλ    

        timeout = 0;
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) /* EV7 */
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };               
        *dat =  I2C_ReceiveData(I2Cx);
         
        I2c_SetStatus(false);
        return 1;
}
/****************************************************************
//��������  :I2C_WriteDat
//����      :���ַaddrдһ���ֽ�
//����      :addr ��ַ
//���      :dat  ��д������
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :ʧ�ܷ���0,�ɹ�����1
//��ע      :
*********************************************************************/
u8 I2C_WriteDat(u8 addr,u8 dat)
{
        u32 timeout = 0;
        
        /* ��ʼλ */
        timeout = 0;
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };

    /* ����������ַ(д)*/
        timeout = 0;
    I2C_Send7bitAddress(I2Cx, G_SENSOR_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };          
  
    /*���͵�ַ*/
        timeout = 0;
    I2C_SendData(I2Cx, addr);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };              

    /* дһ���ֽ�*/
        timeout = 0;
    I2C_SendData(I2Cx, dat); 
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(timeout++ > TIME_OUT_COUNT)
            {
                I2c_SetStatus(true);
                return 0;
            }
        };          
    
    /* ֹͣλ*/
    I2C_GenerateSTOP(I2Cx, ENABLE);
        I2c_SetStatus(false);
        return 1;
  
}
/****************************************************************
//��������  :I2C_Test
//����      :������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void I2C_Test(void)
{
    u8 tmp = 0x7B;
        u8 ret = 0;
        ret = I2C_Config();
        if(ret == 1)                          //��ʼ�ɹ�
        {
          ret = I2C_ReadDat(0x00,&tmp);        //��һ���ֽ�
          ret = ret?ret:ret;          
          ret = I2C_WriteDat(0x0A,0x7B);        //дһ���ֽ�
          tmp = tmp?tmp:tmp;
          ret = ret?ret:ret;          
          tmp = 0;
          ret = I2C_ReadDat(0x0A,&tmp);        //��һ���ֽ�
          tmp = tmp?tmp:tmp;
          ret = ret?ret:ret;
        }
}