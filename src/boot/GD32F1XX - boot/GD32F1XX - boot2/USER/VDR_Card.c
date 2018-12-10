/************************************************************************
//�������ƣ�VDR_Card.c
//���ܣ�ˢ��ģ��
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�������
//����ʱ�䣺2015.8
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/
#include "stm32f10x.h"
#include "VDR.h"



/********************���ر���*************************/
static	u8	CardDataBuffer[VDR_CARD_BUFFER_SIZE] = {0};//��ȡ��IC�����ݷ��ڸû���
static	u8	CardState = 0;//��״̬
static	u8	CardReadStep = 0;//��������

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static void VDRCard_Init(void);
static void VDRCard_StateMonitor(void);
static void VDRCard_StateRead(void);
static void VDRCard_StateLogin(void);
static void VDRCard_StateLogout(void);
static u8 VDRCard_IsInsert(void);
static u8 VDRCard_GetLicense(u8 *pLicense, u8 *pCard);

/********************��������*************************/

/**************************************************************************
//��������VDRCard_TimeTask
//���ܣ�ˢ��ǩ�����ο�ǩ��
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע��0.1���ӽ���1�Σ������������Ҫ���ô˺�����
//ע���������Ҫ�ڳ�ʱ��ʻ��ʱ������ɳ�ʼ������ܿ���
***************************************************************************/
FunctionalState VDRCard_TimeTask(void)
{
	static	u8	InitFlag = 0;//��ʼ����־��1Ϊ�ѳ�ʼ����0Ϊδ��ʼ��

	if(0 == InitFlag)
	{
		if(1 == VDROvertime_GetInitFlag())
		{
			InitFlag = 1;
			VDRCard_Init();
			CardState = CARD_STATE_MONITOR;
		}
	}
	else
	{
		switch(CardState)
		{
			case CARD_STATE_MONITOR://����⵽�п�����ʱ��״̬תΪ��������⵽�п��γ�ʱ״̬תΪǩ��
			{
				VDRCard_StateMonitor();
				break;
			}
			case CARD_STATE_READ://�����ɹ���״̬תΪ��¼��ʧ�ܺ�תΪ���
			{
				VDRCard_StateRead();
				break;
			}
			case CARD_STATE_LOGIN://��ɵ�¼��״̬תΪ���
			{
				VDRCard_StateLogin();
				break;
			}
			case CARD_STATE_LOGOUT://��ɵ�¼��״̬תΪ���
			{
				VDRCard_StateLogout();
				break;
			}
			default:
			{
				CardState = CARD_STATE_MONITOR;
				break;
			}
		}

	}

	return ENABLE;		
}

