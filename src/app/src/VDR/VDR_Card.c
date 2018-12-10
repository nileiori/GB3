/************************************************************************
//�������ƣ�VDR_Card.c
//���ܣ�ˢ��ģ��
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2015.8
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��

*************************************************************************/

/********************�ļ�����*************************/

#include "VDR.h"
#include "ICCARD_M3.h"
#include "ICCARD_E2.h"
#include "Public.h"



/********************���ر���*************************/
static  u8  CardDataBuffer[VDR_CARD_BUFFER_SIZE] = {0};//��ȡ��IC�����ݷ��ڸû���
static  u8  CardReportBuffer[VDR_CARD_BUFFER_SIZE] = {0};//��ʻԱǩ���ϱ�����
static  u8  CardReportBufferLen = 0;
static  u8  CardState = 0;//��״̬
static  u8  CardReadStep = 0;//��������


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
static void VDRCard_GetLoginInfo(u8 *pCard, u8 CardType);
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
    
    static  u8  InitFlag = 0;//��ʼ����־��1Ϊ�ѳ�ʼ����0Ϊδ��ʼ��

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
/**************************************************************************
//��������VDRCard_GetReportInfo
//���ܣ���ü�ʻԱǩ���ϱ���Ϣ
//���룺pBuffer:Ŀ�껺��
//�����
//����ֵ��0:��ʾδ��ȡ������
//��ע���ɼ����б��ʽ���͹����ʽ��
***************************************************************************/
u8 VDRCard_GetReportInfo(u8 *pBuffer)
{
    if(0 == VDROvertime_GetLoginFlag())
		{
		    return 0;
		}
		else if(0 == CardReportBufferLen)
		{
		    return 0;
		}
		else
		{
		    memcpy(pBuffer,CardReportBuffer,CardReportBufferLen);
			  return CardReportBufferLen;
		}
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
    static  u8  LastCardInsertFlag = 0;//�忨��־��1Ϊ�Ѳ忨��0Ϊδ�忨
    static  u8  LastCarRunFlag = 0;//��ʻ��־��1Ϊ��ʻ��0Ϊͣʻ
    static  u32     EnterCount = 0;

    u8  CardInsertFlag = 0;
    u8  CarRunFlag = 0;
	  u8  Buffer[30];

    CarRunFlag = VDRDoubt_GetCarRunFlag();
    CardInsertFlag = VDRCard_IsInsert();
        
        EnterCount++;
        
        if(EnterCount < 40)//���ڿ������γ��ļ�����˲����������ȴ��㹻��ʱ�䣬���������Ϻ�
        {
        
        }
        else if(40 == EnterCount)
        {
                if(0 == CardInsertFlag)//�ϵ���޿�
                {
                        CardState = CARD_STATE_LOGOUT;
                }
                else//�ϵ���п�
                {
                        CardState = CARD_STATE_READ;
                }
        }
        else
        {

            if((1 == CardInsertFlag)&&(0 == LastCardInsertFlag)&&(0 == CarRunFlag))//ͣ���忨
            {
                CardState = CARD_STATE_READ;
            }
            else if((0 == CardInsertFlag)&&(1 == LastCardInsertFlag)&&(0 == CarRunFlag))//ͣ���ο�
            {
                CardState = CARD_STATE_LOGOUT;
            }
            else if((1 == LastCarRunFlag)&&(0 == CarRunFlag)&&(1 == CardInsertFlag))//����ʻ��ͣʻʱ�п�
            {
                CardState = CARD_STATE_READ;
            }
            else if((1 == LastCarRunFlag)&&(0 == CarRunFlag)&&(0 == CardInsertFlag))//����ʻ��ͣʻʱ�޿�
            {
                CardState = CARD_STATE_LOGOUT;
            }
						else if((1 == CarRunFlag)&&(LastCardInsertFlag != CardInsertFlag))
						{
						    strcpy((char *)Buffer,"��ʻ�н�ֹ��ο�");
                Public_PlayTTSVoiceStr(Buffer);
                Public_ShowTextInfo((char *)Buffer,100);
						}
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
    static  u8  count = 0;
    u8 Addr;
    u8 Len;
    u8 License[20];
    
    switch(CardReadStep)
    {
        case    CARD_READ_START:
        {
            count = 0;
            CardReadStep++;
            break;
        }
        case    CARD_READ_24XX:
        {
            count++;
            if(count >= 10)
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
                    if(18 == VDRCard_GetLicense(License,CardDataBuffer+1))
                    {
                      CardDataBuffer[0] = 1;
                      CardState = CARD_STATE_LOGIN;
                        CardReadStep = CARD_READ_START;
                    }
                    else
                    {
                      CardReadStep++;
                    }
                }
                count = 0;
            }
            break;
        }
        case    CARD_READ_4442:
        {
            count++;
            if(count >= 10)
            {
                Len = 128;//���꿨����
                if(0 == SL4442_Buffer_Read(CardDataBuffer+1,Len))
                {
                    CardReadStep++;
                }
                else
                {
                    if(18 == VDRCard_GetLicense(License,CardDataBuffer+1))
                    {
                      CardDataBuffer[0] = 1;
                      CardState = CARD_STATE_LOGIN;
                        CardReadStep = CARD_READ_START;
                    }
                    else
                    {
                      CardReadStep++;
                    }
                }
                count = 0;
            }
            break;
        }
        default: 
        {
            CardState = CARD_STATE_MONITOR;
            CardReadStep = CARD_READ_START;
					  Public_PlayTTSVoiceStr("��Ƭ��Ч");
            Public_ShowTextInfo("��Ƭ��Ч",100);
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
    u8  Buffer[20];
    u8  BufferLen;
    
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
  static u8 InCount = 0;
  static u8 OutCount = 0;
  static u8 InsertFlag = 0;
        
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
    u8  *p = NULL;
    u8  i;
    u8  temp;
    u8  flag;
    
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
						VDRCard_GetLoginInfo(pCard, 0);
            return 18;
        }
        
    //}

    //���ж��Ƿ�Ϊ�б꿨
    p = pCard;
    temp = *p++;
    //if(0x00==(temp&0x0f))//��Ƭ����Ϊ0�������Ǽ�ʻԱ��ݿ�
    //{
        temp = *p++;//��ʻԱ�������ȣ�1�ֽ�
        if(temp > 20)
        {
          return 0;//��ʻԱ�������ȳ����������ǷǷ�����
        }
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
    VDRCard_GetLoginInfo(pCard, 1);
    return 18;
}
/**************************************************************************
//��������VDRCard_GetLoginInfo
//���ܣ���ü�ʻԱǩ����Ϣ
//���룺pCard:ָ���ȡ���Ŀ�Ƭ����,CardType:��Ƭ���ͣ�0Ϊ���꿨��1Ϊ���꿨
//�����pLogin:ָ��ǩ����Ϣ����
//����ֵ��0:��ʾδ��ȡ������
//��ע���ɼ����б��ʽ���͹����ʽ��
***************************************************************************/
static void VDRCard_GetLoginInfo(u8 *pCard, u8 CardType)
{
    u8 *p = NULL;
	  u8 DriverNameLen;
	  u8 DriverCodeLen;
	  u8 OrganizationLen;
	
	  p = pCard;
	
	  if(0 == CardType)
		{
		    CardReportBuffer[0] = 0x01;//�ϰ�
			  VDRPub_ConvertNowTimeToBCD(CardReportBuffer+1);//�ϰ�ʱ��
			  CardReportBuffer[7] = 0x00;//�ɹ�
			  CardReportBuffer[8] = 0x04;//��ʻԱ��������
			  strcpy((char *)CardReportBuffer+9,(const char *)"����");//����Ĭ��Ϊ����
			  memcpy(CardReportBuffer+13,p+32,18);//��ҵ�ʸ�֤���룬�Ѽ�ʻ֤���뵱����ҵ�ʸ�֤��
			  CardReportBuffer[31] = 0x00;
			  CardReportBuffer[32] = 0x00;
			  CardReportBuffer[33] = 0x08;//��֤�������Ƴ���
			  strcpy((char *)CardReportBuffer+34,(const char *)"��������");//��֤����Ĭ��Ϊ��������
			  CardReportBuffer[42] = 0x20;//��Ч��
			  memcpy(CardReportBuffer+43,p+50,3);
			  CardReportBufferLen = 46;
			  if(0 == CardReportBufferLen)
				{
				
				}
			  
		}
		else if(1 == CardType)
		{
		    CardReportBuffer[0] = 0x01;//�ϰ�
			  VDRPub_ConvertNowTimeToBCD(CardReportBuffer+1);//�ϰ�ʱ��
			  CardReportBuffer[7] = 0x00;//�ɹ�
			  DriverNameLen = *(p+1);//��ʻԱ��������
			  CardReportBuffer[8] = DriverNameLen;
			  memcpy(CardReportBuffer+9,p+2,DriverNameLen);//��������
			  memcpy(CardReportBuffer+9+DriverNameLen,p+22+DriverNameLen,20);//������ҵ�ʸ�֤����
			  OrganizationLen = *(p+62+DriverNameLen);//��֤�������Ƴ���
			  CardReportBuffer[29+DriverNameLen] = OrganizationLen;
			  memcpy(CardReportBuffer+30+DriverNameLen,p+63+DriverNameLen,OrganizationLen);//������֤����
			  DriverCodeLen = *(p+63+DriverNameLen+OrganizationLen);
			  memcpy(CardReportBuffer+30+DriverNameLen+OrganizationLen,p+64+DriverNameLen+OrganizationLen+DriverCodeLen,4);//������Ч��
			  CardReportBufferLen = 30+DriverNameLen+OrganizationLen+4;
			  if(0 == CardReportBufferLen)
				{
				
				}
		}
		else
		{
		   return ;
		}
	
	  
}

/*************************************************************
** ��������: VDRCard_GetExternalDriverName
** ��������: ��ȡ��ʻԱ����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char VDRCard_GetExternalDriverName(unsigned char *pBuffer)
{
	unsigned char DriverNameLen;

	DriverNameLen = CardReportBuffer[8];
	memcpy(pBuffer,CardReportBuffer+9,DriverNameLen);//��������

	return DriverNameLen;
}





