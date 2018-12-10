/************************************************************************
//�������ƣ�VDR_Protocol.c
//���ܣ���¼�Ǵ���Э��
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�������
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1��
*************************************************************************/

/********************�ļ�����*************************/
#include "VDR.h"
#include "EepromPram.h"
#include "fm25c160.h"
#include "spi_flash_app.h"
#include "Public.h"
#include "VDR_Usart.h"
#include "stm32f10x_rtc.h"

/********************���ر���*************************/
static u8 VDRProtocolParseBuffer[VDR_PROTOCOL_PARSE_BUFFER_SIZE] = {0};
static u8 VDRProtocolSendBuffer[VDR_PROTOCOL_SEND_BUFFER_SIZE] = {0};

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static u16 VDRProtocol_ParseCmd08H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd09H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd10H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd11H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd12H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd13H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd14H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd15H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);

static u16 VDRProtocol_ParseCmd82H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd83H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmd84H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdC2H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdC3H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdC4H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);

static u16 VDRProtocol_ParseCmdE0H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdE1H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdE2H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdE3H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
static u16 VDRProtocol_ParseCmdE4H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);

/********************��������*************************/

/**************************************************************************
//��������VDRProtocol_Parse
//���ܣ�Э��������
//���룺��Ҫ���������ݡ�����
//�������
//����ֵ��0��������ȷ��1����������
//��ע�����ڽ��յ�һ֡���ݺ�����ô˺�������
***************************************************************************/
u8 VDRProtocol_Parse(u8 *pBuffer, u16 BufferLen)
{
	u16 i,j;
	u16 length;
	u16 SendLen;
	u8 Cmd;
	u8 VerifyCode;
	u8 *p = NULL;

	if(BufferLen < 6)
	{
		return 1;//���ȴ���
	}

	p = pBuffer;
	for(i=0; i<BufferLen-1; i++)
	{
		if((0xaa == *(p+i))&&(0x75 == *(p+i+1)))
		{
			Cmd = *(p+i+2);
			length = 0;
			length |= *(p+i+3) << 8;
			length |= *(p+i+4);
			VerifyCode = 0;
			for(j=i;j<i+6+length;j++)
			{
				VerifyCode = VerifyCode ^ *(p+j);
			}
			if(VerifyCode == *(p+j))
			{
				break;
			}
		}
	}

	if((BufferLen-1) == i)//δ�ҵ��ϸ��֡
	{
		return 2;
	}
	else
	{
		if(length < VDR_PROTOCOL_PARSE_BUFFER_SIZE)
		{
			SendLen = VDRProtocol_ParseCmd(VDRProtocolParseBuffer,Cmd, p+i+6, length);
			VDRProtocol_SendCmd(Cmd,VDRProtocolParseBuffer,SendLen);
			return 0;
		}
		else
		{
			return 3;
		}
	}
	
}
/**************************************************************************
//��������VDRProtocol_ParseCmd
//���ܣ�����ָ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ���Cmd:������
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��VDRProtocol_Parse���øú�����
//������ֵΪ0xFFFFʱ��ʾ�ɼ���������֡���ճ���������ֵΪ0xFFFEʱ��ʾ���ò�������֡���ճ���
//������ֵΪ0xFFFDʱ��ʾ���ǺϷ���������(Cmd���Ϸ�)��
***************************************************************************/
u16 VDRProtocol_ParseCmd(u8 *pDstBuffer, u8 Cmd, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u32 TimeCount;


	TimeCount = RTC_GetCounter();

	switch(Cmd)
	{
		///////////���ݲɼ�����/////////////
		case VDR_PROTOCOL_CMD_00H:
		{
			length = VDRProtocol_ParseCmd00H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_01H:
		{
			length = VDRProtocol_ParseCmd01H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_02H:
		{
			length = VDRProtocol_ParseCmd02H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_03H:
		{
			length = VDRProtocol_ParseCmd03H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_04H:
		{
			length = VDRProtocol_ParseCmd04H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_05H:
		{
			length = VDRProtocol_ParseCmd05H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_06H:
		{
			length = VDRProtocol_ParseCmd06H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_07H:
		{
			length = VDRProtocol_ParseCmd07H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_08H:
		{
			length = VDRProtocol_ParseCmd08H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_09H:
		{
			length = VDRProtocol_ParseCmd09H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_10H:
		{
			length = VDRProtocol_ParseCmd10H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_11H:
		{
			length = VDRProtocol_ParseCmd11H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_12H:
		{
			length = VDRProtocol_ParseCmd12H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_13H:
		{
			length = VDRProtocol_ParseCmd13H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_14H:
		{
			length = VDRProtocol_ParseCmd14H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_15H:
		{
			length = VDRProtocol_ParseCmd15H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		///////////������������/////////////
		case VDR_PROTOCOL_CMD_82H:
		{
			length = VDRProtocol_ParseCmd82H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_83H:
		{
			length = VDRProtocol_ParseCmd83H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_84H:
		{
			length = VDRProtocol_ParseCmd84H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_C2H:
		{
			length = VDRProtocol_ParseCmdC2H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_C3H:
		{
			length = VDRProtocol_ParseCmdC3H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		case VDR_PROTOCOL_CMD_C4H:
		{
			length = VDRProtocol_ParseCmdC4H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			VDRParameter_Write(TimeCount, Cmd);
			break;
		}
		///////////�춨����/////////////
		case VDR_PROTOCOL_CMD_E0H:
		{
			length = VDRProtocol_ParseCmdE0H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_E1H:
		{
			length = VDRProtocol_ParseCmdE1H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_E2H:
		{
			length = VDRProtocol_ParseCmdE2H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_E3H:
		{
			length = VDRProtocol_ParseCmdE3H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		case VDR_PROTOCOL_CMD_E4H:
		{
			length = VDRProtocol_ParseCmdE4H(pDstBuffer,pSrcBuffer,SrcBufferLen);
			break;
		}
		
		default: 
		{
			length = 0xFFFD;
			break;
		}
	}

	return length;
}
/**************************************************************************
//��������VDRProtocol_SendCmd
//���ܣ�����ָ��
//���룺Cmd��ָ�pBuffer��ָ��ָ�����ݣ�BufferLen��ָ�����ݳ���
//�������
//����ֵ��0�����ͳɹ���1������ʧ��
//��ע��VDRProtocol_Parse���øú�����
//������BufferLenΪ0xFFFFʱ��ʾ�ɼ���������֡���ճ���
//������BufferLenΪ0xFFFEʱ��ʾ���ò�������֡���ճ���
//������BufferLenΪ0xFFFDʱ��ʾ���ǺϷ���������(Cmd���Ϸ�),��Ӧ��
***************************************************************************/
u8 VDRProtocol_SendCmd(u8 Cmd, u8 *pBuffer, u16 BufferLen)
{
	u16 SendLen;
	u16 i;
	u8 VerifyCode;
	

	if(0xffff == BufferLen)
	{
		SendLen = 0;
		VDRProtocolSendBuffer[SendLen++] = 0x55;
		VDRProtocolSendBuffer[SendLen++] = 0x7a;
		VDRProtocolSendBuffer[SendLen++] = 0xfa;
		VDRProtocolSendBuffer[SendLen++] = 0x00;

		VerifyCode = 0;
		for(i=0; i<SendLen; i++)
		{
			VerifyCode = VerifyCode ^ VDRProtocolSendBuffer[i];
		}
		VDRProtocolSendBuffer[SendLen++] = VerifyCode;
		if(SendLen == VDRUsart_SendData(VDRProtocolSendBuffer,SendLen))
		{
			return 0;
		}
		else
		{
			return 1;
		}
		
	}
	else if(0xfffe == BufferLen)
	{
		SendLen = 0;
		VDRProtocolSendBuffer[SendLen++] = 0x55;
		VDRProtocolSendBuffer[SendLen++] = 0x7a;
		VDRProtocolSendBuffer[SendLen++] = 0xfb;
		VDRProtocolSendBuffer[SendLen++] = 0x00;
		VerifyCode = 0;
		for(i=0; i<SendLen; i++)
		{
			VerifyCode = VerifyCode ^ VDRProtocolSendBuffer[i];
		}
		VDRProtocolSendBuffer[SendLen++] = VerifyCode;
		if(SendLen == VDRUsart_SendData(VDRProtocolSendBuffer,SendLen))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if(0xfffd == BufferLen)
	{
		return 0;
	}
	else if(BufferLen < 1000)
	{
		SendLen = 0;
		VDRProtocolSendBuffer[SendLen++] = 0x55;
		VDRProtocolSendBuffer[SendLen++] = 0x7a;
		VDRProtocolSendBuffer[SendLen++] = Cmd;
		VDRProtocolSendBuffer[SendLen++] = (BufferLen&0xff00)>>8;
		VDRProtocolSendBuffer[SendLen++] = BufferLen&0xff;
		VDRProtocolSendBuffer[SendLen++] = 0x00;

		memcpy(VDRProtocolSendBuffer+SendLen,pBuffer,BufferLen);
		SendLen += BufferLen;

		VerifyCode = 0;
		for(i=0; i<SendLen; i++)
		{
			VerifyCode = VerifyCode ^ VDRProtocolSendBuffer[i];
		}
		VDRProtocolSendBuffer[SendLen++] = VerifyCode;

		if(SendLen == VDRUsart_SendData(VDRProtocolSendBuffer,SendLen))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else//���ȳ���
	{
		return 0;
	}
	
}
/**************************************************************************
//��������VDRProtocol_ParseCmd00H
//���ܣ�00Hָ��Ӧ�𣬲ɼ���¼��ִ�б�׼�汾
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd00H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;

		*p++ = 0x12;//��¼�Ǳ�׼,2012���׼
		length++;
		
		*p++ = 0x00;//�޸ĵ���,Ĭ��Ϊ00H
		length++;

		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd01H
//���ܣ�01Hָ��Ӧ�𣬲ɼ���ǰ��ʻ����Ϣ
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd01H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 i;
	u8 *p = NULL;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;

		if(0 == VDROvertime_GetLoginFlag())
		{
			for(i=0; i<18; i++)
			{
				*p++ = 0x00;//��ʻԱû�е�¼,���0x00
			}
			length = 18;
		}
		else
		{
			length = VDROvertime_GetLicense(p);
		}

		return length;
		
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd02H
//���ܣ�02Hָ��Ӧ�𣬲ɼ���¼��ʵʱʱ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd02H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;
	//TIME_T *tt;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;
		VDRPub_ConvertNowTimeToBCD(p);
		length = 6;
		return length;
		
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd03H
//���ܣ�03Hָ��Ӧ�𣬲ɼ��ۼ���ʻ���
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd03H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;
	u8 i;
	u8 Buffer[5];
	u8 BufferLen;
	u32 temp;
	TIME_T tt;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;	
		p = pDstBuffer;
		VDRPub_ConvertNowTimeToBCD(p);//ʵʱʱ��
		p += 6;
		length += 6;
		BufferLen = EepromPram_ReadPram(E2_INSTALL_TIME_ID,Buffer);//��װʱ��
		if(0 == BufferLen)//��װʱ���ȡʧ��,��0x00���
		{
			for(i=0; i<6; i++)
			{
				*p++ = 0x00;
			}
			length += 6;
		}
		else
		{
			temp = Public_ConvertBufferToLong(Buffer);
			Gmtime(&tt, temp);
			VDRPub_ConvertTimeToBCD(p,&tt);
			p += 6;
			length += 6;
		}
		BufferLen = EepromPram_ReadPram(E2_CAR_INIT_MILE_ID,Buffer);//��ʼ���
		if(0 == BufferLen)//
		{
			for(i=0; i<4; i++)
			{
				*p++ = 0x00;
			}
			length += 4;	
		}
		else
		{
			temp = Public_ConvertBufferToLong(Buffer);
			Public_ConvertLongToBCD(temp,p);
			p += 4;
			length += 4;
		}

		BufferLen = FRAM_BufferRead(Buffer, FRAM_VDR_MILEAGE_LEN, FRAM_VDR_MILEAGE_ADDR);//�ۼ����
		if(FRAM_VDR_MILEAGE_LEN == BufferLen)
		{
			temp = Public_ConvertBufferToLong(Buffer);
		}
		else
		{
			temp = 0;
		}
		Public_ConvertLongToBCD(temp,p);
		p += 4;
		length += 4;

		return length;
		
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd04H
//���ܣ�04Hָ��Ӧ�𣬲ɼ���¼������ϵ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd04H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;
//	u8 i;
	u8 Buffer[5];
	u8 BufferLen;
//	u32 temp;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;
		VDRPub_ConvertNowTimeToBCD(p);//ʵʱʱ��
		p += 6;
		length += 6;
		
		BufferLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, Buffer);//03������ϵ����3�ֽ�,12��Ϊ2�ֽ�
		if(2 == BufferLen)
		{
			memcpy(p,Buffer,BufferLen);
			p += 2;
			length += 2;
		}
		else
		{
			*p++ = 0x00;
			*p++ = 0x00;
			length += 2;
		}

		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd05H
//���ܣ�05Hָ��Ӧ�𣬲ɼ�������Ϣ
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd05H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;
	u8 i;
	u8 Buffer[20];
	u8 BufferLen;
//	u32 temp;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;

		BufferLen = EepromPram_ReadPram(E2_CAR_IDENTIFICATION_CODE_ID,Buffer);//����ʶ�����
		if(17 == BufferLen)
		{
			memcpy(p,Buffer,BufferLen);
			p += 17;
			length += 17;
		}
		else
		{
			for(i=0; i<17; i++)
			{
				*p++ = ' ';//���ո�
			}
			length += 17;
		}

		BufferLen = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID,Buffer);//���ƺ���
		if((BufferLen > 12)||(0 == BufferLen))
		{
			for(i=0; i<12; i++)
			{
				*p++ = ' ';//���ո�
			}
			length += 12;
		}
		else
		{
			memcpy(p,Buffer,BufferLen);
			p += BufferLen;
			length += BufferLen;
			for(i=BufferLen; i<12; i++)
			{
				*p++ = ' ';//���ո�
				length++;
			}
		}

		BufferLen = EepromPram_ReadPram(E2_CAR_TYPE_ID,Buffer);//���Ʒ���
		if((BufferLen > 12)||(0 == BufferLen))
		{
			for(i=0; i<12; i++)
			{
				*p++ = ' ';//���ո�
			}
			length += 12;
		}
		else
		{
			memcpy(p,Buffer,BufferLen);
			p += BufferLen;
			length += BufferLen;
			for(i=BufferLen; i<12; i++)
			{
				*p++ = ' ';//���ո�
				length++;
			}
		}
		return length;
		
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd06H
//���ܣ�06Hָ��Ӧ�𣬲ɼ���¼��״̬�ź�������Ϣ
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd06H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;
		VDRPub_ConvertNowTimeToBCD(p);//ʵʱʱ��
		p += 6;
		length += 6;

		*p++ = 1;//״̬�ź��ֽڸ���,�̶�Ϊ1
		length++;
		
		SPI_FLASH_BufferRead(p,FLASH_VDR_STATUS_SET_SECTOR,80);//״̬�ź�����
		p += 80;
		length += 80;

		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd07H
//���ܣ�07Hָ��Ӧ�𣬲ɼ���¼��Ψһ�Ա��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd07H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u8 *p = NULL;

	if(SrcBufferLen != 0)
	{
		return 0xffff;
	}
	else
	{
		length = 0;
		p = pDstBuffer;
		length += EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID,p);

		return length;
	}
	
}

//////////////////////////////�ļ��ڲ�����/////////////////////
/**************************************************************************
//��������VDRProtocol_ParseCmd08H
//���ܣ�08Hָ��Ӧ�𣬲ɼ�ָ������ʻ�ٶȼ�¼
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd08H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_SPEED,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd09H
//���ܣ�09Hָ��Ӧ�𣬲ɼ�ָ����λ����Ϣ��¼
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd09H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_POSITION,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd10H
//���ܣ�10Hָ��Ӧ�𣬲ɼ�ָ�����¹��ɵ��¼
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd10H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_DOUBT,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd11H
//���ܣ�11Hָ��Ӧ�𣬲ɼ�ָ���ĳ�ʱ��ʻ��¼
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd11H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDROvertime_GetLog(p, EndTime);
		p += length;
		length += VDRData_Read(p,VDR_DATA_TYPE_OVER_TIME,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd12H
//���ܣ�12Hָ��Ӧ�𣬲ɼ�ָ���ļ�ʻ����ݼ�¼
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd12H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_DRIVER,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd13H
//���ܣ�13Hָ��Ӧ�𣬲ɼ�ָ�����ⲿ�����¼
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd13H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_POWER,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd14H
//���ܣ�14Hָ��Ӧ�𣬲ɼ�ָ���Ĳ����޸ļ�¼
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd14H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_PARAMETER,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd15H
//���ܣ�15Hָ��Ӧ�𣬲ɼ�ָ�����ٶ�״̬��־��¼
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd15H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u16 length;
	u16 Block;
	u8 *p = NULL;
	TIME_T StartTime;
	TIME_T EndTime;
	
	if(SrcBufferLen != 14)
	{
		return 0xffff;
	}
	else
	{
		
		p = pSrcBuffer;

		VDRPub_ConvertBCDToTime(&StartTime,p);
		p += 6;

		VDRPub_ConvertBCDToTime(&EndTime,p);
		p += 6;

		Block = Public_ConvertBufferToShort(p);
		p += 2;

		p = pDstBuffer;
		length = VDRData_Read(p,VDR_DATA_TYPE_SPEED_STATUS,StartTime,EndTime,Block);
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd82H
//���ܣ�82Hָ��Ӧ�����ó�����Ϣ
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd82H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{

	u16 length;
	u8 *p = NULL;
	
	if(SrcBufferLen != 41)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		length = strlen((const char *)p);
		if(length > 17)
		{
			length = 17;
		}
		EepromPram_WritePram(E2_CAR_IDENTIFICATION_CODE_ID,p,length);
		p += 17;
		
		length = strlen((const char *)p);
		if(length > 17)
		{
			length = 17;
		}
		EepromPram_WritePram(E2_CAR_PLATE_NUM_ID, p, length);
		p += 12;

		length = strlen((const char *)p);
		if(length > 12)
		{
			length = 12;
		}
		EepromPram_WritePram(E2_CAR_TYPE_ID, p, length);
		p += 12;
		
		return 0;//Ӧ��֡Ϊ��
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd83H
//���ܣ�83Hָ��Ӧ�����ó��ΰ�װ����
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd83H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u32 TimeCount;
	u8 *p = NULL;
	u8 Buffer[5];
	TIME_T Time;
	
	if(SrcBufferLen != 6)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		if(1 == VDRPub_ConvertBCDToTime(&Time,p))
		{
			TimeCount = ConverseGmtime(&Time);
			Public_ConvertLongToBuffer(TimeCount,Buffer);
			EepromPram_WritePram(E2_INSTALL_TIME_ID, Buffer, 4); 
		}

		return 0;//Ӧ��֡Ϊ��
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmd84H
//���ܣ�84Hָ��Ӧ������״̬��������Ϣ
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmd84H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	
	if((0 != SrcBufferLen%80)||(0 == SrcBufferLen))
	{
		return 0xfffe;
	}
	else 
	{
		p = pSrcBuffer;
		SPI_FLASH_SectorErase(FLASH_VDR_STATUS_SET_SECTOR);
	        SPI_FLASH_BufferWrite(p,FLASH_VDR_STATUS_SET_SECTOR,SrcBufferLen);

		return 0;//Ӧ��֡Ϊ��
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmdC2H
//���ܣ�C2Hָ��Ӧ�����ü�¼��ʱ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmdC2H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	TIME_T Time;
	
	if(SrcBufferLen != 6)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		if(1 == VDRPub_ConvertBCDToTime(&Time,p))
		{
			RTC_SetTime(&Time);
		}

		return 0;//Ӧ��֡Ϊ��
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmdC3H
//���ܣ�C3Hָ��Ӧ����������ϵ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmdC3H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	
	if(SrcBufferLen != 8)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		p += 6;//����6�ֽڵ�ʵʱʱ��
		EepromPram_WritePram(E2_CAR_FEATURE_COEF_ID,p,2); 

		return 0;//Ӧ��֡Ϊ��
	}
	 
}
/**************************************************************************
//��������VDRProtocol_ParseCmdC4H
//���ܣ�C4Hָ��Ӧ��������ʼ���
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmdC4H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	u8 Buffer[5];
	u32 StartMile;
	
	if(SrcBufferLen != 20)
	{
		return 0xfffe;
	}
	else
	{
		p = pSrcBuffer;
		p += 6;//����6�ֽڵ�ʵʱʱ��
		p += 6;//����6�ֽڵĳ��ΰ�װʱ��
		StartMile = Public_ConvertBCDToLong(p);//������ʼ���
		Public_ConvertLongToBuffer(StartMile,Buffer);
		EepromPram_WritePram(E2_CAR_INIT_MILE_ID, Buffer, 4); 
		//�����ۼ���ʻ��̣�ÿ��������ʼ���ʱ���Զ����ۼ���ʻ���Ҳ��Ϊ��ʼ��̣�
//		GpsMile_SetPram(StartMile);

		return 0;//Ӧ��֡Ϊ��
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmdE0H
//���ܣ�E0Hָ��Ӧ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmdE0H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE0H();
		return 0;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmdE1H
//���ܣ�E1Hָ��Ӧ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmdE1H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	u8 *p = NULL;
	u8 PramLen;
	u16 length;
	u16 Speed;
//	u16 CarPulseCoff;
	u32 TotalMile;

	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE1H();

		length = 0;
		p = pDstBuffer;

		PramLen = EepromPram_ReadPram(E2_CAR_ONLY_NUM_ID,p);//Ψһ�Ա��,35�ֽ�
		p += PramLen;
		length += PramLen;

		PramLen = EepromPram_ReadPram(E2_CAR_FEATURE_COEF_ID, p);//����ϵ��,2�ֽ�
		p += PramLen;
		length += PramLen;

		Speed = VDRPulse_GetSecondSpeed()*10;//��ǰ�ٶ�,2�ֽ�
		*p++ = (Speed&0xff00) >> 8;
		*p++ = Speed&0xff;
		length += 2;

		TotalMile = VDRPulse_GetTotalMile()*10;//��ǰ�춨���ۼ���ʻ���,4�ֽ�
		Public_ConvertLongToBuffer(TotalMile,p);
		p += 4;
		length += 4;
		
		return length;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmdE2H
//���ܣ�E2Hָ��Ӧ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmdE2H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE2H();
		return 0;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmdE3H
//���ܣ�E3Hָ��Ӧ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmdE3H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE3H();
		return 0;
	}
}
/**************************************************************************
//��������VDRProtocol_ParseCmdE4H
//���ܣ�E4Hָ��Ӧ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע����VDRProtocol_ParseCmd���øú���
***************************************************************************/
static u16 VDRProtocol_ParseCmdE4H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen)
{
	if(SrcBufferLen != 0)
	{
		return 0xfffd;
	}
	else
	{
//		VDRDocimasy_HandleCmdE4H();
		return 0;
	}
}













