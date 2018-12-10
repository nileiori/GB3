/*******************************************************************************
 * File Name:			PhoneBook.c 
 * Function Describe:	�绰��
 * Relate Module:		
 * Writer:				Joneming
 * Date:				2013-09-02
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
/*******************************************************************************/
////////////////////////////////////////////
#if 1

///////////////////////////
typedef enum
{
    PHONE_STATUS_MISSED_IN,
    PHONE_STATUS_TALK_IN,
    PHONE_STATUS_MISSED_OUT,
    PHONE_STATUS_TALK_OUT,
    PHONE_STATUS_MAX
}E_PHONESTATUS;
////////////////////////
typedef struct
{
    unsigned long startTime;    //ʱ��
    unsigned short duration;    //ʱ��
    unsigned char status;       //״̬ 
    unsigned char callflag;     //��־    
    char name[10];              //����
    char number[20];            //�绰����
    char bak[2];                //����
}STCALLRECORD;
static STCALLRECORD s_stCallRecord;
#define ONE_STPHONE_RECORD_SIZE sizeof(STCALLRECORD)

typedef enum 
{
    CALL_STATUS_HANG_UP,    
    CALL_STATUS_ANSWER_CALL,
    CALL_STATUS_NEW_PHONE,
    CALL_STATUS_WAIT_FOR_CALL,
    CALL_STATUS_MAX
}E_CALL_STATUS;


typedef enum 
{
    PHONE_TASK_EMPTY,    
    PHONE_TASK_NEW_PHONE,
    PHONE_TASK_HOT_LINE,
    PHONE_TASK_BOOK,
    PHONE_TASK_RECORD,
    PHONE_TASK_BACK,
    PHONE_TASK_MAX,
}E_PHONE_TASK;

typedef struct
{
    unsigned char HangUp;          //
    unsigned char control;          //
    unsigned char callstatus;       //״̬
    unsigned char specialFlag;      //
    unsigned char answermode;       //
    unsigned char preTaskFlag;       //
}STPHONEBOOK_ATTRIB; 

static STPHONEBOOK_ATTRIB s_stPhoneBook;

struct SUT_MENU s_stMenuPhone;
const char *s_apcMenuPhone[]= 
{
    "����ͷ�����",
    "�� �� ��",
    "ͨ����¼",
    "����绰",
	""
};
typedef enum 
{
    MENU_PHONE_HOT_LINE, 
    MENU_PHONE_BOOK,    
    MENU_PHONE_RECORD, 
    MENU_PHONE_MAKE,
    MENU_PHONE_MAX
}E_MENU_PHONE;


const char *c_apcPhoneAttrib[] =
{
    "",
    "����",
    "����",
    "����/����",
};

const char *c_apcCallStatus[] =
{
    "δ��",
    "�ѽ�",
    "�Ѳ�",
    "�Ѳ�",
};
/////////////////////////

