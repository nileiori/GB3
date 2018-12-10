/******************************************************************** 
//��Ȩ˵��	:
//�ļ�����	:GPIOControl.c		
//����		:GPIO ����/�������
//�汾��	:
//������	:yjb
//����ʱ��	:2013.03.09
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:1)
//               
***********************************************************************/
//****************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "GPIOControl.h"

//****************�궨��********************

//****************�ⲿ����*******************

//****************ȫ�ֱ���*******************
const uint16_t GPIO_OUT_IN_PIN[GPIO_OUT_IN_MAX] = {
                                                //CON5�ӿ�,32PIN�ӿ�
                                                DLIGHTI_PIN,
                                                LLIGHTI_PIN,
                                                BREAK_PIN,
                                                ACC_PIN,
                                                DOORI_N_PIN,
                                                DOORI_P_PIN,
                                                DOORI2_PIN,
                                                LAMP_PIN,
                                                LOCK_OUT_PIN,
                                                UNLOCK_OUT_PIN,
                                                XLIGHTI_PIN,
                                                RLIGHTI_PIN,
                                                BKHI_PIN,
                                                SWITCHI_PIN,
                                                HORNI_PIN,
                                                VIRBRATIONI_PIN,
                                                COIL_PIN,
                                                HORN_OUT_PIN,
                                                //CON6,16PIN�ӿ�
                                                DETCT1I_PIN,
                                                DETCT2I_PIN,
                                                //GNSSģ��
                                                GPS_OC_PIN,
                                                GPS_SC_PIN,
                                                GPS_CTR_PIN,
                                                //�洢Ƭѡ�ߺ�ʹ����
                                                MEM_WP_PIN,
                                                FLASH_CS_PIN,
                                                FR_CS_PIN,
                                                EPROM_CS_PIN,
                                                //ͨѶģ����ƽ�
                                                GSM_ON_PIN,
                                                GSM_PWC_PIN,
                                                GSM_DTR_PIN,
                                                GSM_RI_PIN,
                                                //����ͷ���ƽ�
                                                CAM_POWER_PIN,
                                                CAM_RD_PIN,
                                                //TTS���ƽ�
                                                TTS_RDY_PIN,
                                                TTS_P_PIN,
                                                //LCD���ƽ�
                                                LCD_ON_PIN,
                                                LCD_CS_PIN,
                                                LCD_RST_PIN,
                                                LCD_AO_PIN,
                                                LCD_SPI_SCK_PIN,
                                                LCD_SPI_MISO_PIN,
                                                LCD_SPI_MOSI_PIN,
                                                //CAN����
                                                CAN_CTR_MOSI_PIN,
                                                //BMA220�𶯴�����
                                                SHOCK_CS_MOSI_PIN,
                                                //���ȿ���
                                                V_COL_MOSI_PIN,
                                                //ic ����Դ����
                                                IC_P_MOSI_PIN,
                                                //����������
                                                BUZZER_PIN,
						//��˷�ʹ�ܿ���
						MIC_EN_PIN,
						//I2S
						I2S_AD_DA_PIN,
						//I2S
						I2S_PWDAN_PIN,
						//I2S
						I2S_PWADN_PIN,
						//SD_POWER
						SD_POWER_PIN,
                                                   //POWER TYPE
                                                PWR_TYPE_PIN,
						 //SD������
   						SD_DETECT_PIN,
                                              };