/////////////////////////////�������ڲ�����///////////////////////// 
/**************************************************************************
//��������VDRCard_Init
//���ܣ�ģ���ʼ��
//���룺��
//�������
//����ֵ��1:��ʼ�����;0:��ʼ��δ���
//��ע��
***************************************************************************/
static void VDRCard_Init(void)
{
	ICCARD_M3_Init();
}
/**************************************************************************
//��������VDRCard_StateMonitor
//���ܣ�����
//���룺��
//�������
//����ֵ����
//��ע��
//1.ͣ��ʱ��⵽�п�����ʱ����ת������״̬
//2.ͣ��ʱ��⵽�п��γ�ʱ����ת��ǩ��״̬
//3.��ʻ��ͣʻʱ���п�����ת���������޿�����ת��ǩ�ˡ�
//4.��ʻ�в忨��ο���Ч
***************************************************************************/
static void VDRCard_StateMonitor(void)
{
	static	u8	LastCardInsertFlag = 0;//�忨��־��1Ϊ�Ѳ忨��0Ϊδ�忨
	static	u8	LastCarRunFlag = 0;//��ʻ��־��1Ϊ��ʻ��0Ϊͣʻ

	u8	CardInsertFlag = 0;
	u8	CarRunFlag = 0;

	CarRunFlag = VDRDoubt_GetCarRunFlag();
	CardInsertFlag = VDRCard_IsInsert();

	if((1 == CardInsertFlag)&&(0 == LastCardInsertFlag)&&(0 == CarRunFlag))//ͣ���忨
	{
		CardState = CARD_STATE_READ;
	}
	else if((0 == CardInsertFlag)&&(1 == LastCardInsertFlag)&&(0 == CarRunFlag))//ͣ���ο�
	{
		CardState = CARD_STATE_LOGOUT;
	}

	if((1 == LastCarRunFlag)&&(0 == CarRunFlag)&&(1 == CardInsertFlag))//����ʻ��ͣʻʱ�п�
	{
		CardState = CARD_STATE_READ;
	}
	else if((1 == LastCarRunFlag)&&(0 == CarRunFlag)&&(0 == CardInsertFlag))//����ʻ��ͣʻʱ�޿�
	{
		CardState = CARD_STATE_LOGOUT;
	}

	LastCarRunFlag = CarRunFlag;
	LastCardInsertFlag = CardInsertFlag;
}
/**************************************************************************
//��������VDRCard_StateRead
//���ܣ�����
//���룺��
//�������
//����ֵ����
//��ע��0.1���ӽ���1�Σ����ж��Ƿ�Ϊ24XX����Ȼ�����ж��Ƿ�Ϊ4442����
//��ȡ�����ݷ���CardDataBuffer�����У�CardDataBuffer[0]��ʾ��д�����1Ϊ�ɹ���0��ʧ�ܣ����������Ϊ��ȡ������
***************************************************************************/
static void VDRCard_StateRead(void)
{
	static	u8	count = 0;
	u8 Addr;
	u8	Len;
	
	switch(CardReadStep)
	{
		case	CARD_READ_START:
		{
			count = 0;
			CardReadStep++;
			break;
		}
		case	CARD_READ_24XX:
		{
			count++;
			if(count >= 5)
			{
				CardDataBuffer[0] = 0;
				Addr = 0;
				Len = 128;//���꿨����
				if(0 == EEPROM_24XX_HL_Buf_Read(Addr, CardDataBuffer+1,Len))
				{
					CardReadStep++;
				}
				else
				{
					CardDataBuffer[0] = 1;
					CardState = CARD_STATE_LOGIN;
				}
				count = 0;
			}
			break;
		}
		case	CARD_READ_4442:
		{
			count++;
			if(count >= 5)
			{
				Len = 128;//���꿨����
				if(0 == SL4442_Buffer_Read(CardDataBuffer+1,Len))
				{
					CardReadStep++;
				}
				else
				{
					CardDataBuffer[0] = 1;
					CardState = CARD_STATE_LOGIN;
				}
				count = 0;
			}
			break;
		}
		default: 
		{
			CardState = CARD_STATE_MONITOR;
			break;
		}
	}
}
/**************************************************************************
//��������VDRCard_StateLogin
//���ܣ�IC��ǩ��
//���룺��
//�������
//����ֵ����
//��ע��0.1���ӽ���1��
//CardDataBuffer[0]��ʾ��д�����1Ϊ�ɹ���0��ʧ�ܣ����������Ϊ��ȡ������
***************************************************************************/
static void VDRCard_StateLogin(void)
{
	u8	Buffer[20];
	u8	BufferLen;
	
	if(1 == CardDataBuffer[0])
	{
		BufferLen = VDRCard_GetLicense(Buffer, CardDataBuffer+1);
		if(18 == BufferLen)
		{
			VDROvertime_DriverLogin(Buffer,18);
		}
		
		CardDataBuffer[0] = 0;
	}
	CardState = CARD_STATE_MONITOR;
}
/**************************************************************************
//��������VDRCard_StateLogout
//���ܣ�IC��ǩ��
//���룺��
//�������
//����ֵ����
//��ע��0.1���ӽ���1��
//CardDataBuffer[0]��ʾ��д�����1Ϊ�ɹ���0��ʧ�ܣ����������Ϊ��ȡ������
***************************************************************************/
static void VDRCard_StateLogout(void)
{
	VDROvertime_DriverLogout();
	CardState = CARD_STATE_MONITOR;
}


