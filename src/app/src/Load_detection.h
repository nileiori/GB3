#ifndef __LOAD_DETECTION_H_
#define __LOAD_DETECTION_H_

typedef struct load_device
{  
   unsigned char 	status;
   unsigned char	staCnt;//״̬�������������ʱ�䲻����˵���豸�Ѿ��γ������𻵣����ǾͰ�������������      
   unsigned short 	weight;
   unsigned int 	id;
   //float			fWeight;
}load_device_data;

u16 Hand_WeightingSystem_GetPositionAdditionalInfo(u8 *pBuffer);

//�����к�������Ƽ����޹�˾����ϵͳ
void Hand_WeightingSystem_ProtocolParse(unsigned char *pBuffer,unsigned short Len);
float Hand_WeightingSystem_MenuDisplayData(void);

#endif