GPIO_TypeDef* GPIO_OUT_IN_PORT[GPIO_OUT_IN_MAX] = {
                                                //CON5�ӿ�,32PIN�ӿ�
                                                DLIGHTI_GPIO_PORT,
                                                LLIGHTI_GPIO_PORT,
                                                BREAK_GPIO_PORT,
                                                ACC_GPIO_PORT,
                                                DOORI_N_GPIO_PORT,
                                                DOORI_P_GPIO_PORT,
                                                DOORI2_GPIO_PORT,
                                                LAMP_GPIO_PORT,
                                                LOCK_OUT_GPIO_PORT,
                                                UNLOCK_OUT_GPIO_PORT,
                                                XLIGHTI_GPIO_PORT,
                                                RLIGHTI_GPIO_PORT,
                                                BKHI_GPIO_PORT,
                                                SWITCHI_GPIO_PORT, 
                                                HORNI_GPIO_PORT,
                                                VIRBRATIONI_GPIO_PORT,
                                                COIL_GPIO_PORT,
                                                HORN_OUT_GPIO_PORT,
                                                //CON6,16PIN�ӿ�
                                                DETCT1I_GPIO_PORT,
                                                DETCT2I_GPIO_PORT,
                                                //GNSSģ��
                                                GPS_OC_GPIO_PORT,
                                                GPS_SC_GPIO_PORT,
                                                GPS_CTR_GPIO_PORT,
                                                //�洢Ƭѡ�ߺ�ʹ����
                                                MEM_WP_GPIO_PORT,
                                                FLASH_CS_GPIO_PORT,
                                                FR_CS_GPIO_PORT,
                                                EPROM_CS_GPIO_PORT,
                                                //ͨѶģ����ƽ�
                                                GSM_ON_GPIO_PORT,
                                                GSM_PWC_GPIO_PORT,
                                                GSM_DTR_GPIO_PORT,
                                                GSM_RI_GPIO_PORT,
                                                //����ͷ���ƽ�
                                                CAM_POWER_GPIO_PORT,
                                                CAM_RD_GPIO_PORT,
                                                //TTS���ƽ�
                                                TTS_RDY_GPIO_PORT,
                                                TTS_P_GPIO_PORT,
                                                //LCD���ƽ�
                                                LCD_ON_GPIO_PORT,
                                                LCD_CS_GPIO_PORT,

                                                LCD_RST_GPIO_PORT,
                                                LCD_AO_GPIO_PORT,
                                                LCD_SPI_SCK_GPIO_PORT,
                                                LCD_SPI_MISO_GPIO_PORT,
                                                LCD_SPI_MOSI_GPIO_PORT,
                                                //CAN����
                                                CAN_CTR_MOSI_GPIO_PORT,
                                                //BMA220�𶯴�����
                                                SHOCK_CS_MOSI_GPIO_PORT,
                                                //���ȿ���
                                                V_COL_MOSI_GPIO_PORT,
                                                //ic ����Դ����
                                                IC_P_MOSI_GPIO_PORT,
                                                //����������
                                                BUZZER_GPIO_PORT,  
						//��˷�ʹ�ܿ���
						MIC_EN_GPIO_PORT,
						//I2S
						I2S_AD_DA_GPIO_PORT,
						//I2S
						I2S_PWDAN_GPIO_PORT,
						//I2S
						I2S_PWADN_GPIO_PORT,
						//SD_POWER
						SD_POWER_GPIO_PORT,
                                                   //POWER TYPE
                                                PWR_TYPE_GPIO_PORT,
						//SD������
   						SD_DETECT_GPIO_PORT,
                                              };
const uint32_t GPIO_OUT_IN_CLK[GPIO_OUT_IN_MAX] = {  
                                                //CON5�ӿ�,32PIN�ӿ�
                                                DLIGHTI_GPIO_CLK,
                                                LLIGHTI_GPIO_CLK,  
                                                BREAK_GPIO_CLK,
                                                ACC_GPIO_CLK,  
                                                DOORI_N_GPIO_CLK,
                                                DOORI_P_GPIO_CLK,
                                                DOORI2_GPIO_CLK,
                                                LAMP_GPIO_CLK,
                                                LOCK_OUT_GPIO_CLK,
                                                UNLOCK_OUT_GPIO_CLK,
                                                XLIGHTI_GPIO_CLK,
                                                RLIGHTI_GPIO_CLK,
                                                BKHI_GPIO_CLK,
                                                SWITCHI_GPIO_CLK,
                                                HORNI_GPIO_CLK,
                                                VIRBRATIONI_GPIO_CLK,
                                                COIL_GPIO_CLK,
                                                HORN_OUT_GPIO_CLK,
                                                //CON6,16PIN�ӿ�
                                                DETCT1I_GPIO_CLK,
                                                DETCT2I_GPIO_CLK,
                                                //GNSSģ��
                                                GPS_OC_GPIO_CLK,
                                                GPS_SC_GPIO_CLK,
                                                GPS_CTR_GPIO_CLK,
                                                //�洢Ƭѡ�ߺ�ʹ����
                                                MEM_WP_GPIO_CLK,
                                                FLASH_CS_GPIO_CLK,
                                                FR_CS_GPIO_CLK,
                                                EPROM_CS_GPIO_CLK,
                                                //ͨѶģ����ƽ�
                                                GSM_ON_GPIO_CLK,
                                                GSM_PWC_GPIO_CLK,
                                                GSM_DTR_GPIO_CLK,
                                                GSM_RI_GPIO_CLK,
                                                //����ͷ���ƽ�
                                                CAM_POWER_GPIO_CLK,
                                                CAM_RD_GPIO_CLK,
                                                //TTS���ƽ�
                                                TTS_RDY_GPIO_CLK,
                                                TTS_P_GPIO_CLK,
                                                //LCD���ƽ�
                                                LCD_ON_GPIO_CLK,
                                                LCD_CS_GPIO_CLK,
                                                LCD_RST_GPIO_CLK,
                                                LCD_AO_GPIO_CLK,
                                                LCD_SPI_SCK_GPIO_CLK,
                                                LCD_SPI_MISO_GPIO_CLK,
                                                LCD_SPI_MOSI_GPIO_CLK,
                                                //CAN����
                                                CAN_CTR_MOSI_GPIO_CLK,
                                                //BMA220�𶯴�����
                                                SHOCK_CS_MOSI_GPIO_CLK,
                                                //���ȿ���
                                                V_COL_MOSI_GPIO_CLK,
                                                //ic ����Դ����
                                                IC_P_MOSI_GPIO_CLK,
                                                //����������
                                                BUZZER_GPIO_CLK,  
						//��˷�ʹ�ܿ���
						MIC_EN_GPIO_CLK,
						//I2S
						I2S_AD_DA_GPIO_CLK,
						//I2S
						I2S_PWDAN_GPIO_CLK,
						//I2S
						I2S_PWADN_GPIO_CLK,
						//SD_POWER
						SD_POWER_GPIO_CLK,
                                                 //POWER TYPE
                                                PWR_TYPE_GPIO_CLK,
						//SD������
   						SD_DETECT_GPIO_CLK,
                                              };


