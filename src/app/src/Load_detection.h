#ifndef __LOAD_DETECTION_H_
#define __LOAD_DETECTION_H_

typedef struct load_device
{  
   unsigned char 	status;
   unsigned char	staCnt;//状态计数器，如果长时间不清零说明设备已经拔出或者损坏，我们就把重量清零算了      
   unsigned short 	weight;
   unsigned int 	id;
   //float			fWeight;
}load_device_data;

u16 Hand_WeightingSystem_GetPositionAdditionalInfo(u8 *pBuffer);

//深圳市汉德网络科技有限公司载重系统
void Hand_WeightingSystem_ProtocolParse(unsigned char *pBuffer,unsigned short Len);
float Hand_WeightingSystem_MenuDisplayData(void);

#endif
