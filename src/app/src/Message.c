/*******************************************************************************
 * File Name:			message.c 
 * Function Describe:	
 * Relate Module:		��Ϣ�洢���绰�����¼�����
 * Writer:				Joneming
 * Date:				2012-05-21
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
////////////////////////
/*******************************************************************************/
#ifdef MESSAGE
/*******************************************************************************/

extern const char *s_apcMenuShortMsg[];

/////////////////////////////////

/////////////////////////////////
#define MSG_SAVE    0xbb
#define PHONE_HEAD  0xabcd
#define EVENT_HEAD  0xcdab
#define INFO_HEAD   0xdcba
#define DRIVER_HEAD  0xaabb
#define INFO_SERVICE_HEAD  0xccdd
#define QUESTION_HEAD  0xEFCD
//////////////////////////////
#define ONE_DRIVER_INFO_MAX_LEN  144
///////////////////////////////////
typedef struct
{ 
    unsigned char drivercode[6];            //����
    unsigned char password[6];              //����  
    unsigned char datalen;                  //��ʻԱ���ݽṹ��Ч����
    unsigned char reverse[3];               //����   
    unsigned char data[ONE_DRIVER_INFO_MAX_LEN];//�����б��ʻԱˢ�������ݽṹ
}STDRIVERINFO;

/******************************************************/
/***��ʻԱˢ�����ݽṹ**********************************
*0:     ��ʻԱ��������
*1:     ��ʻԱ����  n
*1+n:   ��ʻԱ���֤���� 20
*21+n:  ��ҵ�ʸ�֤���� 40
*61+n:  ��֤�������Ƴ���1
*62+n:  ��֤��������
***************************************************/
/**************************************************/

#define STDRIVERINFO_SIZE sizeof(STDRIVERINFO)
////////////////////////////
////////////////////////////////////////////////////////////////////
#define PHONEBOOK_START_ADDR            (unsigned long)0
#define PHONEBOOK_DATALEN               (unsigned long)FLASH_ONE_SECTOR_BYTES
/////////////////////////////////////////
#define EVENTREPORT_START_ADDR          (unsigned long)(PHONEBOOK_START_ADDR+PHONEBOOK_DATALEN)
#define EVENTREPORT_DATALEN             (unsigned long)FLASH_ONE_SECTOR_BYTES
/////////////////////////////
#define INFODEMAND_START_ADDR           (unsigned long)(EVENTREPORT_START_ADDR+EVENTREPORT_DATALEN)
#define INFODEMAND_DATALEN              (unsigned long)FLASH_ONE_SECTOR_BYTES
//////////////////////////////////////////////////
#define DRIVER_INFO_START_ADDR           (unsigned long)(INFODEMAND_START_ADDR+INFODEMAND_DATALEN)
#define DRIVER_INFO_DATALEN              (unsigned long)FLASH_ONE_SECTOR_BYTES
//////////////////////////////////////////////////
#define QUESTION_RESPONSE_START_ADDR        (unsigned long)(DRIVER_INFO_START_ADDR+DRIVER_INFO_DATALEN)//
#define QUESTION_RESPONSE_DATALEN           (unsigned long)FLASH_ONE_SECTOR_BYTES
//////////////////////////////////////////////////
#define INFO_SERVICE_START_ADDR             (unsigned long)(QUESTION_RESPONSE_START_ADDR+QUESTION_RESPONSE_DATALEN)//
#define INFO_SERVICE_DATALEN                (unsigned long)FLASH_ONE_SECTOR_BYTES
//////////////////////////////////


////////////�ռ����////////////////
#define MESSAGE_START_ADDR          (unsigned long)(FLASH_MESSAGE_START_SECTOR*FLASH_ONE_SECTOR_BYTES)
//////////////////////////////////////////
unsigned char s_ucMsgFlashBuffer[FLASH_ONE_SECTOR_BYTES];
//////////////////////////////////////////
//////////////////////////////////
#define ONE_QUESTION_RESPONSE_DATA_LEN          510
#define QUESTION_RESPONSE_MAX_COUNT   (unsigned char)((QUESTION_RESPONSE_DATALEN-STMESSAGEHEAD_SIZE)/ONE_QUESTION_RESPONSE_DATA_LEN)

#define ONE_INFO_SERVICE_DATA_LEN          510
#define INFO_SERVICE_MAX_COUNT   (unsigned char)((INFO_SERVICE_DATALEN-STMESSAGEHEAD_SIZE)/ONE_INFO_SERVICE_DATA_LEN)

enum 
{
    SETTYPE_DELETE,						//
    SETTYPE_UPDATE,						//
    SETTYPE_ADD,
    SETTYPE_MODIFY,
    SETTYPE_DEL_EX,
    SETTYPE_MAX,
}MESSAGESETTYPE;