//****************��������*******************

/****************��������******************
//��������	:GpioOutInit
//����		:��ʼ��GPIO�������
//����		:IoNum ����ֵ,�ο�GPIO_OUT_IN_NUMö��
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void GpioOutInit(GPIO_OUT_IN_NUM IoNum)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* ʵ��GPIOʱ��*/
  RCC_APB2PeriphClockCmd(GPIO_OUT_IN_CLK[IoNum], ENABLE);

  /* ����GPIO�� 50Mhz�������,������ */
  GPIO_InitStructure.GPIO_Pin = GPIO_OUT_IN_PIN[IoNum];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //LED�ƿ��Ʋ��� �������� GPIO_PuPd_UP
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_OUT_IN_PORT[IoNum], &GPIO_InitStructure);
}

/****************��������******************
//��������	:GpioOutOn
//����		:����GPIO�������Ϊ��(������1)
//����		:IoNum ����ֵ,�ο�GPIO_OUT_IN_NUMö��
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void GpioOutOn(GPIO_OUT_IN_NUM IoNum)
{
  GPIO_OUT_IN_PORT[IoNum]->BSRR = GPIO_OUT_IN_PIN[IoNum];
}

/****************��������******************
//��������	:GpioOutOff
//����		:����GPIO�������Ϊ��(������0)
//����		:IoNum ����ֵ,�ο�GPIO_OUT_IN_NUMö��
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void GpioOutOff(GPIO_OUT_IN_NUM IoNum)
{
  GPIO_OUT_IN_PORT[IoNum]->BSRR = GPIO_OUT_IN_PIN[IoNum]<<16;  
}

/****************��������******************
//��������	:GpioOutToggle
//����		:ȡ����ǰGPIO�������״̬
//����		:IoNum ����ֵ,�ο�GPIO_OUT_IN_NUMö��
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void GpioOutToggle(GPIO_OUT_IN_NUM IoNum)
{
  GPIO_OUT_IN_PORT[IoNum]->ODR ^= GPIO_OUT_IN_PIN[IoNum];
}


/****************��������******************
//��������	:GpioInInit
//����		:��ʼ����������
//����		:IoNum ����ֵ,�ο�GPIO_OUT_IN_NUMö��
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void GpioInInit(GPIO_OUT_IN_NUM IoNum)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* ʹ��GPIOʱ�� */
  RCC_APB2PeriphClockCmd(GPIO_OUT_IN_CLK[IoNum], ENABLE);
 // RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* ���ð�ťΪ����ģʽ*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_OUT_IN_PIN[IoNum];
  GPIO_Init(GPIO_OUT_IN_PORT[IoNum], &GPIO_InitStructure);
  
}

/****************��������******************
//��������	:GpioInGetState
//����		:��ȡ��������״̬
//����		:IoNum ����ֵ,�ο�GPIO_OUT_IN_NUMö��
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:��ǰIO��״ֵ̬,0��1
//��ע		:
*********************************************************************/
u8 GpioInGetState(GPIO_OUT_IN_NUM IoNum)
{
  return GPIO_ReadInputDataBit(GPIO_OUT_IN_PORT[IoNum], GPIO_OUT_IN_PIN[IoNum]);
}
