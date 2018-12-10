//�����к�������Ƽ����޹�˾����ϵͳ

#include "include.h"

static load_device_data loadInfo;

static float			fWeight;

//�����к�������Ƽ����޹�˾
void Hand_WeightingSystem_ProtocolParse(unsigned char *pBuffer,unsigned short Len)
{
	char *ptr = NULL;
	char *endptr;
	if(0 == strncmp((char *)pBuffer,"$ ",2))//ƥ��ؼ���
	{
		ptr = strchr((char *)pBuffer,' ');//��һ���ո�
		if(ptr)
		{			
   			loadInfo.id = strtoul(ptr, &endptr, 10);
			ptr = strchr((char *)pBuffer+2,' ');//�ڶ����ո�
			if(ptr)
			{
				fWeight = atof(ptr);//���ڳ������ǹ���
				loadInfo.weight = (unsigned short)(fWeight*100);//����ƽ̨��1/100��
			}
		}
		loadInfo.staCnt = 0;//�����豸����
		loadInfo.status = 1;
		//�յ��������������ϱ�
		Report_UploadPositionInfo(CHANNEL_DATA_1);
        Report_UploadPositionInfo(CHANNEL_DATA_2);
		loadInfo.status = 0;
		//���id�ź���������
		loadInfo.id = 0;
		loadInfo.weight = 0;
	}
}
//����λ����Ϣ
u16 Hand_WeightingSystem_GetPositionAdditionalInfo(u8 *pBuffer)
{
	u8 length;
	u8 *p = NULL;
	
	length = 0;
	p = pBuffer;

	if(loadInfo.status)
	{
		*p++ = 0xF0;//RFID ��
		*p++ = 4;
		Public_ConvertLongToBuffer(loadInfo.id,p);
		p += 4;
		length += 6;

		*p++ = 0xE1;//��������λ��1/100 ��	
		*p++ = 2;
		Public_ConvertShortToBuffer(loadInfo.weight,p);
		p += 2;
		length += 4;
	}
	return length;
	
}
//�˵���ʾ
float Hand_WeightingSystem_MenuDisplayData(void)
{
	if(loadInfo.staCnt++ > 7)
	{
		loadInfo.staCnt = 0;
		fWeight = 0.0f;
		loadInfo.weight = 0;
	}
	return fWeight;
}