#define SAVE_PHONE_RECORD_SECTOR        (FLASH_CALL_RECORD_START_SECTOR)
#define SAVE_PHONE_RECORD_ADDR          (SAVE_PHONE_RECORD_SECTOR*FLASH_ONE_SECTOR_BYTES)//
#define PHONEBOOK_RECORD_DATALEN        (unsigned long)FLASH_ONE_SECTOR_BYTES
#define PHONE_RECORD_MAX_COUNT          (unsigned char)(((PHONEBOOK_RECORD_DATALEN-STMESSAGEHEAD_SIZE)/ONE_STPHONE_RECORD_SIZE)-3)
#define PHONE_RECORD_HEAD               0xDCBA
#define PHONE_RECORD_SAVE               0xbb
/////////////////////////////////////////////
LZM_RET TaskShowNewCallPhone(LZM_MESSAGE *Msg);
LZM_RET TaskShowDialing(LZM_MESSAGE *Msg);
LZM_RET TaskShowCallTalking(LZM_MESSAGE *Msg);
LZM_RET TaskShowMakeNewDial(LZM_MESSAGE *Msg);
/*************************************************************
** ��������: PhoneBook_ReadCallRecord
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBook_ReadCallRecord(void) 
{
    sFLASH_ReadBuffer(s_ucMsgFlashBuffer,SAVE_PHONE_RECORD_ADDR,PHONEBOOK_RECORD_DATALEN);
}
/*************************************************************
** ��������: PhoneBook_SaveCallRecord
** ��������: Flash������
** ��ڲ���: s:����ָ�룬startAddr:��ʼ��ַ(��Ե�ַ);lenght:����
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBook_SaveCallRecord(void) 
{
    ///////////////��д��֮ǰ�ȸ�ʽ��//////////////////////
    sFLASH_EraseSector(SAVE_PHONE_RECORD_ADDR);
    ////////////////////////
    sFLASH_WriteBuffer(s_ucMsgFlashBuffer,SAVE_PHONE_RECORD_ADDR,PHONEBOOK_RECORD_DATALEN);
}
/*************************************************************
** ��������: PhoneBook_CallRecordDeleteAll
** ��������: ɾ������ͨ����¼����
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBook_CallRecordDeleteAll(void)
{
    STMESSAGEHEAD stMsgHead;
    sFLASH_EraseSector(SAVE_PHONE_RECORD_ADDR);
    ////////////////////////////////////
    stMsgHead.currentAddr = STMESSAGEHEAD_SIZE;
    stMsgHead.HeadData = PHONE_RECORD_HEAD;
    stMsgHead.saveFlag = PHONE_RECORD_SAVE;
    stMsgHead.recordCnt = 0;
    stMsgHead.oldestindex = 0;
    stMsgHead.curindex = 0;
    /////////////////////////////////////
    sFLASH_WriteBuffer((unsigned char *)&stMsgHead,SAVE_PHONE_RECORD_ADDR,STMESSAGEHEAD_SIZE);
    ///////////////////////////////
}
/*************************************************************
** ��������: PhoneBook_ReadAndCheckCallRecord
** ��������: ���ͨ����¼�Ƿ����
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
void PhoneBook_ReadAndCheckCallRecord(void) //
{
    STMESSAGEHEAD stMsgHead;
    /////////////////////////////////
    sFLASH_ReadBuffer(s_ucMsgFlashBuffer,SAVE_PHONE_RECORD_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if((stMsgHead.HeadData != PHONE_RECORD_HEAD)|| (stMsgHead.saveFlag != PHONE_RECORD_SAVE)||(stMsgHead.currentAddr>PHONEBOOK_RECORD_DATALEN)||(stMsgHead.currentAddr<STMESSAGEHEAD_SIZE)|| (0 !=(stMsgHead.currentAddr-STMESSAGEHEAD_SIZE)%ONE_STPHONE_RECORD_SIZE))
    {
        PhoneBook_CallRecordDeleteAll();
    }    
}
/*************************************************************
** ��������: PhoneBook_SaveOneCallRecordData
** ��������: ����һ��ͨ����¼����
** ��ڲ���: pBuffer:�����׵�ַ,datalen���ݳ���
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBook_SaveOneCallRecordData(void)
{
    STMESSAGEHEAD  stMsgHead;
    PhoneBook_ReadCallRecord();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    ////////////////////////////
    if(stMsgHead.recordCnt<PHONE_RECORD_MAX_COUNT)
    {
        stMsgHead.curindex=stMsgHead.recordCnt;
        stMsgHead.recordCnt++;
        stMsgHead.oldestindex =0;
    }
    else
    {
        stMsgHead.curindex++;
        if(stMsgHead.curindex>=PHONE_RECORD_MAX_COUNT)
        {
            stMsgHead.curindex = 0;
        }
        //////////////////
        stMsgHead.oldestindex = stMsgHead.curindex+1;
        if(stMsgHead.oldestindex>=PHONE_RECORD_MAX_COUNT)
        {
            stMsgHead.oldestindex =0;
        }
    }
    //////////////////
    stMsgHead.currentAddr=stMsgHead.curindex*ONE_STPHONE_RECORD_SIZE;
    stMsgHead.currentAddr +=STMESSAGEHEAD_SIZE;
    /////////////////////////////////////
    memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&s_stCallRecord,ONE_STPHONE_RECORD_SIZE);
    ////////////////////////////////////////////////////
    stMsgHead.HeadData = PHONE_RECORD_HEAD;
    stMsgHead.saveFlag = PHONE_RECORD_SAVE;
    memcpy(&s_ucMsgFlashBuffer[0],&stMsgHead,STMESSAGEHEAD_SIZE);
    PhoneBook_SaveCallRecord();
}
/*************************************************************
** ��������: PhoneBook_ReadOneCallRecordData
** ��������: ��ȡָ����ŵ�һ��ͨ����¼����
** ��ڲ���: index��Ŵ�1��ʼ(���µ���),pBuffer:�����׵�ַ,
** ���ڲ���: ��
** ���ز���: ���ݳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short PhoneBook_ReadOneCallRecordData(unsigned char index,STCALLRECORD *stCallRecord)
{
    unsigned char i;
    unsigned short addr;
    STMESSAGEHEAD  stMsgHead;
    sFLASH_ReadBuffer(s_ucMsgFlashBuffer,SAVE_PHONE_RECORD_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(0==stMsgHead.recordCnt||index>stMsgHead.recordCnt)return 0;
    i = (stMsgHead.recordCnt+stMsgHead.oldestindex-index)%stMsgHead.recordCnt;
    addr = i*ONE_STPHONE_RECORD_SIZE;
    addr +=STMESSAGEHEAD_SIZE;
    PhoneBook_ReadCallRecord();
    /////////////////////////////////
    memcpy(stCallRecord,&s_ucMsgFlashBuffer[addr],ONE_STPHONE_RECORD_SIZE);
    return ONE_STPHONE_RECORD_SIZE;
}
/*************************************************************
** ��������: PhoneBook_GetCallRecordTotalNum
** ��������: ȡ��ͨ����¼������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char PhoneBook_GetCallRecordTotalNum(void)
{
    STMESSAGEHEAD  stMsgHead;
    sFLASH_ReadBuffer(s_ucMsgFlashBuffer,SAVE_PHONE_RECORD_ADDR,STMESSAGEHEAD_SIZE);
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(PHONE_RECORD_HEAD==stMsgHead.HeadData&&PHONE_RECORD_SAVE==stMsgHead.saveFlag)
    {
        return stMsgHead.recordCnt;
    }
    ///////////////////////
    return 0;    
}
/*************************************************************
** ��������: PhoneBook_ModifyCurCallRecord
** ��������: �޸ĵ�ǰͨ����¼����
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void PhoneBook_ModifyCurCallRecord(void)
{
    STMESSAGEHEAD stMsgHead;
    PhoneBook_ReadCallRecord();
    memcpy(&stMsgHead,s_ucMsgFlashBuffer,STMESSAGEHEAD_SIZE);
    if(PHONE_RECORD_HEAD!=stMsgHead.HeadData||PHONE_RECORD_SAVE!=stMsgHead.saveFlag||0==stMsgHead.recordCnt)return;
    memcpy(&s_ucMsgFlashBuffer[stMsgHead.currentAddr],&s_stCallRecord,ONE_STPHONE_RECORD_SIZE);
    PhoneBook_SaveCallRecord();
}
/*******************************************************************************
** ��������: PhoneBook_HangUpCall
** ��������: �Ҷϵ绰
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void PhoneBook_HangUpCall(void)
{
    communication_CallPhone("13800000000",PHONE_END);
    if(s_stPhoneBook.HangUp==0)
    {
        s_stPhoneBook.HangUp =1;
        LZM_SetOperateTimer(PhoneBook_HangUpCall,PUBLICSECS(0.1));
    }
    else
    {
        s_stPhoneBook.HangUp =0;
    }
}
/*******************************************************************************
** ��������: PhoneBook_SetCurrentCallStatus
** ��������: ���õ�ǰͨ��״̬
** ��ڲ���:  
** ���ڲ���: ��
** ���ز���:  
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
void PhoneBook_SetCurrentCallStatus(unsigned char value)
{
    s_stPhoneBook.callstatus = value;
    switch(value)
    {
        case CALL_STATUS_HANG_UP:            
            break;        
        case CALL_STATUS_ANSWER_CALL:
            break;  
        case CALL_STATUS_NEW_PHONE:
            s_stPhoneBook.preTaskFlag = PHONE_TASK_NEW_PHONE;
            LZM_StartNewTask(TaskShowNewCallPhone);
            break;
    }
}
/*************************************************************
** ��������: PhoneBook_DisposeNewCallPhone
** ��������: ����������
** ��ڲ���: �绰����
** ���ڲ���: ��
** ���ز���: �ȴ�������־:0:ֱ�Ӿܽ�����;1:���ڵȴ��û���������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char PhoneBook_DisposeNewCallPhone(unsigned char *phone)
{
    unsigned char name[24];
    unsigned char flag; 
    //////////////////////////////
    flag=Public_CheckPhoneInformation(phone,name);//0��û�иõ绰��������ϣ�1: ���룻2: ������3������/����
    if(2 == flag||0 == flag)//�绰�����ں���,�����ں���
    {
        PhoneBook_HangUpCall();
        //ֱ�Ӿܽӵ绰/////////////
        return 0;
    }
    else//�绰�����ں���,���ڿɺ���
    if(flag)
    {
        strcpy(s_stCallRecord.name,(char *)name);
    }
    /////////////////////////////////////
    s_stCallRecord.callflag = flag;
    strcpy(s_stCallRecord.number,(char *)phone);    
    PhoneBook_SetCurrentCallStatus(CALL_STATUS_NEW_PHONE);
    return 1;
}
/*************************************************************
** ��������: Public_DisposePhoneStatus
** ��������: ����绰״̬����
** ��ڲ���: phone:�绰����,status:�绰״̬��0��Ϊ�Ҷ�;1:��ͨ;2:������;
** ���ڲ���: ��
** ���ز���: ����ǵ绰����״̬Ϊ������ʱ,����ֵ:0:ֱ�Ӿܽ�����;1:���ڵȴ��û���������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char PhoneBook_DisposePhoneStatus(unsigned char *phone,unsigned char status)
{
    unsigned char result;
    result =0;
    switch(status)
    {
        case 0://�һ�
            s_stPhoneBook.specialFlag =0;
            PhoneBook_SetCurrentCallStatus(CALL_STATUS_HANG_UP);            
            break;
        case 1://ͨ��            
            PhoneBook_SetCurrentCallStatus(CALL_STATUS_ANSWER_CALL);
            break;
        case 2://������
            if(CALL_STATUS_HANG_UP==s_stPhoneBook.callstatus)
            result=PhoneBook_DisposeNewCallPhone(phone);
            break;
    }
    return result;
}
/*************************************************************
** ��������: ShowPhoneBookIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void ShowPhoneBookIndex(unsigned char index,unsigned char count)
{
    char buffer[20];
    STMSG_PHONEBOOK stPhoneBook;
    if(0xff==index)
    {
        LcdShowStr(0, 0, "�绰��", 1);
        LcdShowStr(0, 32, "����:", 0);
        LcdShowStr(0, 48, "����:", 0);
    }
    else
    {
        LcdClearArea(0, 16, 128, 32);
        LcdClearArea(43, 32, 128, 63);
        Message_GetOnePhoneBookContent(index,&stPhoneBook);
        sprintf(buffer,"%d/%-2d",count,index);
        LcdShowStr(88,0,buffer,1);
        LcdShowStr(0,16,(char *)stPhoneBook.number,0);
        LcdShowStr(44,32,(char *)stPhoneBook.name,0);
        LcdShowStr(44,48,c_apcPhoneAttrib[stPhoneBook.callflag],0);       
    }
}
/*************************************************************
** ��������: PhoneBook_CheckPhoneCallResult
** ��������: ���绰���뼰���н��
** ��ڲ���: phone�绰����,newname����
** ���ڲ���: ��
** ���ز���: 0���гɹ�
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char PhoneBook_CheckPhoneCallResult(unsigned char *phone,unsigned char *newname)
{
    unsigned char name[20];
    unsigned char flag;
    s_stPhoneBook.specialFlag =Public_CheckPhoneIsSpecialNumber(phone);
    if(0==s_stPhoneBook.specialFlag&&Public_CheckIsPhoneServerRunStatus())
    {
        Public_ShowTextInfo("��ʻ��,��ֹ����",SECS(3));
        return 1;
    }
    flag=Public_CheckPhoneInformation((unsigned char *)phone,name);//0��û�иõ绰��������ϣ�1: ���룻2: ������3������/����
    if(1==flag||0==flag)//��ֹ����/����,1: ���룻2: ������3������/����
    {
        s_stPhoneBook.control =0x55;
        Public_ShowTextInfo("�ú����ֹ����!",SECS(3));
        return 2;
    }
    else
    {
        flag=communication_CallPhone((unsigned char *)phone,PHONE_NORMAL);
        if(flag)
        {
            PhoneBook_SetCurrentCallStatus(CALL_STATUS_MAX);
            strcpy(s_stCallRecord.number,(char *)phone);
            if(newname==NULL||(!strcmp((char *)newname,"δ֪")))
            strcpy(s_stCallRecord.name,(char *)name);
            else
            strcpy(s_stCallRecord.name,(char *)newname);
            /////////////////////////////            
            LZM_StartNewTask(TaskShowDialing);
            return 0;
        }
        else
        {
            s_stPhoneBook.control =0x55;
            PhoneBook_HangUpCall();
            Public_ShowTextInfo("���г���,����",SECS(3));
            return 3;
        }
    }
}
/*************************************************************
** ��������: ShowPhoneBookTalkCall
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void ShowPhoneBookTalkCall(unsigned char index)
{
    STMSG_PHONEBOOK stPhoneBook;
    /////////////////////////
    Message_GetOnePhoneBookContent(index,&stPhoneBook);    
    //////////////////////////////////
    PhoneBook_CheckPhoneCallResult((unsigned char *)stPhoneBook.number,(unsigned char *)stPhoneBook.name);    
}
/*************************************************************
** ��������: TaskShowPhoneBook
** ��������: "��ʾ�绰��"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
LZM_RET TaskShowPhoneBook(LZM_MESSAGE *Msg)
{
    unsigned char temp;
    static unsigned char sucStep=0;
    static unsigned char sucMaxCount=0;
    switch(Msg->message)
    {
        case TASK_START:
            LcdClearScreen();            
            temp=Message_GetPhoneBookTotalNum();
            if(temp==0)
            {
                sucStep=0;
                sucMaxCount=0;
                LcdShowCaption("�绰��Ϊ��!", 23);                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
                s_stPhoneBook.control=0xff;
            }
            else
            {
                if(sucStep>=temp||temp!=sucMaxCount)
                {
                    sucStep =1;
                }
                sucMaxCount=temp;
                s_stPhoneBook.control = 0;
                ShowPhoneBookIndex(0xff,sucMaxCount);
                ShowPhoneBookIndex(sucStep,sucMaxCount);
            }
            return 1;
        case KEY_PRESS:
            if(s_stPhoneBook.control)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuPhone);
                    break;
                case KEY_ENTER://ȷ��
                    ShowPhoneBookTalkCall(sucStep);
                    break;
                case KEY_UP://--	
                    if(sucStep>1)
                    {
                        sucStep--;
                    }
                    else
                    {
                        sucStep = sucMaxCount;
                    }
                    ShowPhoneBookIndex(sucStep,sucMaxCount);
                    break;
                case KEY_DOWN://++	
                    if(sucStep<sucMaxCount)
                    {
                        sucStep++;
                    }
                    else
                    {
                        sucStep=1;
                    }
                    ShowPhoneBookIndex(sucStep,sucMaxCount);
                    break;
            }
            return 1;
        case TEST_TIMER:
            if(0xff==s_stPhoneBook.control)
            {
                LZM_StartNewTask(TaskMenuPhone);
            }
            break;
        case SHOW_TIMER:
            s_stPhoneBook.control =0;
            LcdPasteScreen();
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: ShowPhoneBookIndex
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void ShowCallRecordIndex(unsigned char index,unsigned char count)
{
    unsigned char hh,mm,ss;
    TIME_T stTime;
    char buffer[20];
    STCALLRECORD stCallRecord;
    LcdClearScreen();
    PhoneBook_ReadOneCallRecordData(index,&stCallRecord);    
    LcdShowStr(0,0,(char *)stCallRecord.name,0);
    sprintf(buffer,"%d/%-2d",count,index);
    LcdShowStr(90,0,buffer,1);
    LcdShowStr(0,16,(char *)stCallRecord.number,0);
    LcdShowStr(0,32,c_apcCallStatus[stCallRecord.status],0);
    if(stCallRecord.duration)
    {
        hh=stCallRecord.duration/3600;
        mm=stCallRecord.duration/60;
        ss=stCallRecord.duration%60;
        sprintf(buffer,"%02d:%02d:%02d",hh,mm,ss);
        LcdShowStr(36,32,buffer,0);
    }
    /////////////////////////////////
    Gmtime(&stTime,stCallRecord.startTime);
    Public_LcdShowTime(48,stTime);
}
/*************************************************************
** ��������: ShowPhoneBookTalkCall
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void ShowCallRecordTalkCall(unsigned char index)
{
    STCALLRECORD stCallRecord;    
    PhoneBook_ReadOneCallRecordData(index,&stCallRecord);
    PhoneBook_CheckPhoneCallResult((unsigned char *)stCallRecord.number,(unsigned char *)stCallRecord.name);
}
/*************************************************************
** ��������: TaskShowCallRecord
** ��������: "��ʾ�绰��"�˵�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
LZM_RET TaskShowCallRecord(LZM_MESSAGE *Msg)
{
    unsigned char temp;
    static unsigned char sucStep=0;
    static unsigned char sucMaxCount=0;
    switch(Msg->message)
    {
        case TASK_START:
            s_stPhoneBook.preTaskFlag = PHONE_TASK_RECORD;
            temp=PhoneBook_GetCallRecordTotalNum();
            if(temp==0)
            {
                sucStep=0;
                sucMaxCount=0;
                LcdClearScreen();
                LcdShowCaption("ͨ����¼Ϊ��!", 23);                
                LZM_SetAlarm(TEST_TIMER, SECS(2));
                s_stPhoneBook.control=0xff;
            }
            else
            {
                if(sucStep>=temp||temp!=sucMaxCount)
                {
                    sucStep =1;
                }
                sucMaxCount=temp;
                s_stPhoneBook.control = 0;
                ShowCallRecordIndex(sucStep,sucMaxCount);
            }
            return 1;
        case KEY_PRESS:
            if(s_stPhoneBook.control)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuPhone);
                    break;
                case KEY_ENTER://ȷ��
                    ShowCallRecordTalkCall(sucStep);
                    break;
                case KEY_UP://--	
                    if(sucStep>1)
                    {
                        sucStep--;
                    }
                    else
                    {
                        sucStep = sucMaxCount;
                    }
                    ShowCallRecordIndex(sucStep,sucMaxCount);
                    break;
                case KEY_DOWN://++	
                    if(sucStep<sucMaxCount)
                    {
                        sucStep++;
                    }
                    else
                    {
                        sucStep=1;
                    }
                    ShowCallRecordIndex(sucStep,sucMaxCount);
                    break;
            }
            return 1;
        case TEST_TIMER:
            if(0xff==s_stPhoneBook.control)
            {
                LZM_StartNewTask(TaskMenuPhone);
            }
            break;
        case SHOW_TIMER:
            s_stPhoneBook.control =0;
            LcdPasteScreen();
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskShowNewCallPhone
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
LZM_RET TaskShowNewCallPhone(LZM_MESSAGE *Msg)
{
    unsigned char buffer[10];
    switch(Msg->message)
    {
        case TASK_START:
            if(s_stPhoneBook.preTaskFlag>=PHONE_TASK_BACK)
            {
                LZM_StartNewTask(TaskShowMainInterface);
                return 1;
            }
            s_stPhoneBook.answermode = 0;
            if(EepromPram_ReadPram(E2_TERMINAL_GET_PHONE_STRATEGY_ID,buffer))
            {
                s_stPhoneBook.answermode =(unsigned char)Public_ConvertBufferToLong(buffer);
            }
            LcdClearScreen();
            LcdShowStr(0, 0,"������", 1); 
            LcdShowStr(0, 16,s_stCallRecord.number, 0);
            LcdShowStr(0, 32,s_stCallRecord.name, 0);
            LcdShowStr(0, 48,"ȷ����,���ؾܾ�",0);           
            s_stCallRecord.duration = 0;
            s_stCallRecord.startTime =RTC_GetCounter();
            s_stCallRecord.status = PHONE_STATUS_MISSED_IN;
            PhoneBook_SaveOneCallRecordData();
            LZM_SetAlarm(REFRESH_TIMER, PUBLICSECS(70));
            LZM_SetAlarm(TEST_TIMER, 1);
            SysAutoRunCountReset();
            s_stPhoneBook.specialFlag = Public_CheckPhoneIsSpecialNumber((unsigned char *)s_stCallRecord.number);            
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_SetAlarm(REFRESH_TIMER, 1); 
                    break;
                case KEY_ENTER://ȷ��
                    communication_CallPhone(gPhone.telephone,PHONE_TALK);
                    PhoneBook_SetCurrentCallStatus(CALL_STATUS_ANSWER_CALL); 
                    LZM_SetAlarm(TEST_TIMER, 1);
                    break;                                
                case KEY_UP://--                    
                    break;
                case KEY_DOWN://++                    
                    break;
            }
            return 1;
        case REFRESH_TIMER:
            PhoneBook_HangUpCall();
            PhoneBook_SetCurrentCallStatus(CALL_STATUS_HANG_UP); 
            LZM_SetAlarm(TEST_TIMER, 1);
            break;
        case TEST_TIMER:
            if(CALL_STATUS_HANG_UP==s_stPhoneBook.callstatus)
            {
                s_stPhoneBook.preTaskFlag = PHONE_TASK_BACK;
                LZM_ReturnOldTaskEx();
            }
            else
            if(CALL_STATUS_ANSWER_CALL==s_stPhoneBook.callstatus)
            {
                LZM_StartNewTaskEx(TaskShowCallTalking);
            }
            else
            if(CALL_STATUS_WAIT_FOR_CALL==s_stPhoneBook.callstatus)
            {
                communication_CallPhone(gPhone.telephone,PHONE_TALK);
                PhoneBook_SetCurrentCallStatus(CALL_STATUS_ANSWER_CALL); 
                LZM_SetAlarm(TEST_TIMER, 1);
            }
            else
            {
                SysAutoRunCountReset();
                LZM_SetAlarm(TEST_TIMER, PUBLICSECS(1));
                if(0==s_stPhoneBook.specialFlag&&Public_CheckIsPhoneServerRunStatus())
                {
                    PhoneBook_HangUpCall();
                    PhoneBook_SetCurrentCallStatus(CALL_STATUS_HANG_UP);
                    LZM_SetAlarm(TEST_TIMER, 1);
                    break;
                }
                else
                if(Io_ReadStatusBit(STATUS_BIT_ACC)||(0==s_stPhoneBook.answermode))
                {
                    PhoneBook_SetCurrentCallStatus(CALL_STATUS_WAIT_FOR_CALL); 
                    LZM_SetAlarm(TEST_TIMER, SECS(2));
                }
            }
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskShowDialing
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
LZM_RET TaskShowDialing(LZM_MESSAGE *Msg)
{
    switch(Msg->message)
    {
        case TASK_START:
            SysAutoRunCountReset();
            LcdClearScreen();
            LcdShowStr(0, 0,"���ں���...", 1);
            LcdShowStr(0, 16,s_stCallRecord.number, 0);
            LcdShowStr(0, 32,s_stCallRecord.name, 0);
            LcdShowStr(0, 48,"<����>�Ҷ�", 0);
            s_stCallRecord.duration = 0;
            s_stCallRecord.startTime =RTC_GetCounter();
            s_stCallRecord.status = PHONE_STATUS_MISSED_OUT;
            PhoneBook_SaveOneCallRecordData();
            LZM_SetAlarm(TEST_TIMER, PUBLICSECS(1));
            LZM_SetAlarm(REFRESH_TIMER, PUBLICSECS(70));
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_SetAlarm(REFRESH_TIMER, 1);                     
                    break;
                case KEY_ENTER://ȷ��
                    break;                                
                case KEY_UP://--                    
                    break;
                case KEY_DOWN://++                    
                    break;
            }
            return 1;
        case REFRESH_TIMER:  
            PhoneBook_HangUpCall();
            PhoneBook_SetCurrentCallStatus(CALL_STATUS_HANG_UP); 
            LZM_SetAlarm(TEST_TIMER, 1);
            break;
        case TEST_TIMER:
            if(CALL_STATUS_HANG_UP==s_stPhoneBook.callstatus)
            {
                if(PHONE_TASK_BOOK==s_stPhoneBook.preTaskFlag)
                {
                    s_stPhoneBook.preTaskFlag = PHONE_TASK_BACK;
                    LZM_StartNewTask(TaskShowPhoneBook);
                }
                else
                if(PHONE_TASK_RECORD==s_stPhoneBook.preTaskFlag)
                {
                    s_stPhoneBook.preTaskFlag = PHONE_TASK_BACK;
                    LZM_StartNewTask(TaskShowCallRecord);
                }
                else
                {
                    s_stPhoneBook.preTaskFlag = PHONE_TASK_BACK;
                    LZM_StartNewTask(TaskMenuPhone);
                }
            }
            else
            if(CALL_STATUS_ANSWER_CALL==s_stPhoneBook.callstatus)
            {
                LZM_StartNewTaskEx(TaskShowCallTalking);
            }
            else
            {
                SysAutoRunCountReset();
                LZM_SetAlarm(TEST_TIMER, PUBLICSECS(1));
                if(0==s_stPhoneBook.specialFlag&&Public_CheckIsPhoneServerRunStatus())
                {
                    PhoneBook_HangUpCall();
                    PhoneBook_SetCurrentCallStatus(CALL_STATUS_HANG_UP); 
                    LZM_SetAlarm(TEST_TIMER, 1);
                    break;
                }
            }
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TaskShowCallTalking
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
LZM_RET TaskShowCallTalking(LZM_MESSAGE *Msg)
{
    char buffer[20];
    unsigned char hh,mm,ss;
    switch(Msg->message)
    {
        case TASK_START:
            if(s_stPhoneBook.preTaskFlag>=PHONE_TASK_BACK)
            {
                LZM_StartNewTask(TaskShowMainInterface);
            }
            else
            {
                LcdClearScreen();
                LcdShowStr(0, 0,"ͨ����", 1);
                LcdShowStr(0, 16,s_stCallRecord.number, 0);
                LcdShowStr(0, 32,s_stCallRecord.name, 0);
                s_stCallRecord.duration = 0; 
                s_stCallRecord.status++;
                s_stCallRecord.startTime =RTC_GetCounter();            
                PhoneBook_ModifyCurCallRecord();
                LZM_SetTimer(TEST_TIMER, PUBLICSECS(1));
                SysAutoRunCountReset();
                s_stPhoneBook.control = 0;
            }
            return 1;
        case KEY_PRESS:
            switch(TaskPara)
            {
                case KEY_BACK://����
                    PhoneBook_HangUpCall();
                    PhoneBook_SetCurrentCallStatus(CALL_STATUS_HANG_UP); 
                    LZM_SetAlarm(TEST_TIMER, 1);                     
                    break;
                case KEY_ENTER://ȷ��
                    break;                                
                case KEY_UP://-- 
                    SetTelVolumeIncr();
                    s_stPhoneBook.control = 1;
                    break;
                case KEY_DOWN://++
                    SetTelVolumeDecr();
                    s_stPhoneBook.control = 1;
                    break;
            }
            return 1;
        case TEST_TIMER:
            if(CALL_STATUS_HANG_UP==s_stPhoneBook.callstatus)
            {
                s_stPhoneBook.preTaskFlag = PHONE_TASK_BACK;
                LZM_KillTimer(TEST_TIMER);
                PhoneBook_ModifyCurCallRecord();
                LZM_ReturnOldTaskEx();
            }
            else
            {
                if(0==s_stPhoneBook.specialFlag&&Public_CheckIsPhoneServerRunStatus())
                {
                    PhoneBook_HangUpCall();
                    PhoneBook_SetCurrentCallStatus(CALL_STATUS_HANG_UP); 
                    LZM_SetAlarm(TEST_TIMER, 1);
                    break;//
                }
                s_stCallRecord.duration++;
                hh=s_stCallRecord.duration/3600;
                mm=s_stCallRecord.duration/60;
                ss=s_stCallRecord.duration%60;
                sprintf(buffer,"%02d:%02d:%02d",hh,mm,ss);
                LcdShowStr(0, 48,buffer, 0);
                SysAutoRunCountReset();
                if(s_stPhoneBook.control)
                {
                    s_stPhoneBook.control++;
                    if(s_stPhoneBook.control<6)
                    {
                        sprintf(buffer,"v:%d",GetTelVolume());
                        LcdShowStr(100, 0,buffer, 0);
                    }
                    else
                    {
                        s_stPhoneBook.control = 0;
                        LcdClearArea(100, 0, 128,16);
                    }
                }
            }
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
/*************************************************************
** ��������: TakeCallMonitorServerPhone
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void TakeCallMonitorServerPhone(void)
{
    unsigned char len;
    unsigned char buffer[20];
    len = EepromPram_ReadPram(E2_MONITOR_SERVER_PHONE_ID, buffer);
    buffer[len]='\0';
    if(0==len)
    {
        s_stPhoneBook.control =0x55;
        Public_ShowTextInfo("���ĺ���δ����",SECS(3));
        return;
    }
    //////////////////////////
    if(communication_CallPhone(buffer,PHONE_NORMAL))
    {
        PhoneBook_SetCurrentCallStatus(CALL_STATUS_MAX);
        strcpy(s_stCallRecord.number,(char *)buffer);
        strcpy(s_stCallRecord.name,"�ͷ�����");
        s_stCallRecord.callflag = 3;
        s_stPhoneBook.specialFlag = 1;
        s_stPhoneBook.preTaskFlag = PHONE_TASK_HOT_LINE;
        LZM_StartNewTask(TaskShowDialing);
    }
    else
    {
        s_stPhoneBook.control =0x55;
        PhoneBook_HangUpCall();
        Public_ShowTextInfo("���г���,����",SECS(3));
    }
}
/*************************************************************
** ��������: TaskMenuPhone
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
LZM_RET TaskMenuPhone(LZM_MESSAGE *Msg)
{ 
    switch(Msg->message)
    {
        case TASK_START:
            MenuInit(&s_stMenuPhone,s_apcMenuPhone);
            MenuShow(&s_stMenuPhone);
            s_stPhoneBook.control =0;
            s_stPhoneBook.preTaskFlag = PHONE_TASK_EMPTY;
            return 1;
        case KEY_PRESS:
            if(s_stPhoneBook.control)break;
            switch(TaskPara)
            {
                case KEY_BACK:
                    LZM_StartNewTask(TaskMenuMain);
                    break;
                case KEY_ENTER:	
                    switch(MenuGetHandle(&s_stMenuPhone))
                    {
                        case MENU_PHONE_HOT_LINE:
                            TakeCallMonitorServerPhone();
                            break;
                        case MENU_PHONE_BOOK:
                            LZM_StartNewTask(TaskShowPhoneBook);
                            break;
                        case MENU_PHONE_RECORD://
                            LZM_StartNewTask(TaskShowCallRecord);
                            break;
                        case MENU_PHONE_MAKE://
                            LZM_StartNewTask(TaskShowMakeNewDial);
                            break;
                            
                    }
                    break;
                default:
                    MenuResponse(&s_stMenuPhone,TaskPara);
                    break;
            }
            return 1;
        case SHOW_TIMER:
            s_stPhoneBook.control =0;
            LcdPasteScreen();            
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}

/*************************************************************
** ��������: TaskShowMakeNewDial
** ��������: ����绰 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
LZM_RET TaskShowMakeNewDial(LZM_MESSAGE *Msg)
{
    static ST_ESPIN stESpinPhone;
    switch(Msg->message)
    {
        case TASK_START:            
            LcdClearScreen();
            s_stPhoneBook.control=0;
            LcdShowCaption("����绰",0);
            LcdShowHorLine(0, 131, 18, 1);
            LcdShowHorLine(0, 131, 45, 1);
            LcdShowCaption("��<ȷ��>����",47);
            if((strlen(stESpinPhone.buffer)!=12)||(stESpinPhone.width != 12))
            {
                ESpinInit(&stESpinPhone, 16, 24, 12,1);
                stESpinPhone.type = 1;//�ַ���
                strcpy(stESpinPhone.buffer,"000000000000"); 
            }
            ESpinShow(&stESpinPhone);
            ESpinSetFocus(&stESpinPhone);
            return 1;
        case KEY_PRESS:
            if(s_stPhoneBook.control==0xff)break;
            switch(TaskPara)
            {
                case KEY_BACK://����
                    LZM_StartNewTask(TaskMenuPhone);//ϵͳ����
                    break;
                case KEY_ENTER://ȷ��
                    if(stESpinPhone.buffer[0]==0x30)
                    {
                        unsigned char flag;
                        unsigned char name[20];                        
                        flag=Public_CheckPhoneInformation((unsigned char *)&stESpinPhone.buffer[1],name);//0��û�иõ绰��������ϣ�1: ���룻2: ������3������/����
                        if(flag)
                        {
                            PhoneBook_CheckPhoneCallResult((unsigned char *)&stESpinPhone.buffer[1],NULL);
                        }
                        else
                        {
                            PhoneBook_CheckPhoneCallResult((unsigned char *)stESpinPhone.buffer,NULL);
                        }
                    }
                    else
                    {
                        PhoneBook_CheckPhoneCallResult((unsigned char *)stESpinPhone.buffer,NULL);
                    }
                    break;                                
                default:                    
                    ESpinResponse(&stESpinPhone,TaskPara);                     
                    break;
            }
            return 1;
        case INT_RTC:
            break;
        default:
            return LZM_DefTaskProc(Msg);
    }
    return 0;
}
#endif
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