static u8 VDRCard_IsInsert(void)
{
	static u8	InCount = 0;
	static u8	OutCount = 0;
	
	static u8	InsertFlag = 0;
	
	if(ICCARD_DETECT_READ())//�޿�
  {
		InCount = 0;
		OutCount++;
		if(OutCount >= 10)
		{
			OutCount--;
			InsertFlag = 0;
		}
  }
  else//�п�
  {
    OutCount = 0;
		InCount++;
		if(InCount >= 10)
		{
			InCount--;
			InsertFlag = 1;
		} 
  }
	
	return InsertFlag;
}
/**************************************************************************
//��������VDRCard_GetLicense
//���ܣ��ӿ�Ƭ�����л�ȡ��ʻ֤����
//���룺pCard:ָ���ȡ���Ŀ�Ƭ����
//�����pLicenseָ���ȡ���ļ�ʻ֤����
//����ֵ��0:��ʾδ��ȡ���Ϸ��ļ�ʻ֤����;18��ʾ��ȡ���Ϸ��ļ�ʻ֤���룬����Ϊ18
//��ע���ɼ����б��ʽ���͹����ʽ��
***************************************************************************/
static u8 VDRCard_GetLicense(u8 *pLicense, u8 *pCard)
{
	u8	*p = NULL;
	u8	i;
	u8	temp;
	u8	flag;
	
	p = pCard;
	
	//if(*(p+127) == Public_GetXorVerify(p,127))//4442��û�м�У�飬���ȥ��
	//{
		
		//���ж��Ƿ�Ϊ���꿨
		flag = 0;
		p = pCard+32;//����ʻ֤����
		for(i=0; i<18; i++)
		{
			temp = *p++;
			if(((temp >= '0')&&(temp <= '9'))
				||('x'==temp)
			    ||('X' == temp)
						||(' ' == temp)
							||(0 == temp))
			{

			}
			else
			{
					flag = 1;
					break;
			}
		}
		if(0 == flag)//����ʻ֤��Ч��
		{
			p = pCard+50;
			temp = *p++;
			if(temp <= 0x99)
			{
				
			}
			else
			{
				flag = 1;
			}
			temp = *p++;
			if((temp >= 0x01)&&(temp <= 0x12))
			{
				
			}
			else
			{
				flag = 1;
			}
			temp = *p++;
			if((temp >= 0x01)&&(temp <= 0x31))
			{
				
			}
			else
			{
				flag = 1;
			}
		}
		if(0 == flag)//�ǹ��꿨
		{
			p = pCard+32;
			memcpy(pLicense,p,18);
			for(i=0; i<18; i++)
			{
				if(0 == pLicense[i])
				{
					//pLicense[i] = ' ';
				}
			}
			return 18;
		}
		
	//}

	//���ж��Ƿ�Ϊ�б꿨
	p = pCard;
	temp = *p++;
	//if(0x00==(temp&0x0f))//��Ƭ����Ϊ0�������Ǽ�ʻԱ��ݿ�
	//{
		temp = *p++;//��ʻԱ�������ȣ�1�ֽ�
		p += temp;//������ʻԱ����
		for(i=0; i<18; i++)
		{
			temp = *p++;
			if(((temp >= '0')&&(temp <= '9'))
				||('x'==temp)
					||('X' == temp)
						||(' ' == temp)
							||(0 == temp))
			{

			}
			else
			{
				return 0;
			}
		}
		p -= 18;
		memcpy(pLicense,p,18);
		for(i=0; i<18; i++)
		{
			if(0 == pLicense[i])
			{
					//pLicense[i] = ' ';
			}
		}
	//}
	//else
	//{
		//return 0;
	//}
	
	return 18;
}






