//深圳市汉德网络科技有限公司载重系统

#include "include.h"

static load_device_data loadInfo;

static float			fWeight;

//深圳市汉德网络科技有限公司
void Hand_WeightingSystem_ProtocolParse(unsigned char *pBuffer,unsigned short Len)
{
	char *ptr = NULL;
	char *endptr;
	if(0 == strncmp((char *)pBuffer,"$ ",2))//匹配关键字
	{
		ptr = strchr((char *)pBuffer,' ');//第一个空格
		if(ptr)
		{			
   			loadInfo.id = strtoul(ptr, &endptr, 10);
			ptr = strchr((char *)pBuffer+2,' ');//第二个空格
			if(ptr)
			{
				fWeight = atof(ptr);//串口出来的是公斤
				loadInfo.weight = (unsigned short)(fWeight*100);//传给平台是1/100吨
			}
		}
		loadInfo.staCnt = 0;//表明设备在线
		loadInfo.status = 1;
		//收到重量数据马上上报
		Report_UploadPositionInfo(CHANNEL_DATA_1);
        Report_UploadPositionInfo(CHANNEL_DATA_2);
		loadInfo.status = 0;
		//清除id号和重量数据
		loadInfo.id = 0;
		loadInfo.weight = 0;
	}
}
//附加位置信息
u16 Hand_WeightingSystem_GetPositionAdditionalInfo(u8 *pBuffer)
{
	u8 length;
	u8 *p = NULL;
	
	length = 0;
	p = pBuffer;

	if(loadInfo.status)
	{
		*p++ = 0xF0;//RFID 号
		*p++ = 4;
		Public_ConvertLongToBuffer(loadInfo.id,p);
		p += 4;
		length += 6;

		*p++ = 0xE1;//重量，单位：1/100 吨	
		*p++ = 2;
		Public_ConvertShortToBuffer(loadInfo.weight,p);
		p += 2;
		length += 4;
	}
	return length;
	
}
//菜单显示
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

