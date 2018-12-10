#ifndef __VDR_PROTOCOL_H
#define __VDR_PROTOCOL_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"
#include "VDR_Usart.h"

#define VDR_PROTOCOL_PARSE_BUFFER_SIZE	VDR_USART_RX_BUFFER_SIZE	//���ջ������1024�ֽ�
#define VDR_PROTOCOL_SEND_BUFFER_SIZE 	VDR_USART_TX_BUFFER_SIZE	//���ͻ������1024�ֽ�

#define VDR_PROTOCOL_CMD_00H	0x00
#define VDR_PROTOCOL_CMD_01H	0x01
#define VDR_PROTOCOL_CMD_02H	0x02
#define VDR_PROTOCOL_CMD_03H	0x03
#define VDR_PROTOCOL_CMD_04H	0x04
#define VDR_PROTOCOL_CMD_05H	0x05
#define VDR_PROTOCOL_CMD_06H	0x06
#define VDR_PROTOCOL_CMD_07H	0x07
#define VDR_PROTOCOL_CMD_08H	0x08
#define VDR_PROTOCOL_CMD_09H	0x09
#define VDR_PROTOCOL_CMD_10H	0x10
#define VDR_PROTOCOL_CMD_11H	0x11
#define VDR_PROTOCOL_CMD_12H	0x12
#define VDR_PROTOCOL_CMD_13H	0x13
#define VDR_PROTOCOL_CMD_14H	0x14
#define VDR_PROTOCOL_CMD_15H	0x15

#define VDR_PROTOCOL_CMD_82H	0x82
#define VDR_PROTOCOL_CMD_83H	0x83
#define VDR_PROTOCOL_CMD_84H	0x84

#define VDR_PROTOCOL_CMD_C2H	0xC2
#define VDR_PROTOCOL_CMD_C3H	0xC3
#define VDR_PROTOCOL_CMD_C4H	0xC4

#define VDR_PROTOCOL_CMD_D0H    0xD0

#define VDR_PROTOCOL_CMD_E0H	0xE0
#define VDR_PROTOCOL_CMD_E1H	0xE1
#define VDR_PROTOCOL_CMD_E2H	0xE2
#define VDR_PROTOCOL_CMD_E3H	0xE3
#define VDR_PROTOCOL_CMD_E4H	0xE4

/**************************************************************************
//��������VDRProtocol_Parse
//���ܣ���¼�Ǵ���(DB9����)Э�����
//���룺��Ҫ���������ݡ�����
//�������
//����ֵ��0��������ȷ��1����������
//��ע�����ڽ��յ�һ֡���ݺ�����ô˺�������
***************************************************************************/
u8 VDRProtocol_Parse(u8 *pBuffer, u16 BufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd
//���ܣ������ֽ���
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ���Cmd:������
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��VDRProtocol_Parse���øú�����
//������ֵΪ0xFFFFʱ��ʾ�ɼ���������֡���ճ���������ֵΪ0xFFFEʱ��ʾ���ò�������֡���ճ���
//������ֵΪ0xFFFDʱ��ʾ���ǺϷ���������(Cmd���Ϸ�)��
***************************************************************************/
u16 VDRProtocol_ParseCmd(u8 *pDstBuffer, u8 Cmd, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//��������VDRProtocol_Packet
//���ܣ�����Ҫ���͵����ݴ��
//���룺Cmd��ָ�pBuffer��ָ����Ҫ���͵����ݣ�BufferLen�����ݳ���
//�������
//����ֵ�������ĳ���
//��ע�����øú�����pBuffer��������ݻᱻ��д
***************************************************************************/
u16 VDRProtocol_Packet(u8 Cmd, u8 *pBuffer, u16 BufferLen);
/**************************************************************************
//��������VDRProtocol_SendCmd
//���ܣ������
//���룺Cmd��ָ�pBuffer��ָ��ָ�����ݣ�BufferLen��ָ�����ݳ���
//�������
//����ֵ��0�����ͳɹ���1������ʧ��
//��ע��VDRProtocol_Parse���øú�����
//������BufferLenΪ0xFFFFʱ��ʾ�ɼ���������֡���ճ���
//������BufferLenΪ0xFFFEʱ��ʾ���ò�������֡���ճ���
//������BufferLenΪ0xFFFDʱ��ʾ���ǺϷ���������(Cmd���Ϸ�),��Ӧ��
***************************************************************************/
u8 VDRProtocol_SendCmd(u8 Cmd, u8 *pBuffer, u16 BufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd00H
//���ܣ�00Hָ��Ӧ�𣬲ɼ���¼��ִ�б�׼�汾
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd00H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd01H
//���ܣ�01Hָ��Ӧ�𣬲ɼ���ǰ��ʻ����Ϣ
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd01H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd02H
//���ܣ�02Hָ��Ӧ�𣬲ɼ���¼��ʵʱʱ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd02H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd03H
//���ܣ�03Hָ��Ӧ�𣬲ɼ��ۼ���ʻ���
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd03H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd04H
//���ܣ�04Hָ��Ӧ�𣬲ɼ���¼������ϵ��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd04H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd05H
//���ܣ�05Hָ��Ӧ�𣬲ɼ�������Ϣ
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd05H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd06H
//���ܣ�06Hָ��Ӧ�𣬲ɼ���¼��״̬�ź�������Ϣ
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd06H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);
/**************************************************************************
//��������VDRProtocol_ParseCmd07H
//���ܣ�07Hָ��Ӧ�𣬲ɼ���¼��Ψһ�Ա��
//���룺pSrcBuffer:ָ��Դ���ݣ�ָ�����ݣ���SrcBufferLen:Դ���ݳ���
//    ��pSrcBuffer:ָ��Ŀ�����ݣ�Ӧ�����ݣ�
//�������
//����ֵ��Ӧ�����ݳ���
//��ע��
***************************************************************************/
u16 VDRProtocol_ParseCmd07H(u8 *pDstBuffer, u8 *pSrcBuffer, u16 SrcBufferLen);


#endif