enum 
{
    MSG_SET_DRIVER_INFO_EMPTY ,
    MSG_SET_DRIVER_INFO_UPDATE,						//      
    MSG_SET_DRIVER_INFO_ADD,
    MSG_SET_DRIVER_INFO_MODIFY,
    MSG_SET_DRIVER_INFO_DELETE,						//
    MSG_SET_DRIVER_INFO_QUERY,
    MSG_SET_DRIVER_INFO_MAX,
}E_MSGSET_DRIVER_INFO_TYPE;
////////////////////////////
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/ 
void EventReportDefaultValue(void);
unsigned char Messege_GetOneDriverInfoDatalen(unsigned char *pBuffer,unsigned short BufferLen);
/*************************************************************
** ��������: MessageFlashFormat()
** ��������: Flash��ʽ��
** ��ڲ���: startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void MessageFlashFormat(unsigned short startAddr) 
{
    //ע��startAddrΪ��Ե�ַ
    unsigned long addr;
    addr = startAddr;
    addr += MESSAGE_START_ADDR;
    ////////////////
      sFLASH_EraseSector(addr);
}

/*************************************************************
** ��������: MessageSaveDataToFlash()
** ��������: Flashд����
** ��ڲ���: s:���б����Դ����ָ��,startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void MessageSaveDataToFlash(unsigned char *s,unsigned short startAddr,unsigned short lenght) 
{
    //ע��startAddrΪ��Ե�ַ
    unsigned long addr;
    addr = startAddr;
    addr += MESSAGE_START_ADDR;
    /////////////////////    
    sFLASH_WriteBuffer(s,addr,lenght);
}

/*************************************************************
** ��������: MessageReadDataFromFlash()
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void MessageReadDataFromFlash(unsigned char *s,unsigned short startAddr,unsigned short lenght) 
{
    //ע��startAddrΪ��Ե�ַ
    unsigned long addr;
    addr = startAddr;
    addr += MESSAGE_START_ADDR;    
    ///////////////
    sFLASH_ReadBuffer(s,addr,lenght);
}

/*************************************************************
** ��������: PhoneBookReadDataFromFlash
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBookReadDataFromFlash(void) 
{
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,PHONEBOOK_START_ADDR,PHONEBOOK_DATALEN);
}

/*************************************************************
** ��������: PhoneBookSaveDataToFlash
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBookSaveDataToFlash(void) 
{
    ///////////////��д��֮ǰ�ȸ�ʽ��//////////////////////
    MessageFlashFormat(PHONEBOOK_START_ADDR);///
    ////////////////////////
    MessageSaveDataToFlash(s_ucMsgFlashBuffer,PHONEBOOK_START_ADDR,PHONEBOOK_DATALEN);
}

/*************************************************************
** ��������: EventReportReadDataFromFlash
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EventReportReadDataFromFlash(void) 
{
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,EVENTREPORT_START_ADDR,EVENTREPORT_DATALEN);
}

/*************************************************************
** ��������: EventReportSaveDataToFlash
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EventReportSaveDataToFlash(void) 
{
    MessageFlashFormat(EVENTREPORT_START_ADDR);
    ////////////////////////////
    MessageSaveDataToFlash(s_ucMsgFlashBuffer,EVENTREPORT_START_ADDR,EVENTREPORT_DATALEN);
}

/*************************************************************
** ��������: InfoDemandReadDataFromFlash
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoDemandReadDataFromFlash(void) 
{
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,INFODEMAND_START_ADDR,INFODEMAND_DATALEN);
}

/*************************************************************
** ��������: InfoDemandSaveDataToFlash
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoDemandSaveDataToFlash(void) 
{
    MessageFlashFormat(INFODEMAND_START_ADDR);
    ////////////////////////////
    MessageSaveDataToFlash(s_ucMsgFlashBuffer,INFODEMAND_START_ADDR,INFODEMAND_DATALEN);
}

/*************************************************************
** ��������: DriverInfoReadDataFromFlash
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void DriverInfoReadDataFromFlash(void) 
{
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,DRIVER_INFO_START_ADDR,DRIVER_INFO_DATALEN);
}
/*************************************************************
** ��������: DriverInfoSaveDataToFlash
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void DriverInfoSaveDataToFlash(void) 
{
    MessageFlashFormat(DRIVER_INFO_START_ADDR);
    ////////////////////////////
    MessageSaveDataToFlash(s_ucMsgFlashBuffer,DRIVER_INFO_START_ADDR,DRIVER_INFO_DATALEN);
}

/*************************************************************
** ��������: InfoServiceSaveDataToFlash
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoServiceReadDataFromFlash(void) 
{
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,INFO_SERVICE_START_ADDR,INFO_SERVICE_DATALEN);
}
/*************************************************************
** ��������: InfoServiceSaveDataToFlash
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoServiceSaveDataToFlash(void) 
{
    MessageFlashFormat(INFO_SERVICE_START_ADDR);
    ////////////////////////////
    MessageSaveDataToFlash(s_ucMsgFlashBuffer,INFO_SERVICE_START_ADDR,INFO_SERVICE_DATALEN);
}
/*************************************************************
** ��������: InfoServiceSaveDataToFlash
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void QuestionResponseReadDataFromFlash(void) 
{
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,QUESTION_RESPONSE_START_ADDR,QUESTION_RESPONSE_DATALEN);
}
/*************************************************************
** ��������: InfoServiceSaveDataToFlash
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void QuestionResponseSaveDataToFlash(void) 
{
    MessageFlashFormat(QUESTION_RESPONSE_START_ADDR);
    ////////////////////////////
    MessageSaveDataToFlash(s_ucMsgFlashBuffer,QUESTION_RESPONSE_START_ADDR,QUESTION_RESPONSE_DATALEN);
}
/*************************************************************
** 
**     Private  code 
** 
**************************************************************/
/*************************************************************
** ��������: PhoneBookDeleteAll
** ��������: ɾ�����е绰��
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Message_stMsgHeadInit(STMESSAGEHEAD *stMsgHead,unsigned short HeadData)
{
    stMsgHead->currentAddr  = STMESSAGEHEAD_SIZE;
    stMsgHead->HeadData     = HeadData;
    stMsgHead->saveFlag     = MSG_SAVE;
    stMsgHead->recordCnt    = 0;
    stMsgHead->oldestindex  = 0;
    stMsgHead->curindex     = 0;
}
/*************************************************************
** ��������: PhoneBookDeleteAll
** ��������: ɾ�����е绰��
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBookDeleteAll(void)
{
    STMESSAGEHEAD stMsgHead;
    MessageFlashFormat(PHONEBOOK_START_ADDR);
    ////////////////////////////////////
    Message_stMsgHeadInit(&stMsgHead,PHONE_HEAD);
    /////////////////////////////////////
    MessageSaveDataToFlash((unsigned char *)&stMsgHead,PHONEBOOK_START_ADDR,STMESSAGEHEAD_SIZE);
}
/*************************************************************
** ��������: GetPhoneBookAddress
** ��������: ����ָ����ϵ�˵��׵�ַ
** ��ڲ���: name:��ϵ������,num��¼����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short GetLinkmanAddressFromPhoneBook(unsigned char *name,unsigned char num)
{
    unsigned char i;
    unsigned short addr;
    STMSG_PHONEBOOK stPhone;
    addr = STMESSAGEHEAD_SIZE;
    for(i = 0; i < num; i++)
    {
        memcpy(&stPhone,&s_ucMsgFlashBuffer[addr],STPHONEBOOK_SIZE);
        if(!strcmp((char *)stPhone.name,(char *)name))
        {
            return addr;
        }
        addr +=STPHONEBOOK_SIZE;
    }
    return 0;
}
/*************************************************************
** ��������: PhoneBookUpdate
** ��������: �������е绰��
** ��ڲ���: ���µ绰������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBookUpdate(unsigned char *data,unsigned short datalen)
{
    unsigned char i,length,num;
    unsigned short index,addr;
    STMSG_PHONEBOOK stPhone;
    STMESSAGEHEAD  stMsgHead; 
    Message_stMsgHeadInit(&stMsgHead,PHONE_HEAD);
    memset(s_ucMsgFlashBuffer,0,sizeof(s_ucMsgFlashBuffer));
    index = 0;
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stPhone.callflag = data[index++];
        length = data[index++];
        ////////////////////////
        if(length==0)return;
        memcpy(stPhone.number,&data[index],length);
        stPhone.number[length] = '\0';
        index += length;
        ///////////////////////
        length = data[index++];
        memcpy(stPhone.name,&data[index],length);
        stPhone.name[length] = '\0';
        index+=length;
        addr=GetLinkmanAddressFromPhoneBook(stPhone.name,stMsgHead.recordCnt);
        if(addr)//�ҵ���Ӧ��ϵ��
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stPhone,STPHONEBOOK_SIZE);
        }
        else
        {
            ////////////////////
            if(stMsgHead.currentAddr+STPHONEBOOK_SIZE > PHONEBOOK_DATALEN)break;
            ///////////////////////////////////////////
            memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stPhone,STPHONEBOOK_SIZE);
            ////////////////////////
            stMsgHead.currentAddr += STPHONEBOOK_SIZE;
            stMsgHead.recordCnt++;
        }
        //////////////
        if(index>=datalen)break;
    }
    ///////////////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    /////////////////////////////////
    PhoneBookSaveDataToFlash();
}
/*************************************************************
** ��������: PhoneBookAdd
** ��������: ׷�ӵ绰��
** ��ڲ���: ׷�ӵ绰������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBookAdd(unsigned char *data,unsigned short datalen)
{
    unsigned char i,length,num;
    unsigned short index,addr;
    STMSG_PHONEBOOK stPhone;
    STMESSAGEHEAD stMsgHead;
    PhoneBookReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    index = 0;    
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stPhone.callflag = data[index++];
        length = data[index++];
        ////////////////////////
        if(length==0)return;
        memcpy(stPhone.number,&data[index],length);
        stPhone.number[length] = '\0';
        index += length;
        ///////////////////////
        length = data[index++];
        memcpy(stPhone.name,&data[index],length);
        stPhone.name[length] = '\0';
        index+=length;
        addr=GetLinkmanAddressFromPhoneBook(stPhone.name,stMsgHead.recordCnt);
        if(addr)//�ҵ���Ӧ��ϵ��
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stPhone,STPHONEBOOK_SIZE);
        }
        else
        {
            ////////////////////
            if(stMsgHead.currentAddr+STPHONEBOOK_SIZE > PHONEBOOK_DATALEN)break;
            /////////////////////////
            memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stPhone,STPHONEBOOK_SIZE);
            ////////////////////////
            stMsgHead.currentAddr+=STPHONEBOOK_SIZE;
            stMsgHead.recordCnt++;
        }
        //////////////
        if(index>=datalen)break;
    }
    ///////////////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    /////////////////////////////////
    ////////////////////
    PhoneBookSaveDataToFlash();
}

/*************************************************************
** ��������: PhoneBookModify
** ��������: �޸ĵ绰��
** ��ڲ���: �޸ĵ绰������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBookModify(unsigned char *data,unsigned short datalen)
{
    unsigned char i,length,num;
    unsigned short index,addr;
    STMSG_PHONEBOOK stPhone;
    STMESSAGEHEAD  stMsgHead;
    PhoneBookReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    index = 0;    
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stPhone.callflag = data[index++];
        length = data[index++];
        if(length==0)return;
        ////////////////////////
        memcpy(stPhone.number,&data[index],length);
        stPhone.number[length] = '\0';
        index += length;
        ///////////////////////
        length = data[index++];
        memcpy(stPhone.name,&data[index],length);
        stPhone.name[length] = '\0';
        index+=length;
        ////////////////////
        addr=GetLinkmanAddressFromPhoneBook(stPhone.name,stMsgHead.recordCnt);
        if(addr)//�ҵ���Ӧ��ϵ��
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stPhone,STPHONEBOOK_SIZE);
        }
        //////////////
        if(index>=datalen)break;
    }    
    /////////////////////////////////
    ////////////////////
    PhoneBookSaveDataToFlash();
}

/*************************************************************
** ��������: EventReportDeleteAll
** ��������: ɾ�������¼�����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EventReportDeleteAll(void)
{
    STMESSAGEHEAD stMsgHead;
    MessageFlashFormat(EVENTREPORT_START_ADDR);
    ////////////////////////////////////
    Message_stMsgHeadInit(&stMsgHead,EVENT_HEAD);
    /////////////////////////////////////
    MessageSaveDataToFlash((unsigned char *)&stMsgHead,EVENTREPORT_START_ADDR,STMESSAGEHEAD_SIZE);
    
}
/*************************************************************
** ��������: GetEventReportAddress
** ��������: ����ָ���¼�����ID���׵�ַ
** ��ڲ���: ID:�¼�����ID,num��¼����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short GetAddressFromEventReport(unsigned char ID,unsigned char num)
{
    unsigned char i;
    unsigned short addr;
    STMESSAGE stEvent;
    addr = STMESSAGEHEAD_SIZE;
    for(i = 0; i < num; i++)
    {
        memcpy(&stEvent,&s_ucMsgFlashBuffer[addr],STMESSAGE_SIZE);
        if(stEvent.ID==ID)
        {
            return addr;
        }
        addr +=STMESSAGE_SIZE;
    }
    return 0;
}
/*************************************************************
** ��������: EventReportUpdate
** ��������: ���������¼�����
** ��ڲ���: �����¼���������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EventReportUpdate(unsigned char *data,unsigned short datalen)
{
    unsigned char i,length,num;
    unsigned short index,addr;
    STMESSAGE stEvent;
    STMESSAGEHEAD  stMsgHead; 
    Message_stMsgHeadInit(&stMsgHead,EVENT_HEAD);
    memset(s_ucMsgFlashBuffer,0,sizeof(s_ucMsgFlashBuffer));
    index = 0;
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stEvent.ID = data[index++];
        length = data[index++];
        ////////////////////////
        if(length==0)return;
        memcpy(stEvent.content,&data[index],length);
        stEvent.content[length] = '\0';
        index += length;
        addr=GetAddressFromEventReport(stEvent.ID,stMsgHead.recordCnt);
        if(addr)//�ҵ��¼�����
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stEvent,STMESSAGE_SIZE);
        }
        else
        {
            ///////////////////////
            if(stMsgHead.currentAddr+STMESSAGE_SIZE > EVENTREPORT_DATALEN)break;
            ////////////////////
            memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stEvent,STMESSAGE_SIZE);
            ////////////////////////
            stMsgHead.currentAddr += STMESSAGE_SIZE;
            stMsgHead.recordCnt++;
        }        
        //////////////
        if(index>=datalen)break;
    }
    ///////////////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    /////////////////////////////////
    EventReportSaveDataToFlash();
}
/*************************************************************
** ��������: EventReportAdd
** ��������: ׷���¼�����
** ��ڲ���: ׷���¼���������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EventReportAdd(unsigned char *data,unsigned short datalen)
{
    unsigned char i,length,num;
    unsigned short index,addr;
    STMESSAGE stEvent;
    STMESSAGEHEAD stMsgHead;
    EventReportReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    index = 0;    
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stEvent.ID = data[index++];
        length = data[index++];        
        ////////////////////////
        if(length==0)return;
        memcpy(stEvent.content,&data[index],length);
        stEvent.content[length] = '\0';
        index += length;
        ///////////////////////
        addr=GetAddressFromEventReport(stEvent.ID,stMsgHead.recordCnt);
        if(addr)//�ҵ��¼�����
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stEvent,STMESSAGE_SIZE);
        }
        else
        {
            if(stMsgHead.currentAddr+STMESSAGE_SIZE > EVENTREPORT_DATALEN)break;
            ////////////////////
            memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stEvent,STMESSAGE_SIZE);
            ////////////////////////
            stMsgHead.currentAddr+=STMESSAGE_SIZE;
            stMsgHead.recordCnt++;
            //////////////
        }
        if(index>=datalen)break;
    }
    ///////////////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    /////////////////////////////////
    ////////////////////
    EventReportSaveDataToFlash();
}
/*************************************************************
** ��������: EventReportModify
** ��������: �޸��¼�����
** ��ڲ���: �޸��¼���������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EventReportModify(unsigned char *data,unsigned short datalen)
{
    unsigned char i,length,num;
    unsigned short index,addr;
    STMESSAGE stEvent;
    STMESSAGEHEAD  stMsgHead;
    EventReportReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    index = 0;    
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stEvent.ID = data[index++];
        length = data[index++];
        ////////////////////////
        memcpy(stEvent.content,&data[index],length);
        stEvent.content[length] = '\0';
        index += length;
        ////////////////////
        addr=GetAddressFromEventReport(stEvent.ID,stMsgHead.recordCnt);
        if(addr)//�ҵ��¼�����
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stEvent,STMESSAGE_SIZE);
        }
        //////////////
        if(index>=datalen)break;
    }    
    /////////////////////////////////
    ////////////////////
    EventReportSaveDataToFlash();
}
/*************************************************************
** ��������: EventReportDelEx
** ��������: ɾ���ض������¼�
** ��ڲ���: �¼���������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char EventReportDelEx(unsigned char *data,unsigned short datalen)
{
    unsigned char i,num;
    unsigned short index,addr,tmpAddr,length;
    STMESSAGE stEvent;
    STMESSAGEHEAD  stMsgHead;
    EventReportReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    index = 0;    
    num = data[index++];
    if(num>stMsgHead.recordCnt)return 1;//�����¼�����
    for(i = 0; i < num; i++)
    {
        stEvent.ID = data[index++];        
        ////////////////////
        if(0==GetAddressFromEventReport(stEvent.ID,stMsgHead.recordCnt))//δ�ҵ��¼�����
        {
            return 2;
        }
    }
    ///////////////
    index = 0;    
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stEvent.ID = data[index++];        
        ////////////////////
        addr=GetAddressFromEventReport(stEvent.ID,stMsgHead.recordCnt);
        length=stMsgHead.recordCnt*STMESSAGE_SIZE+STMESSAGEHEAD_SIZE;
        tmpAddr=addr+STMESSAGE_SIZE;
        if(length>tmpAddr)
        {
            memmove(&s_ucMsgFlashBuffer[addr],&s_ucMsgFlashBuffer[tmpAddr],length-tmpAddr);
        }
        if(stMsgHead.recordCnt)
        {
            stMsgHead.recordCnt--;
            stMsgHead.currentAddr -= STMESSAGE_SIZE;
        }
        //////////////
        if(index>=datalen)break;
    }
    ///////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    /////////////////////////////////
    EventReportSaveDataToFlash();
    return 0;
}
/*************************************************************
** ��������: EventReportDefaultValue
** ��������: ��Ԥ�ö���ŵ��¼�������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EventReportDefaultValue(void)
{
    STMESSAGEHEAD stMsgHead;
    STMESSAGE stEvent;
    unsigned char i;
    i = 0;
    for(;;)
    {
        if(strlen(s_apcMenuShortMsg[i]) == 0)break;
        i ++;
    }
    //////////////////////////////////
    Message_stMsgHeadInit(&stMsgHead,EVENT_HEAD);
    memset(s_ucMsgFlashBuffer,0,sizeof(s_ucMsgFlashBuffer));
    stMsgHead.recordCnt = i;    
    for(i=0; i<stMsgHead.recordCnt;i++)
    {
        stEvent.ID=i+1;
        strcpy((char *)stEvent.content,s_apcMenuShortMsg[i]);  
        memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stEvent,STMESSAGE_SIZE);
        stMsgHead.currentAddr +=STMESSAGE_SIZE;
    }
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    ///////////////////////////
    EventReportSaveDataToFlash();
}
/*************************************************************
** ��������: GetEventReportTotalNum
** ��������: ȡ���¼�����������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetEventReportTotalNum(void)
{
    unsigned char buffer[STMESSAGEHEAD_SIZE];
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(buffer,EVENTREPORT_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,buffer,STMESSAGEHEAD_SIZE);
    if(EVENT_HEAD==stMsgHead.HeadData&&MSG_SAVE==stMsgHead.saveFlag)
    {
        return stMsgHead.recordCnt;
    }
    ///////////////////////
    return 0;    
}
/*************************************************************
** ��������: GetOneEventReportDetails
** ��������: ȡ����Ӧ��ŵ��¼���������
** ��ڲ���: index���(��0��ʼ),
** ���ڲ���: stEvent�¼�����ṹ��
** ���ز���: 1:�ɹ�,0:ʧ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetOneEventReportDetails(unsigned char index,STMESSAGE *stEvent)
{
    unsigned char buffer[STMESSAGEHEAD_SIZE];
    unsigned short addr;
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(buffer,EVENTREPORT_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,buffer,STMESSAGEHEAD_SIZE);
    if(EVENT_HEAD==stMsgHead.HeadData&&MSG_SAVE==stMsgHead.saveFlag)
    {
        if(index<stMsgHead.recordCnt)
        {
            addr = index*STMESSAGE_SIZE;
            addr += STMESSAGEHEAD_SIZE;            
            MessageReadDataFromFlash((unsigned char *)stEvent,EVENTREPORT_START_ADDR+addr,STMESSAGE_SIZE);
            return 1;
        }
    }
    return 0;
    /////////////////////// 
}
/*************************************************************
** ��������: InfoDemandDeleteAll
** ��������: ɾ��������Ϣ�㲥
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoDemandDeleteAll(void)
{
    STMESSAGEHEAD stMsgHead;
    MessageFlashFormat(INFODEMAND_START_ADDR);
    ////////////////////////////////////
    Message_stMsgHeadInit(&stMsgHead,INFO_HEAD);
    /////////////////////////////////////
    MessageSaveDataToFlash((unsigned char *)&stMsgHead,INFODEMAND_START_ADDR,STMESSAGEHEAD_SIZE);
}
/*************************************************************
** ��������: GetInfoDemandAddress
** ��������: ����ָ����Ϣ�㲥ID���׵�ַ
** ��ڲ���: ID:��Ϣ�㲥ID,num��¼����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short GetAddressFromInfoDemand(unsigned char ID,unsigned char num)
{
    unsigned char i;
    unsigned short addr;
    STMESSAGE stInfo;
    addr = STMESSAGEHEAD_SIZE;
    for(i = 0; i < num; i++)
    {
        memcpy(&stInfo,&s_ucMsgFlashBuffer[addr],STMESSAGE_SIZE);
        if(stInfo.ID==ID)
        {
            return addr;
        }
        addr +=STMESSAGE_SIZE;
    }
    return 0;
}
/*************************************************************
** ��������: InfoDemandUpdate
** ��������: ����������Ϣ�㲥
** ��ڲ���: ������Ϣ�㲥����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoDemandUpdate(unsigned char *data,unsigned short datalen)
{
    unsigned char i,num;
    unsigned short index,length,addr;
    STMESSAGE stInfo;
    STMESSAGEHEAD  stMsgHead;
    Message_stMsgHeadInit(&stMsgHead,INFO_HEAD);
    memset(s_ucMsgFlashBuffer,0,sizeof(s_ucMsgFlashBuffer));
    index = 0;    
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stInfo.ID = data[index++];
        length = data[index++];
        length<<=8;
        length |= data[index++];
        if(length==0)return;
        ////////////////////////
        memcpy(stInfo.content,&data[index],length);
        stInfo.content[length] = '\0';
        index += length;
        stInfo.status = 0;
        addr=GetAddressFromInfoDemand(stInfo.ID,stMsgHead.recordCnt);
        if(addr)//�ҵ���Ϣ�㲥
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stInfo,STMESSAGE_SIZE);
        }
        else
        {
            ///////////////////////
            if(stMsgHead.currentAddr+STMESSAGE_SIZE > INFODEMAND_DATALEN)break;
            ////////////////////
            memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stInfo,STMESSAGE_SIZE);
            ////////////////////////
            stMsgHead.currentAddr += STMESSAGE_SIZE;
            stMsgHead.recordCnt++;
        }
        //////////////
        if(index>=datalen)break;
    }
    ///////////////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    /////////////////////////////////
    InfoDemandSaveDataToFlash();
}
/*************************************************************
** ��������: InfoDemandAdd
** ��������: ׷����Ϣ�㲥
** ��ڲ���: ׷����Ϣ�㲥����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoDemandAdd(unsigned char *data,unsigned short datalen)
{
    unsigned char i,num;
    unsigned short index,length,addr;
    STMESSAGE stInfo;
    STMESSAGEHEAD stMsgHead;
    InfoDemandReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    index = 0;    
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stInfo.ID = data[index++];
        length = data[index++];
        length<<=8;
        length |= data[index++];
        if(length==0)return;
        ////////////////////////
        memcpy(stInfo.content,&data[index],length);
        stInfo.content[length] = '\0';
        index += length;
        ///////////////////////
        stInfo.status = 0;
        addr=GetAddressFromInfoDemand(stInfo.ID,stMsgHead.recordCnt);
        if(addr)//�ҵ���Ϣ�㲥
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stInfo,STMESSAGE_SIZE);
        }
        else
        {
            ////////////////////////
            if(stMsgHead.currentAddr+STMESSAGE_SIZE > INFODEMAND_DATALEN)break;
            ////////////////////
            memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stInfo,STMESSAGE_SIZE);
            ////////////////////////
            stMsgHead.currentAddr+=STMESSAGE_SIZE;
            stMsgHead.recordCnt ++;
        }
        //////////////
        if(index>=datalen)break;
    }
    ///////////////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    /////////////////////////////////
    ////////////////////
    InfoDemandSaveDataToFlash();
}

/*************************************************************
** ��������: InfoDemandModify
** ��������: �޸���Ϣ�㲥
** ��ڲ���: �޸���Ϣ�㲥����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoDemandModify(unsigned char *data,unsigned short datalen)
{
    unsigned char i,num;
    unsigned short index,addr,length;
    STMESSAGE stInfo;
    STMESSAGEHEAD  stMsgHead;
    InfoDemandReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    index = 0;    
    num = data[index++];
    for(i = 0; i < num; i++)
    {
        stInfo.ID = data[index++];
        length = data[index++];
        length<<=8;
        length |= data[index++];
        if(length==0)return;
        ////////////////////////
        memcpy(stInfo.content,&data[index],length);
        stInfo.content[length] = '\0';
        index += length;
        ////////////////
        stInfo.status = 0;
        ////////////////////
        addr=GetAddressFromInfoDemand(stInfo.ID,stMsgHead.recordCnt);
        if(addr)//�ҵ���Ϣ�㲥
        {
            memcpy(&s_ucMsgFlashBuffer[addr],&stInfo,STMESSAGE_SIZE);
        }
        //////////////
        if(index>=datalen)break;
    }    
    /////////////////////////////////
    ////////////////////
    InfoDemandSaveDataToFlash();
}

/*************************************************************
** ��������: GetInfoDemandTotalNum
** ��������: ȡ����Ϣ�㲥������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetInfoDemandTotalNum(void)
{
    unsigned char buffer[STMESSAGEHEAD_SIZE];
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(buffer,INFODEMAND_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,buffer,STMESSAGEHEAD_SIZE);
    if(INFO_HEAD==stMsgHead.HeadData&&MSG_SAVE==stMsgHead.saveFlag)
    {
        return stMsgHead.recordCnt;
    }
    ///////////////////////
    return 0;    
}
/*************************************************************
** ��������: GetOneInfoDemandDetails
** ��������: ȡ����Ӧ��ŵ���Ϣ�㲥����
** ��ڲ���: index���(��0��ʼ),
** ���ڲ���: stInfo��Ϣ�㲥�ṹ��
** ���ز���: 1:�ɹ�,0:ʧ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetOneInfoDemandDetails(unsigned char index,STMESSAGE *stInfo)
{
    unsigned char buffer[STMESSAGEHEAD_SIZE];
    unsigned short addr;
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(buffer,INFODEMAND_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,buffer,STMESSAGEHEAD_SIZE);
    if(INFO_HEAD==stMsgHead.HeadData&&MSG_SAVE==stMsgHead.saveFlag)
    {
        if(index<stMsgHead.recordCnt)
        {
            addr = index*STMESSAGE_SIZE;
            addr += STMESSAGEHEAD_SIZE;            
            MessageReadDataFromFlash((unsigned char *)stInfo,INFODEMAND_START_ADDR+addr,STMESSAGE_SIZE);
            return 1;
        }
    }
    return 0;
    /////////////////////// 
}
/*************************************************************
** ��������: DriverInfoDeleteAll
** ��������: ɾ�����м�ʻԱ��Ϣ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void DriverInfoDeleteAll(void)
{
    STMESSAGEHEAD stMsgHead;
    MessageFlashFormat(DRIVER_INFO_START_ADDR);
    ////////////////////////////////////
    Message_stMsgHeadInit(&stMsgHead,DRIVER_HEAD);
    /////////////////////////////////////
    MessageSaveDataToFlash((unsigned char *)&stMsgHead,DRIVER_INFO_START_ADDR,STMESSAGEHEAD_SIZE);
    ///////////////////////////////
    #ifdef USE_NAVILCD
    NaviLcd_DriverLogout();
    #endif
}
/*************************************************************
** ��������: GetDriverInfoRecordNumber
** ��������: ȡ����Ӧ���ŵļ�¼��(��1��ʼ���)
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetDriverInfoRecordNumber(unsigned char *drivercode)
{
    unsigned char i;
    unsigned short addr;
    STDRIVERINFO stDriver;
    STMESSAGEHEAD  stMsgHead;
    DriverInfoReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr = STMESSAGEHEAD_SIZE;
    for(i = 0; i < stMsgHead.recordCnt; i++)
    {
        memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);
        if(strncmp((char*)drivercode,(char*)stDriver.drivercode,6)==0)
        {
            return i+1;
        }
        addr +=STDRIVERINFO_SIZE;
    }
    /////////////////////
    return 0;
}
/*************************************************************
** ��������: GetDriverInfoRecordNumber
** ��������: ȡ����Ӧ���ŵļ�¼��(��1��ʼ���)
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetDriverInfoRecordNumberEx(unsigned char *drivercode)
{
    unsigned char i;
    unsigned short addr;
    STDRIVERINFO stDriver;
    STMESSAGEHEAD  stMsgHead;
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr = STMESSAGEHEAD_SIZE;
    for(i = 0; i < stMsgHead.recordCnt; i++)
    {
        memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);
        if(strncmp((char*)drivercode,(char*)stDriver.drivercode,6)==0)
        {
            return i+1;
        }
        addr +=STDRIVERINFO_SIZE;
    }
    /////////////////////
    return 0;
}

/*************************************************************
** ��������: GetDriverInfoTotalNumber
** ��������: ��ȡ��ǰ��ʻԱ�ܸ���
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 0��ʾ�޼�ʻԱ,����Ϊ��ʻԱ����
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetDriverInfoTotalNumber(void)
{
    unsigned char buffer[20];
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(buffer,DRIVER_INFO_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,buffer,STMESSAGEHEAD_SIZE);
    /////////////////////
    if(DRIVER_HEAD==stMsgHead.HeadData&&MSG_SAVE==stMsgHead.saveFlag)
    {
        return stMsgHead.recordCnt;
    }
    //////////////////////
    return 0;
}
/*************************************************************
** ��������: GetDriverInfoRecordNumberForCheckData
** ��������: ����ʻԱ��������ʻԱ���֤���룬ȡ����Ӧ��¼��(��1��ʼ���)
** ��ڲ���: �����ϱ��ĸ�ʽ������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char GetDriverInfoRecordNumberForCheckData(unsigned char *data)
{
    unsigned char i,len;
    unsigned short addr;
    STDRIVERINFO stDriver;
    STMESSAGEHEAD  stMsgHead;    
    DriverInfoReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr = STMESSAGEHEAD_SIZE;
    for(i = 0; i < stMsgHead.recordCnt; i++)
    {
        memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);
        len=stDriver.data[0];//��������
        len += 19;//��ʻԱ����18���ֽ�+��������1���ֽ�+����ʵ�ʳ���
        if(strncmp((char*)stDriver.data,(char*)data,len)==0)
        {
            return i+1;
        }
        addr +=STDRIVERINFO_SIZE;
    }
    /////////////////////
    return 0;
}
/*************************************************************
** ��������: CheckDriverInfoForLogin
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char CheckDriverInfoForLogin(unsigned char *drivercode,unsigned char *password,unsigned char* recordnum)
{
    unsigned char i;
    unsigned short addr;
    STDRIVERINFO stDriver;
    STMESSAGEHEAD  stMsgHead;
    DriverInfoReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr = STMESSAGEHEAD_SIZE;
    for(i = 0; i < stMsgHead.recordCnt; i++)
    {
        memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);
        if(strncmp((char*)drivercode,(char*)stDriver.drivercode,6)==0)
        {
            if(strncmp((char*)password,(char*)stDriver.password,6)==0)
            {
                *recordnum =i+1;
                return 0;
            }
            else
            {
                ////////////////////
                Public_PlayTTSVoiceStr("�������!");
                return 1;
            }
        }
        addr +=STDRIVERINFO_SIZE;
    }
    /////////////////////
    Public_PlayTTSVoiceStr("�ù��Ų�����");
    return 2;
}
/*************************************************************
** ��������: DriverInfoAdd
** ��������: ׷�Ӽ�ʻԱ��Ϣ
** ��ڲ���: ׷�Ӽ�ʻԱ��Ϣ
** ���ڲ���: ���ش洢�ļ�¼��(��Ŵ�1��ʼ),0:��ʾ��Ϣ����򱣴����
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char DriverInfoAdd(unsigned char *index,unsigned char *drivercode,unsigned char *password,unsigned char *data,unsigned short datalen)
{
    STDRIVERINFO stDriver;
    STMESSAGEHEAD stMsgHead;
    unsigned char num;
    /////////////////////////
    if(0==Messege_GetOneDriverInfoDatalen(data,datalen))
    {
        Public_PlayTTSVoiceStr("���ݸ�ʽ����");
        return 4;
    }
    num=GetDriverInfoRecordNumber(drivercode);//��������ͬ�Ĺ���û��
    if(num)
    {
        Public_PlayTTSVoiceStr("�ù����Ѵ���");
        return 1;
    }
    //////////////////////
    num=GetDriverInfoRecordNumberForCheckData(data);
    if(num)
    {
        Public_PlayTTSVoiceStr("�ü�ʻԱ�Ѵ���");
        return 2;
    }
    ///////////////////////////////////
    ////////////////////////////////////
    DriverInfoReadDataFromFlash();    
    memcpy(&stMsgHead,&s_ucMsgFlashBuffer[0],STMESSAGEHEAD_SIZE);
    /////////////////////////////////
    if((stMsgHead.currentAddr+STDRIVERINFO_SIZE)>INFODEMAND_DATALEN)
    {
        Public_PlayTTSVoiceStr("�ռ䲻��");
        return 3;
    }
    ////////////////////////////////////    
    stDriver.datalen = datalen;
    memcpy(stDriver.drivercode,drivercode,6);
    memcpy(stDriver.password,password,6);
    memcpy(stDriver.data,data,datalen);
    memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stDriver,STDRIVERINFO_SIZE);
    ////////////////////
    stMsgHead.currentAddr +=STDRIVERINFO_SIZE;
    stMsgHead.recordCnt ++;
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    DriverInfoSaveDataToFlash(); 
    ///////////////////
    *index =stMsgHead.recordCnt;
    return 0;
}

/*************************************************************
** ��������: DriverInfoModifyDriverCodeAndPassword
** ��������: �޸ļ�ʻԱ��Ϣ����/����
** ��ڲ���: �޸ļ�ʻԱ��Ϣ
** ���ڲ���: 
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char DriverInfoModifyDriverCodeAndPassword(unsigned char index ,unsigned char *drivercode,unsigned char *password)
{
    unsigned short addr;
    STDRIVERINFO stDriver;
    STMESSAGEHEAD stMsgHead;
    unsigned char num;
    /////////////////////
    DriverInfoReadDataFromFlash();    
    memcpy(&stMsgHead,&s_ucMsgFlashBuffer[0],STMESSAGEHEAD_SIZE);
    ///////////////////////////////
    if(stMsgHead.recordCnt<index)return 3;//δ��¼,
    if(index==0)return 3;//δ��¼,
    num=GetDriverInfoRecordNumber(drivercode);
    if(num&& (num != index))
    {
        Public_PlayTTSVoiceStr("�ù����Ѵ���");
        return 2;//������Ŵ���,�����ǵ�ǰ��Ҫ�޸ĵļ�¼�������
    }
    /////////////////////////////////
    addr =(index-1)*STDRIVERINFO_SIZE;
    addr += STMESSAGEHEAD_SIZE;
    ///////////////////
    memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);
    //////////////////////////////////
    memcpy(stDriver.drivercode,drivercode,6);
    memcpy(stDriver.password,password,6);
    memcpy(&s_ucMsgFlashBuffer[addr],&stDriver,STDRIVERINFO_SIZE);
    ////////////////////
    DriverInfoSaveDataToFlash();  
    ///////////////////////////////
    Public_PlayTTSVoiceStr("�޸ĳɹ�,���ס�޸�����");
    return 0;
}

/*************************************************************
** ��������: DriverInfoModify
** ��������: �޸ļ�ʻԱ��Ϣ
** ��ڲ���: �޸ļ�ʻԱ��Ϣ,index(��Ŵ�1��ʼ),
** ���ڲ���: 
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char DriverInfoModify(unsigned char *index,unsigned char *drivercode,unsigned char *password,unsigned char *data,unsigned char datalen)
{
    unsigned short addr;
    STDRIVERINFO stDriver;
    STMESSAGEHEAD stMsgHead; 
    unsigned char num;
    /////////////////////
    if(0==Messege_GetOneDriverInfoDatalen(data,datalen))
    {
        Public_PlayTTSVoiceStr("���ݸ�ʽ����");
        return 4;
    }
    DriverInfoReadDataFromFlash();    
    memcpy(&stMsgHead,&s_ucMsgFlashBuffer[0],STMESSAGEHEAD_SIZE);
    ///////////////////////////////
    if(stMsgHead.recordCnt<*index)
    {
        Public_PlayTTSVoiceStr("�ü�¼�Ų�����");
        return 4;
    }
    ////////////////////
    if(*index==0)
    {
        Public_PlayTTSVoiceStr("�ü�¼��Ϊ0");
        return 4;
    }
    //////////////////////////
    num=GetDriverInfoRecordNumber(drivercode);
    if(num&& (num != *index))
    {
        Public_PlayTTSVoiceStr("�ù����Ѵ���");
        return 1;//������Ŵ���,�����ǵ�ǰ��Ҫ�޸ĵļ�¼�������
    }
    /////////////////////
    num=GetDriverInfoRecordNumberForCheckData(data);
    if(num&& (num != *index))
    {
        Public_PlayTTSVoiceStr("�ü�ʻԱ�Ѵ���");
        return 2;//�����ʻԱ��������ʻԱ���֤�������,�����ǵ�ǰ��Ҫ�޸ĵļ�¼�������
    }
    ////////////////////////////
    /////////////////////////////////
    addr =(*index-1)*STDRIVERINFO_SIZE;
    addr += STMESSAGEHEAD_SIZE;
    memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);
    stDriver.datalen = datalen;
    memcpy(stDriver.drivercode,drivercode,6);
    memcpy(stDriver.password,password,6);
    memcpy(stDriver.data,data,datalen);
    memcpy(&s_ucMsgFlashBuffer[addr],&stDriver,STDRIVERINFO_SIZE);
    ////////////////////
    DriverInfoSaveDataToFlash();  
    return 0;
}

/*************************************************************
** ��������: DriverInfoAddOrModify
** ��������: ׷��/�޸ļ�ʻԱ��Ϣ
** ��ڲ���: ׷��/�޸ļ�ʻԱ��Ϣ
** ���ڲ���: ���ش洢�ļ�¼��(��Ŵ�1��ʼ)0:��ʾ��Ϣ����򱣴����
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char DriverInfoAddOrModify(unsigned char *index,unsigned char *drivercode,unsigned char *password,unsigned char *data,unsigned short datalen)
{
    unsigned char result;
    STMESSAGEHEAD stMsgHead; 
    /////////////////////
    DriverInfoReadDataFromFlash();    
    memcpy(&stMsgHead,&s_ucMsgFlashBuffer[0],STMESSAGEHEAD_SIZE);
    if(stMsgHead.recordCnt>=*index)
    {
        result=DriverInfoModify(index,drivercode,password,data,datalen);
        if(0==result)
        {
            Public_PlayTTSVoiceStr("��ʻԱ�޸ĳɹ�");
        }
    }
    else
    {
        result=DriverInfoAdd(index,drivercode,password,data,datalen);
        if(0==result)
        {
            Public_PlayTTSVoiceStr("��ʻԱ��ӳɹ�");
        }
    }
    ///////////////
    return result;
    ///////////////////////////////
}
/*************************************************************
** ��������: MessageReadAndCheckPhoneBook
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void MessageReadAndCheckPhoneBook(void) //
{
    STMESSAGEHEAD stMsgHead;
    ////////////////////////////////////
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,PHONEBOOK_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if((stMsgHead.HeadData != PHONE_HEAD)|| (stMsgHead.saveFlag != MSG_SAVE)||(stMsgHead.currentAddr>PHONEBOOK_DATALEN)||(stMsgHead.recordCnt*STPHONEBOOK_SIZE+STMESSAGEHEAD_SIZE!= stMsgHead.currentAddr))
    {
        PhoneBookDeleteAll();
    }
}
/*************************************************************
** ��������: MessageReadAndCheckEventReport
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void MessageReadAndCheckEventReport(void) //
{
    STMESSAGEHEAD stMsgHead;
    /////////////////////////////////
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,EVENTREPORT_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if((stMsgHead.HeadData != EVENT_HEAD)|| (stMsgHead.saveFlag != MSG_SAVE)||(stMsgHead.currentAddr>INFODEMAND_DATALEN)||(stMsgHead.recordCnt*STMESSAGE_SIZE+STMESSAGEHEAD_SIZE!= stMsgHead.currentAddr))
    {
        //EventReportDeleteAll();
        EventReportDefaultValue();
    }   
}
/*************************************************************
** ��������: MessageReadAndCheckInfoDemand
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void MessageReadAndCheckInfoDemand(void) //
{
    STMESSAGEHEAD stMsgHead;
    /////////////////////////////////
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,INFODEMAND_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if((stMsgHead.HeadData != INFO_HEAD)|| (stMsgHead.saveFlag != MSG_SAVE)||(stMsgHead.currentAddr>INFODEMAND_DATALEN) ||(stMsgHead.recordCnt*STMESSAGE_SIZE+STMESSAGEHEAD_SIZE!= stMsgHead.currentAddr))
    {
        InfoDemandDeleteAll();
    }   
}
/*************************************************************
** ��������: MessageReadAndCheckInfoDemand
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void MessageReadAndCheckDriverInfo(void) //
{
    STMESSAGEHEAD stMsgHead;
    /////////////////////////////////
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,DRIVER_INFO_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if((stMsgHead.HeadData != DRIVER_HEAD)|| (stMsgHead.saveFlag != MSG_SAVE)||(stMsgHead.currentAddr>DRIVER_INFO_DATALEN)||(stMsgHead.recordCnt*STDRIVERINFO_SIZE+STMESSAGEHEAD_SIZE!= stMsgHead.currentAddr))
    {
        DriverInfoDeleteAll();
    }
}

/*************************************************************
** ��������: InfoServiceDeleteAll
** ��������: ɾ��������Ϣ��������
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoServiceDeleteAll(void)
{
    STMESSAGEHEAD stMsgHead;
    MessageFlashFormat(INFO_SERVICE_START_ADDR);
    ////////////////////////////////////
    Message_stMsgHeadInit(&stMsgHead,INFO_SERVICE_HEAD);
    /////////////////////////////////////
    MessageSaveDataToFlash((unsigned char *)&stMsgHead,INFO_SERVICE_START_ADDR,STMESSAGEHEAD_SIZE);
    ///////////////////////////////
}
/*************************************************************
** ��������: MessageReadAndCheckInfoService
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void MessageReadAndCheckInfoService(void) //
{
    STMESSAGEHEAD stMsgHead;
    /////////////////////////////////
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,INFO_SERVICE_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if((stMsgHead.HeadData != INFO_SERVICE_HEAD)|| (stMsgHead.saveFlag != MSG_SAVE)||(stMsgHead.currentAddr>INFO_SERVICE_DATALEN)||(stMsgHead.recordCnt*ONE_INFO_SERVICE_DATA_LEN+STMESSAGEHEAD_SIZE!= stMsgHead.currentAddr))
    {
        InfoServiceDeleteAll();
    }    
}

/*************************************************************
** ��������: QuestionResponseDeleteAll
** ��������: ɾ����������Ӧ����Ϣ
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void QuestionResponseDeleteAll(void)
{
    STMESSAGEHEAD stMsgHead;
    MessageFlashFormat(QUESTION_RESPONSE_START_ADDR);
    ////////////////////////////////////
    Message_stMsgHeadInit(&stMsgHead,QUESTION_HEAD);
    /////////////////////////////////////
    MessageSaveDataToFlash((unsigned char *)&stMsgHead,QUESTION_RESPONSE_START_ADDR,STMESSAGEHEAD_SIZE);
    ///////////////////////////////
}
/*************************************************************
** ��������: MessageReadAndCheckInfoService
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void MessageReadAndCheckQuestionResponse(void) //
{
    STMESSAGEHEAD stMsgHead;
    /////////////////////////////////
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,QUESTION_RESPONSE_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if((stMsgHead.HeadData != QUESTION_HEAD)|| (stMsgHead.saveFlag != MSG_SAVE)||(stMsgHead.currentAddr>QUESTION_RESPONSE_DATALEN)||(stMsgHead.recordCnt*ONE_QUESTION_RESPONSE_DATA_LEN+STMESSAGEHEAD_SIZE!= stMsgHead.currentAddr))
    {
        QuestionResponseDeleteAll();
    }    
}
/*************************************************************
** ��������: MessageFlashReadAndCheck()
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void MessageFlashReadAndCheck(void) //
{
    ////////////////////////////////////
    MessageReadAndCheckPhoneBook();
    /////////////////////////////////
    MessageReadAndCheckEventReport();
    //////////////////////////////////
    MessageReadAndCheckInfoDemand();
    //////////////////////////
    MessageReadAndCheckDriverInfo();  
    ////////////////////////////////////
    MessageReadAndCheckInfoService();
    //////////////////////////////////
    MessageReadAndCheckQuestionResponse();
}
/*************************************************************
** 
**     Public  code 
** 
**************************************************************/
/*************************************************************
** ��������: CheckPhoneInformation
** ��������: ���绰����
** ��ڲ���: phone����Ҫ���ĵ绰�����׵�ַ�� 
** ���ڲ���: name�����ص���ϵ������
** ���ز���: 0��û�иõ绰��������ϣ�1: ���룻2: ������3������/����
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char CheckPhoneInformation(unsigned char *number,unsigned char *name)
{
    unsigned char i;
    unsigned short addr;
    STMSG_PHONEBOOK stPhone;
    STMESSAGEHEAD  stMsgHead; 
    if(!strlen((char *)number))return 0;
    PhoneBookReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr = STMESSAGEHEAD_SIZE;
    for(i = 0; i < stMsgHead.recordCnt; i++)
    {
        memcpy(&stPhone,&s_ucMsgFlashBuffer[addr],STPHONEBOOK_SIZE);
        if(!strcmp((char *)stPhone.number,(char *)number))
        {
            strcpy((char *)name,(char *)stPhone.name);
            return stPhone.callflag;
        }
        addr += STPHONEBOOK_SIZE;
    }
    
    return 0;
}

/*************************************************************
** ��������: GetPhoneBook
** ��������: ȡ�õ绰��
** ��ڲ���: callflag���绰�����Ա�־��0���У�1: ���룻2: ������3������/����
** ���ڲ���: buffer��������Ӧ���Ա�־�ĵ绰��(��ʽ��Э�顶�绰����ʽ��)
** ���ز���: ʵ�ʳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short GetPhoneBook(unsigned char callflag,unsigned char *buffer)
{
    unsigned char i,length,num;
    unsigned short index,addr;
    STMSG_PHONEBOOK stPhone;
    STMESSAGEHEAD  stMsgHead; 
    PhoneBookReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr=STMESSAGEHEAD_SIZE;
    ///////////////////////
    index = 1;
    ////////////////////
    num =0;
    for(i = 0; i < stMsgHead.recordCnt; i++)
    {
        //////////////////////////////
        memcpy(&stPhone,&s_ucMsgFlashBuffer[addr],STPHONEBOOK_SIZE);
        ///////////////////////////////
        if((stPhone.callflag == callflag)||!callflag)
        {
            num ++;
            buffer[index++] = stPhone.callflag;
            /////////////////////
            length =strlen((char *)stPhone.number);
            buffer[index++] = length;
            memcpy(&buffer[index],&stPhone.number,length);
            index +=length;
            ///////////////////////
            length =strlen((char *)stPhone.name);
            buffer[index++] = length;
            memcpy(&buffer[index],&stPhone.name,length);
            index +=length;
        }
        addr +=STPHONEBOOK_SIZE;
    }
    ///////////////
    buffer[0] = num;
    /////////////////
    return index;
}
/*************************************************************
** ��������: GetEventReport
** ��������: ȡ���¼������б�
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: ʵ�ʳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short GetEventReport(unsigned char *buffer)
{
    unsigned char i,length;
    unsigned short index,addr;
    STMESSAGE stEvent;
    STMESSAGEHEAD  stMsgHead; 
    EventReportReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr=STMESSAGEHEAD_SIZE;
    ///////////////////////
    index = 0;    
    buffer[index++] = stMsgHead.recordCnt;
    ////////////////////
    for(i = 0; i < stMsgHead.recordCnt; i++)
    {
        //////////////////////////////
        memcpy(&stEvent,&s_ucMsgFlashBuffer[addr],STMESSAGE_SIZE);
        ///////////////////////////////
        buffer[index++] = stEvent.ID;
        /////////////////////
        length =strlen((char *)stEvent.content);
        buffer[index++] = length;
        memcpy(&buffer[index],&stEvent.content,length);
        index +=length;
        ///////////////////////
        addr +=STMESSAGE_SIZE;
    }
    ///////////////
    /////////////////
    return index;
}
/*************************************************************
** ��������: GetInfoDemand
** ��������: ȡ����Ϣ�㲥�б�
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: ʵ�ʳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short GetInfoDemand(unsigned char *buffer)
{
    unsigned char i,length;
    unsigned short index,addr;
    STMESSAGE stInfo;
    STMESSAGEHEAD  stMsgHead; 
    InfoDemandReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr=STMESSAGEHEAD_SIZE;
    ///////////////////////
    index = 0;
    buffer[index++] = stMsgHead.recordCnt;    
    ////////////////////
    for(i = 0; i < stMsgHead.recordCnt; i++)
    {
        //////////////////////////////
        memcpy(&stInfo,&s_ucMsgFlashBuffer[addr],STMESSAGE_SIZE);
        ///////////////////////////////
        buffer[index++] = stInfo.ID;
        /////////////////////
        length =strlen((char *)stInfo.content);
        buffer[index++] = length;
        memcpy(&buffer[index],&stInfo.content,length);
        index +=length;
        buffer[index++] = stInfo.status;
        ///////////////////////
        addr +=STMESSAGE_SIZE;
    }
    ///////////////
    /////////////////
    return index;
}

/*************************************************************
** ��������: InfoDemandChangeStatus
** ��������: ������Ϣ�㲥״̬
** ��ڲ���: ID:��Ϣ�㲥ID,status�޸ĵ�״̬
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoDemandChangeStatus(unsigned char ID,unsigned char status)
{
    unsigned short addr;
    STMESSAGEHEAD  stMsgHead;
    InfoDemandReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr=GetAddressFromInfoDemand(ID,stMsgHead.recordCnt);
    if(addr)//�ҵ���Ϣ�㲥
    {
        addr += STMESSAGE_SIZE;
        s_ucMsgFlashBuffer[addr-1] = status;
        /////////////////////////////
    }
    /////////////////////////////////
    /////////////////////////////////
    InfoDemandSaveDataToFlash();
}
/*************************************************************
** ��������: GetDriverInfoAll
** ��������: ȡ����Ӧ��¼�ŵļ�ʻԱ��Ϣ
** ��ڲ���: recordNumber��¼��,buffer�������ݵ��׵�ַ,flag��־:0:��ʾ��ϸ��Ϣ(�����ϱ��Ľṹ),1:��ʾ��ϸ��Ϣ(�ַ�����ʽ,��ʾ����ʾ��),2:��ʾ��ʻԱ��������ʻԱ����
** ���ڲ���: ��
** ���ز���: �������ݵĳ���,Ϊ0��ʾû����Ӧ��¼�ŵļ�ʻԱ��Ϣ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short GetDriverInfoAll(unsigned char *buffer,unsigned char flag)
{
    unsigned char i,temp;
    unsigned short addr,len,index;
    STMESSAGEHEAD  stMsgHead;
    STDRIVERINFO stDriver;
    DriverInfoReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    ////////////////////////////
    len = 0;
    addr = STMESSAGEHEAD_SIZE;
    if(flag ==2||flag ==3)
    {
       buffer[len++] = stMsgHead.recordCnt;
    }
    /////////////////
    if(stMsgHead.recordCnt==0)return len;
    ///////////////////////
    for(i=0; i<stMsgHead.recordCnt;i++)
    {
        memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);    
        ///////////////////////    
        if(flag ==0)////�ϱ�ƽ̨�����ݽṹ//////////////////////
        {
            temp=stDriver.datalen;        
            memcpy(&buffer[len],stDriver.data,temp);
            len+=temp;
        }
        else
        if(flag ==1)//�ַ���,��ʾ��////////////////////////
        {
            memcpy(&buffer[len],stDriver.drivercode,6);
            len+=6;
            index =0;
            //��������//////// 
            temp=stDriver.data[index++];
            //����//////// 
            memcpy(&buffer[len],&stDriver.data[index],temp);       
            index += temp;
            len += temp;
            buffer[len++]=',';
            ////��ʻԱ���֤���� 20//////////////////// 
            temp=strlen((char *)&stDriver.data[index]);
            if(temp>20)temp = 20;        
            memcpy(&buffer[len],&stDriver.data[index],temp);  
            index += 20;
            len += temp;
            buffer[len++]=',';
            /////////��ҵ�ʸ�֤���� 40/////////////
            temp=strlen((char *)&stDriver.data[index]);
            if(temp>40)temp = 40;        
            memcpy(&buffer[len],&stDriver.data[index],temp);   
            index += 40;
            len += temp;
            buffer[len++]=',';        
            /////��֤�������Ƴ���//////////////////
            temp=stDriver.data[index++];
            ////��֤��������///////////
            memcpy(&buffer[len],&stDriver.data[index],temp);       
            //index += temp;
            len += temp;
        }
        else
        if(flag ==2)////��ʾ��ʻԱ���롢��ʻԱ����/////////////////////
        {
            memcpy(&buffer[len],stDriver.drivercode,6);
            len += 6;
            temp=stDriver.data[0];
            temp++;
            memcpy(&buffer[len],&stDriver.data[0],temp);
            len += temp;
        }
        else
        if(3 == flag)//////��ʾ��ʻԱ���롢��ʻԱ����,�ϱ�ƽ̨�����ݽṹ/////////////////////
        {
            memcpy(&buffer[len],stDriver.drivercode,6);
            len += 6;
            memcpy(&buffer[len],stDriver.password,6);
            len += 6;
            temp = stDriver.datalen;
            memcpy(&buffer[len],stDriver.data,temp);
            len+=temp;
        }
        addr += STDRIVERINFO_SIZE;
    }
    //////////////////////////
    return len;
}
/*************************************************************
** ��������: GetDriverInfo
** ��������: ȡ����Ӧ��¼�ŵļ�ʻԱ��Ϣ
** ��ڲ���: recordNumber��¼��,buffer�������ݵ��׵�ַ,flag��־:0:��ʾ��ϸ��Ϣ(�����ϱ��Ľṹ),1:��ʾ��ϸ��Ϣ(�ַ�����ʽ,��ʾ����ʾ��),2:��ʾ��ʻԱ��������ʻԱ����
** ���ڲ���: ��
** ���ز���: �������ݵĳ���,Ϊ0��ʾû����Ӧ��¼�ŵļ�ʻԱ��Ϣ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short GetDriverInfo(unsigned char recordNumber,unsigned char *buffer,unsigned char flag)
{
    unsigned char number,temp;
    unsigned short index,len,addr;
    STMESSAGEHEAD  stMsgHead;
    STDRIVERINFO stDriver;
    if(0 == recordNumber)return 0;
    DriverInfoReadDataFromFlash();
    number =recordNumber;
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(stMsgHead.recordCnt<number)return 0;
    addr = (number-1)*STDRIVERINFO_SIZE;
    addr += STMESSAGEHEAD_SIZE;
    memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);    
    ///////////////////////
    len = 0;
    if(flag ==0)////�ϱ�ƽ̨�����ݽṹ//////////////////////
    {
        temp=stDriver.datalen;        
        memcpy(&buffer[len],stDriver.data,temp);
        len+=temp;
    }
    else
    if(flag ==1)//�ַ���,��ʾ��////////////////////////
    {
        memcpy(&buffer[len],stDriver.drivercode,6);
        len+=6;
        index =0;
        //��������//////// 
        temp=stDriver.data[index++];
        //����//////// 
        memcpy(&buffer[len],&stDriver.data[index],temp);       
        index += temp;
        len += temp;
        buffer[len++]=',';
        ////��ʻԱ���֤���� 20//////////////////// 
        temp=strlen((char *)&stDriver.data[index]);
        if(temp>20)temp = 20;        
        memcpy(&buffer[len],&stDriver.data[index],temp);  
        index += 20;
        len += temp;
        buffer[len++]=',';
        /////////��ҵ�ʸ�֤���� 40/////////////
        temp=strlen((char *)&stDriver.data[index]);
        if(temp>40)temp = 40;        
        memcpy(&buffer[len],&stDriver.data[index],temp);   
        index += 40;
        len += temp;
        buffer[len++]=',';        
        /////��֤�������Ƴ���//////////////////
        temp=stDriver.data[index++];
        ////��֤��������///////////
        memcpy(&buffer[len],&stDriver.data[index],temp);       
        //index += temp;
        len += temp;
    }
    else
    if(flag ==2)////��ʾ��ʻԱ���롢��ʻԱ����/////////////////////
    {
        memcpy(&buffer[len],stDriver.drivercode,6);
        len += 6;
        temp=stDriver.data[0];
        temp++;
        memcpy(&buffer[len],&stDriver.data[0],temp);
        len += temp;
    }
    else
    if(3 == flag)////��ʾ��ʻԱ���롢��ʻԱ����,�ϱ�ƽ̨�����ݽṹ/////////////////////
    {
        memcpy(&buffer[len],stDriver.drivercode,6);
        len += 6;
        memcpy(&buffer[len],stDriver.password,6);
        len += 6;
        temp=stDriver.datalen;
        memcpy(&buffer[len],stDriver.data,temp);
        len+=temp;
    }
    else
    if(4 == flag)////��ʾ�����ϱ�ƽ̨�����ݽṹ/////////////////////
    {
        len = 0;
        index =0;
        //��������////////
        temp=stDriver.data[index++];
        buffer[len++]=temp;
        //����//////// 
        memcpy(&buffer[len],&stDriver.data[index],temp);       
        index += temp;
        len += temp;
        ////��ʻԱ���֤���� 20//////////////////// 
        temp=20;      
        memcpy(&buffer[len],&stDriver.data[index],temp);  
        index += temp;
        len += temp;
        ////////////
        index += 40;
        /////��֤�������Ƴ���//////////////////
        temp=stDriver.data[index++];
        buffer[len++]=temp;
        ////��֤��������///////////
        memcpy(&buffer[len],&stDriver.data[index],temp);
        len += temp;
        buffer[len++]=0x20;
        buffer[len++]=0x99;
        buffer[len++]=0x12;
        buffer[len++]=0x31;
    }
    //////////////////////////
    return len;
}
/*************************************************************
** ��������: SaveCurrentDriverParameter
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void SaveCurrentDriverParameter(unsigned char recordnumber)
{
    unsigned long val;
    unsigned char number,buffer[10];
    unsigned short addr;
    STMESSAGEHEAD  stMsgHead;
    STDRIVERINFO stDriver;
    if(0 == recordnumber)return;
    DriverInfoReadDataFromFlash();
    number =recordnumber;
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(stMsgHead.recordCnt<number)return;
    addr = (number-1)*STDRIVERINFO_SIZE;
    addr += STMESSAGEHEAD_SIZE;
    memcpy(&stDriver,&s_ucMsgFlashBuffer[addr],STDRIVERINFO_SIZE);
    memcpy(buffer,&stDriver.drivercode,6);
    buffer[6]='\0';
    val=atol((char *)buffer);
    Public_ConvertLongToBuffer(val,buffer);
    //д��ʻԱ���뵽eeprom
    EepromPram_WritePram(E2_CURRENT_DRIVER_ID, &buffer[1], 3);
    //////////////////////////
    number=stDriver.data[0];
    number++;
    EepromPram_WritePram(E2_CURRENT_LICENSE_ID, &stDriver.data[number], 18);    
}
/*************************************************************
** ��������: InfoService_SaveOneData
** ��������: ����һ����Ϣ��������
** ��ڲ���: pBuffer:�����׵�ַ,datalen���ݳ���
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void InfoService_SaveOneData(unsigned char *pBuffer,unsigned short datalen)
{
    STMESSAGEHEAD  stMsgHead;
    InfoServiceReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    ////////////////////////////
    if(stMsgHead.recordCnt<INFO_SERVICE_MAX_COUNT)
    {
        stMsgHead.curindex=stMsgHead.recordCnt;
        stMsgHead.recordCnt++;
        stMsgHead.oldestindex =0;
    }
    else
    {
        stMsgHead.curindex++;
        if(stMsgHead.curindex>=INFO_SERVICE_MAX_COUNT)
        {
            stMsgHead.curindex = 0;
        }
        //////////////////
        stMsgHead.oldestindex = stMsgHead.curindex+1;
        if(stMsgHead.oldestindex>=INFO_SERVICE_MAX_COUNT)
        {
            stMsgHead.oldestindex =0;
        }
    }
    //////////////////
    stMsgHead.currentAddr=stMsgHead.curindex*ONE_INFO_SERVICE_DATA_LEN;
    stMsgHead.currentAddr +=STMESSAGEHEAD_SIZE;
    ///////////////////////////////////////
    if(datalen>ONE_INFO_SERVICE_DATA_LEN-2)datalen = ONE_INFO_SERVICE_DATA_LEN-2;
    /////////////////////////////////////
    Public_ConvertShortToBuffer(datalen,&s_ucMsgFlashBuffer[stMsgHead.currentAddr]);
    memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr+2],pBuffer,datalen);
    ////////////////////////////////////////////////////
    stMsgHead.HeadData = INFO_SERVICE_HEAD;
    stMsgHead.saveFlag = MSG_SAVE;
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    InfoServiceSaveDataToFlash();
}
/*************************************************************
** ��������: InfoService_ReadOneData
** ��������: ��ȡָ����ŵ�һ����Ϣ��������
** ��ڲ���: index��Ŵ�1��ʼ(���µ���),pBuffer:�����׵�ַ,
** ���ڲ���: ��
** ���ز���: ���ݳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short InfoService_ReadOneData(unsigned char index,unsigned char *pBuffer)
{
    unsigned char i;
    unsigned short addr;
    unsigned short datalen;
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,INFO_SERVICE_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(0==stMsgHead.recordCnt||index>stMsgHead.recordCnt)return 0;
    i = (stMsgHead.recordCnt+stMsgHead.oldestindex-index)%stMsgHead.recordCnt;
    addr = i*ONE_INFO_SERVICE_DATA_LEN;
    addr +=STMESSAGEHEAD_SIZE;
    InfoServiceReadDataFromFlash();
    datalen=Public_ConvertBufferToShort(&s_ucMsgFlashBuffer[addr]);
    if(datalen>ONE_INFO_SERVICE_DATA_LEN-2)
    {
        return 0;
    }
    /////////////////////////////////
    memcpy(pBuffer,&s_ucMsgFlashBuffer[addr+2],datalen);
    return datalen;
}
/*************************************************************
** ��������: InfoService_GetTotalNum
** ��������: ȡ����Ϣ����������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char InfoService_GetTotalNum(void)
{
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,INFO_SERVICE_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(INFO_SERVICE_HEAD==stMsgHead.HeadData&&MSG_SAVE==stMsgHead.saveFlag)
    {
        return stMsgHead.recordCnt;
    }
    ///////////////////////
    return 0;    
}
/*************************************************************
** ��������: QuestionResponse_SaveOneData
** ��������: ����һ������Ӧ������
** ��ڲ���: pBuffer:�����׵�ַ,datalen���ݳ���
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void QuestionResponse_SaveOneData(unsigned char *pBuffer,unsigned short datalen)
{
    STMESSAGEHEAD  stMsgHead;
    QuestionResponseReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    ////////////////////////////
    if(stMsgHead.recordCnt<QUESTION_RESPONSE_MAX_COUNT)
    {
        stMsgHead.curindex=stMsgHead.recordCnt;
        stMsgHead.recordCnt++;
        stMsgHead.oldestindex =0;
    }
    else
    {
        stMsgHead.curindex++;
        if(stMsgHead.curindex>=QUESTION_RESPONSE_MAX_COUNT)
        {
            stMsgHead.curindex = 0;
        }
        //////////////////
        stMsgHead.oldestindex = stMsgHead.curindex+1;
        if(stMsgHead.oldestindex>=QUESTION_RESPONSE_MAX_COUNT)
        {
            stMsgHead.oldestindex =0;
        }
    }
    //////////////////
    stMsgHead.currentAddr = stMsgHead.curindex*ONE_QUESTION_RESPONSE_DATA_LEN;
    stMsgHead.currentAddr += STMESSAGEHEAD_SIZE;
    ///////////////////////////////////////
    if(datalen>ONE_QUESTION_RESPONSE_DATA_LEN-2)datalen = ONE_QUESTION_RESPONSE_DATA_LEN-2;
    /////////////////////////////////////
    Public_ConvertShortToBuffer(datalen,&s_ucMsgFlashBuffer[stMsgHead.currentAddr]);
    memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr+2],pBuffer,datalen);
    ////////////////////////////////////////////////////
    stMsgHead.HeadData = QUESTION_HEAD;
    stMsgHead.saveFlag = MSG_SAVE;
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    QuestionResponseSaveDataToFlash();
}
/*************************************************************
** ��������: QuestionResponse_ReadOneData
** ��������: ��ȡָ����ŵ�һ������Ӧ������
** ��ڲ���: index��Ŵ�1��ʼ(���µ���),pBuffer:�����׵�ַ,
** ���ڲ���: ��
** ���ز���: ���ݳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short QuestionResponse_ReadOneData(unsigned char index,unsigned char *pBuffer)
{
    unsigned char i;
    unsigned short addr;
    unsigned short datalen;
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,QUESTION_RESPONSE_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(0==stMsgHead.recordCnt||index>stMsgHead.recordCnt)return 0;
    i = (stMsgHead.recordCnt+stMsgHead.oldestindex-index)%stMsgHead.recordCnt;
    addr = i*ONE_QUESTION_RESPONSE_DATA_LEN;
    addr +=STMESSAGEHEAD_SIZE;
    QuestionResponseReadDataFromFlash();
    datalen=Public_ConvertBufferToShort(&s_ucMsgFlashBuffer[addr]);
    if(datalen>ONE_QUESTION_RESPONSE_DATA_LEN-2)
    {
        return 0;
    }
    /////////////////////////////////
    memcpy(pBuffer,&s_ucMsgFlashBuffer[addr+2],datalen);
    return datalen;
}
/*************************************************************
** ��������: QuestionResponse_GetTotalNum
** ��������: ȡ������Ӧ��������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char QuestionResponse_GetTotalNum(void)
{
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,QUESTION_RESPONSE_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(QUESTION_HEAD==stMsgHead.HeadData&&MSG_SAVE==stMsgHead.saveFlag)
    {
        return stMsgHead.recordCnt;
    }
    ///////////////////////
    return 0;
}
/*************************************************************
** ��������: EventReport_DisposeRadioProtocol
** ��������: �¼��������Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���
** ���ڲ���: ��
** ���ز���: ���ز������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char EventReport_DisposeRadioProtocol(unsigned char *pBuffer,unsigned short BufferLen)
{
    unsigned char SetType;
    SetType = *pBuffer++;
    switch(SetType)
    {
        case SETTYPE_DELETE:
            EventReportDeleteAll();
            break;
        case SETTYPE_UPDATE:
            EventReportUpdate(pBuffer,BufferLen-1);
            break;
        case SETTYPE_ADD:
            EventReportAdd(pBuffer,BufferLen-1);
            break;
        case SETTYPE_MODIFY:
            EventReportModify(pBuffer,BufferLen-1);
            break;
        case SETTYPE_DEL_EX:
            return EventReportDelEx(pBuffer,BufferLen-1);
            //break;
        default:
            return 3;
    }
    return 0;
}
/*************************************************************
** ��������: InfoDemand_DisposeRadioProtocol
** ��������: ��Ϣ�㲥����Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���
** ���ڲ���: ��
** ���ز���: ���ز������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char InfoDemand_DisposeRadioProtocol(unsigned char *pBuffer,unsigned short BufferLen)
{
    unsigned char SetType;
    SetType = *pBuffer++;
    switch(SetType)
    {
        case SETTYPE_DELETE:
            InfoDemandDeleteAll();
            break;
        case SETTYPE_UPDATE:
            InfoDemandUpdate(pBuffer,BufferLen-1);
            break;
        case SETTYPE_ADD:
            InfoDemandAdd(pBuffer,BufferLen-1);
            break;
        case SETTYPE_MODIFY:
            InfoDemandModify(pBuffer,BufferLen-1);
            break;
        default:
            return 3;
    }
    return 0;
}
/*************************************************************
** ��������: PhoneBook_DisposeRadioProtocol
** ��������: �绰������Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���
** ���ڲ���: ��
** ���ز���: ���ز������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char PhoneBook_DisposeRadioProtocol(unsigned char *pBuffer,unsigned short BufferLen)
{
    unsigned char SetType;
    SetType = *pBuffer++;
    switch(SetType)
    {
        case SETTYPE_DELETE:
            PhoneBookDeleteAll();
            break;
        case SETTYPE_UPDATE:
            PhoneBookUpdate(pBuffer,BufferLen-1);
            break;
        case SETTYPE_ADD:
            PhoneBookAdd(pBuffer,BufferLen-1);
            break;
        case SETTYPE_MODIFY:
            PhoneBookModify(pBuffer,BufferLen-1);
            break;
        default:
            return 3;
    }
    return 0;
}
/*************************************************************
** ��������: Message_GetPhoneBookTotalNum
** ��������: ȡ�õ绰��������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Message_GetPhoneBookTotalNum(void)
{
    unsigned char buffer[STMESSAGEHEAD_SIZE];
    STMESSAGEHEAD  stMsgHead;
    MessageReadDataFromFlash(buffer,PHONEBOOK_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,buffer,STMESSAGEHEAD_SIZE);
    if(PHONE_HEAD==stMsgHead.HeadData&&MSG_SAVE==stMsgHead.saveFlag)
    {
        return stMsgHead.recordCnt;
    }
    ///////////////////////
    return 0;    
}
/*************************************************************
** ��������: Message_GetOnePhoneBookContent
** ��������: ȡ��ĳ���绰������
** ��ڲ���: index������,stPhoneBook�ṹ��ָ��
** ���ڲ���: ��
** ���ز���: 0,��ȡʧ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Message_GetOnePhoneBookContent(unsigned char index,STMSG_PHONEBOOK *stPhoneBook)
{
    unsigned short addr;
    STMESSAGEHEAD  stMsgHead;
    if(0==index)return 0;
    MessageReadDataFromFlash(s_ucMsgFlashBuffer,PHONEBOOK_START_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(PHONE_HEAD!=stMsgHead.HeadData||MSG_SAVE!=stMsgHead.saveFlag)return 0;
    if(index>stMsgHead.recordCnt)return 0;
    PhoneBookReadDataFromFlash();
    addr = (index-1)*STPHONEBOOK_SIZE;
    addr +=STMESSAGEHEAD_SIZE;
    /////////////////////////////////
    memcpy(stPhoneBook,&s_ucMsgFlashBuffer[addr],STPHONEBOOK_SIZE);
    return STPHONEBOOK_SIZE;
}
/*************************************************************
** ��������: DriverInfo_DisposeRadioProtocol
** ��������: ��ʻԱ���Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���
** ���ڲ���: ��
** ���ز���: ���ز������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char DriverInfo_DisposeRadioProtocol(unsigned char *pBuffer,unsigned short BufferLen)
{
    unsigned char buffer[10];
    unsigned char temp,i;
    unsigned short index,addr,len; 
    index =0;
    i =1;
    //////////////
    DriverInfoDeleteAll();  
    ////////////////////
    while(index<BufferLen)
    {
        addr =index;
        /////////////////////
        temp=pBuffer[index++];
        if(temp>20)return 1;//����10������
        index +=temp;
        ///////////////
        index +=20;
        index +=40;
        if(index>BufferLen)return 2;//����
        temp=pBuffer[index++];
        if(temp>80)return 3;//����40������
        index +=temp;
        if(index>BufferLen)return 2;//����
        //////////////////////////
        len=index-addr;
        if(len>ONE_DRIVER_INFO_MAX_LEN)return 4;//�����ܳ���
        sprintf((char *)buffer,"%06d",i);
        DriverInfoAdd(&temp,buffer,"000000",&pBuffer[addr],len);
        i++;
    };
    return 0;
}
/*************************************************************
** ��������: DriverInfo_ReadDataForRadioProtocol
** ��������: ��ȡ��ʻԱ����Э�����
** ��ڲ���: pBuffer:�����׵�ַ,
** ���ڲ���: ��
** ���ز���: ���ݳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short DriverInfo_ReadDataForRadioProtocol(unsigned char *pBuffer)
{
    return GetDriverInfoAll(pBuffer,0);
}
/*************************************************************
** ��������: DriverInfo_GetOneDatalen
** ��������: ȡ��һ����ʻԱ��Ϣ��ĳ���
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���
** ���ڲ���: ��
** ���ز���: ���ز������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Messege_GetOneDriverInfoDatalen(unsigned char *pBuffer,unsigned short BufferLen)
{
    unsigned char temp;
    unsigned char len;
    if(BufferLen<62||BufferLen>ONE_DRIVER_INFO_MAX_LEN)return 0;//
    len = 0;
    temp=pBuffer[len++];//��ʻԱ��������
    if(temp>20)return 0;//
    len +=temp;////��ʻԱ����
    ///////////////
    len +=20;///��ʻԱ���֤����
    len +=40;
    if(len+1>BufferLen)return 0;//
    temp=pBuffer[len++];//��֤�������ݳ���
    if(temp>80)return 0;//
    len +=temp;    
    ////////////////////
    if(len>BufferLen)return 0;
    //////////////////////////
    return len;
}
/*************************************************************
** ��������: DriverInfo_PlatformReadData
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Messege_PlatformReadDriverInfoData(void)
{
    unsigned char buffer[1024];
    unsigned short len;
    buffer[0] = 5;//������
    buffer[1] = 0;//�����ֽ�
    len=GetDriverInfoAll(&buffer[2],3);
    len +=2;
    EIExpand_PotocolSendData(EIEXPAND_CMD_SET_DRIVER_INFO,buffer,len);
}
/*************************************************************
** ��������: Messege_CheckDriverInfodatalen
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Messege_CheckDriverInfodatalen(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char *pTmp;
    unsigned short datalen;
    unsigned char sum,i,templen;   
    sum = *pBuffer++;
    datalen = BufferLen-1;
    pTmp =pBuffer;
    if(sum)
    {
        for(i= 0; i<sum; i++) 
        {
            templen = 12;//
            pTmp += templen;
            if(datalen < templen)return 0;
            templen = Messege_GetOneDriverInfoDatalen(pTmp,datalen);
            if(0==templen)return 0;
            if(datalen>templen)
            {
                pTmp += templen;
                datalen -=templen;
            }
            else
            if(datalen==templen)//
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    return 1;
}
/*************************************************************
** ��������: Messege_DriverInfoUpdate
** ��������: 
** ��ڲ���: pBuffer:��Ϣ�������׵�ַ,BufferLen��Ϣ�����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Messege_DriverInfoUpdate(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char flag;
    unsigned char *p;    
    unsigned char i,sum;
    STDRIVERINFO stDriver;
    STMESSAGEHEAD stMsgHead;
    unsigned short len,datalen;
    p = pBuffer;
    flag = Messege_CheckDriverInfodatalen(p,BufferLen);
    if(0==flag)return 0;    
    //////////////
    ////////////////
    p = pBuffer;
    sum = *p++;
    datalen = BufferLen - 1; 
    Message_stMsgHeadInit(&stMsgHead,DRIVER_HEAD);
    memset(s_ucMsgFlashBuffer,0,sizeof(s_ucMsgFlashBuffer));
    ////////////////////
    for(i= 0; i<sum; i++) 
    {
        memcpy(stDriver.drivercode,p,6);
        p += 6;
        memcpy(stDriver.password,p,6);
        p += 6;
        datalen -= 12;
        len = Messege_GetOneDriverInfoDatalen(p,datalen);
        memcpy(stDriver.data,p,len);
        stDriver.datalen=len;
        p += len;
        if(stMsgHead.currentAddr +STDRIVERINFO_SIZE>DRIVER_INFO_DATALEN)break;
        memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stDriver,STDRIVERINFO_SIZE);
        ////////////////////
        stMsgHead.currentAddr += STDRIVERINFO_SIZE;
        stMsgHead.recordCnt++;
        if(datalen>len)datalen -= len;
        else break;
    }
    /////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    DriverInfoSaveDataToFlash(); 
    ////////////////
    return 1;    
}
/*************************************************************
** ��������: Messege_DriverInfoAdd
** ��������: 
** ��ڲ���: pBuffer:��Ϣ�������׵�ַ,BufferLen��Ϣ�����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Messege_DriverInfoAdd(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char *p,*pTmp;    
    unsigned char i,sum;
    unsigned short len,datalen;
    STDRIVERINFO stDriver;
    STMESSAGEHEAD stMsgHead;
    p = pBuffer;
    sum = *p++;
    if(0 == sum)return 0;
    ////////////////////
    pTmp = p;
    datalen = BufferLen - 1;
    for(i= 0; i<sum; i++) 
    {
        if(GetDriverInfoRecordNumber(pTmp))return 0;
        datalen -= 12;
        pTmp +=12;
        len = Messege_GetOneDriverInfoDatalen(pTmp,datalen);
        if(0 ==len)return 0;
        if(GetDriverInfoRecordNumberForCheckData(pTmp))return 0;
        pTmp +=len;
        if(datalen>len)datalen -= len;
        else break;
    }
    //////////////////////////////////
    pTmp = p;
    datalen = BufferLen - 1;
    ///////////////////////////////////
    DriverInfoReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    for(i= 0; i<sum; i++) 
    {
        memcpy(stDriver.drivercode,pTmp,6);
        pTmp += 6;
        memcpy(stDriver.password,pTmp,6);
        pTmp += 6;
        datalen -= 12;
        len = Messege_GetOneDriverInfoDatalen(pTmp,datalen);
        stDriver.datalen = len;
        memcpy(stDriver.data,pTmp,len);        
        if(stMsgHead.currentAddr +STDRIVERINFO_SIZE>DRIVER_INFO_DATALEN)break;
        memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&stDriver,STDRIVERINFO_SIZE);
        ////////////////////
        pTmp += len;
        stMsgHead.currentAddr += STDRIVERINFO_SIZE;
        stMsgHead.recordCnt++;
        if(datalen>len)datalen -= len;
        else break;
    }
    /////////////////////
    stMsgHead.HeadData = DRIVER_HEAD;
    stMsgHead.saveFlag = MSG_SAVE;
    /////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    DriverInfoSaveDataToFlash(); 
    return 1;    
}
/*************************************************************
** ��������: Messege_DriverInfoModify
** ��������: 
** ��ڲ���: pBuffer:��Ϣ�������׵�ַ,BufferLen��Ϣ�����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Messege_DriverInfoModify(unsigned char *pBuffer, unsigned short BufferLen)
{
    STDRIVERINFO stDriver;
    STMESSAGEHEAD stMsgHead;
    unsigned char *p,*pTmp;    
    unsigned char i,sum,index1,index2;
    unsigned short len,datalen;
    unsigned char buffer[30];
    unsigned short addr;    
    p = pBuffer;
    sum = *p++;
    if(0 == sum)return 0;
    ////////////////////
    pTmp = p;
    datalen = BufferLen - 1;
    for(i= 0; i<sum; i++) 
    {
        index1=GetDriverInfoRecordNumber(pTmp);
        /////////////////////
        datalen -= 12;
        pTmp +=12;        
        len = Messege_GetOneDriverInfoDatalen(pTmp,datalen);
        if(0 ==len)return 0;
        index2 = GetDriverInfoRecordNumberForCheckData(pTmp);
        if(0==index1&&0==index2)return 0;//δ�ҵ�
        if(index1&&index2&&index1!=index2)return 0;//���ҵ�,����ͬ
        if(index1)//�ݴ�ID
        {
            buffer[i] = index1-1;
        }
        else
        {
            buffer[i] = index2-1;
        }
        pTmp +=len;
        if(datalen>len)datalen -= len;
        else break;
    }
    //////////////////////////////////
    pTmp = p;
    datalen = BufferLen - 1;
    ///////////////////////////////////
    DriverInfoReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    for(i= 0; i<sum; i++) 
    {
        memcpy(stDriver.drivercode,pTmp,6);
        pTmp += 6;
        memcpy(stDriver.password,pTmp,6);
        pTmp += 6;
        datalen -= 12;
        len = Messege_GetOneDriverInfoDatalen(pTmp,datalen);
        stDriver.datalen=len;
        memcpy(stDriver.data,pTmp,len);        
        addr = buffer[i]*STDRIVERINFO_SIZE+STMESSAGEHEAD_SIZE;
        memcpy(&s_ucMsgFlashBuffer[addr],&stDriver,STDRIVERINFO_SIZE);
        pTmp += len;
        ////////////////////
        if(datalen>len)datalen -= len;
        else break;
    }
    /////////////////////
    stMsgHead.HeadData = DRIVER_HEAD;
    stMsgHead.saveFlag = MSG_SAVE;
    /////////////////////
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    DriverInfoSaveDataToFlash(); 
    return 1;    
}
/*************************************************************
** ��������: Messege_DeleteOneDriverInfo
** ��������: ɾ����ʻԱ��������ʻԱ���֤����
** ��ڲ���: �����ϱ��ĸ�ʽ������
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Messege_DeleteOneDriverInfo(unsigned char index)
{
    unsigned short addr,len;
    STMESSAGEHEAD  stMsgHead;
    DriverInfoReadDataFromFlash();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    addr = index-1;
    addr *= STDRIVERINFO_SIZE;
    addr += STMESSAGEHEAD_SIZE;
    len = 0;
    if(index < stMsgHead.recordCnt)
    {
        len = stMsgHead.recordCnt-index;
        len *= STDRIVERINFO_SIZE;
        memmove(&s_ucMsgFlashBuffer[addr],&s_ucMsgFlashBuffer[addr+STDRIVERINFO_SIZE],len);
    }
    memset(&s_ucMsgFlashBuffer[addr+len],0,STDRIVERINFO_SIZE);
    stMsgHead.recordCnt--;
    stMsgHead.currentAddr -= STDRIVERINFO_SIZE;
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    DriverInfoSaveDataToFlash();
}
/*************************************************************
** ��������: Messege_DriverInfoUpdate
** ��������: 
** ��ڲ���: pBuffer:��Ϣ�������׵�ַ,BufferLen��Ϣ�����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Messege_DriverInfoDelete(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char *p,*pTmp;    
    unsigned char i,sum;
    unsigned short index,len,datalen;
    p = pBuffer;
    sum = *p++;
    ////////////////////
    if(sum)//��Ҫ��ɾ��ĳ��ʻԱ
    {
        pTmp = p;
        datalen = BufferLen - 1;
        for(i= 0; i<sum; i++) 
        {
            datalen -= 12;
            pTmp +=12;
            len = Messege_GetOneDriverInfoDatalen(pTmp,datalen);
            if(0 ==len)return 0;
            if(0==GetDriverInfoRecordNumberForCheckData(pTmp))return 0;
            pTmp +=len;
            if(datalen>len)datalen -= len;
            else break;
        }
        //////////////////////////////////
        pTmp = p;
        datalen = BufferLen - 1;
        ///////////////////////////////////
        for(i= 0; i<sum; i++) 
        {
            datalen -= 12;
            pTmp += 12;
            len = Messege_GetOneDriverInfoDatalen(pTmp,datalen);
            index = GetDriverInfoRecordNumberForCheckData(pTmp);
            Messege_DeleteOneDriverInfo(index);
            pTmp +=len;
        }
    }
    else//ɾ��ȫ��
    {
        DriverInfoDeleteAll();
    }
    return 1;    
}
/*************************************************************
** ��������: DriverInfo_DisposeEIExpandProtocol
** ��������: ������չЭ�顪�����ü�ʻԱ��Ϣ
** ��ڲ���: pBuffer:��Ϣ�������׵�ַ,BufferLen��Ϣ�����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Messege_DisposeEIExpandProtocolForDriverInfo(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char AckResult=0;
    unsigned char operate;
    unsigned char *p;    
    p = pBuffer;
    operate = *p++;//������־
    if(MSG_SET_DRIVER_INFO_QUERY == operate)//��ȡ
    {
        LZM_SetOperateTimer(Messege_PlatformReadDriverInfoData,2*SYSTICK_1SECOND);
        return 0;
    }
    ////////////////
    if(BufferLen<2)return 2;//
    //////////////////
    p++;//�����ֽ�  
    //////////////////
    if(MSG_SET_DRIVER_INFO_UPDATE == operate)//����
    {
        AckResult= Messege_DriverInfoUpdate(p,BufferLen-2);
    }
    else
    if(MSG_SET_DRIVER_INFO_ADD == operate)//׷��
    {
         AckResult= Messege_DriverInfoAdd(p,BufferLen-2);
    }
    else
    if(MSG_SET_DRIVER_INFO_MODIFY == operate)//�޸�
    {
        AckResult= Messege_DriverInfoModify(p,BufferLen-2);
    }
    else
    if(MSG_SET_DRIVER_INFO_DELETE == operate)//ɾ��
    {
       AckResult= Messege_DriverInfoDelete(p,BufferLen-2);
    }
    return(AckResult)?0:1;
}
/*************************************************************
** ��������: Message_ParameterInitialize
** ��������: ������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Message_ParameterInitialize(void)
{
    MessageFlashReadAndCheck();
    #ifdef __DOWNLOAD_GB2313_H
    DownloadGB2313_CheckFlashArea();
    #endif
    #ifdef USE_PHONE_BOOK
    PhoneBook_ReadAndCheckCallRecord();
    #endif
}
#endif
/******************************************************************************
**                            End Of File
******************************************************************************/
