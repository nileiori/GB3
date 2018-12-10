
/*******************************************************************************
 * File Name:           Image_Store.c
 * Function Describe:    µœ÷∂‡√ΩÃÂÕº∆¨ºÏÀ˜£¨µ•ÃıÕº∆¨ºÏÀ˜…œ¥´π¶ƒ‹
 * Relate Module:
 * Writer:              fanqinghai 
 * Date:                2016-03-15
 *******************************************************************************/


#include "include.h"


#define BUFFER_SIZE             550
#define ATTEMPE_STEP_LEN      6*FLASH_ONE_SECTOR_BYTES   //“ª∏±Õº∆¨◊Ó∂‡∑÷≈‰24kø’º‰
#define STORE_IMAGE_NMB         3    //ø…±£¥ÊÕº∆¨’≈ ˝

static u32 SendMediaID;
u8 FlashBuffer[BUFFER_SIZE];    

/**************************************************************************
//∫Ø ˝√˚£∫delay
//π¶ƒ‹£∫—” ±∫Ø ˝
// ‰»Î£∫Œﬁ
// ‰≥ˆ£∫Œﬁ
//∑µªÿ÷µ£∫Œﬁ
//±∏◊¢£∫
***************************************************************************/
void delay()
{
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
}

/**************************************************************************
//∫Ø ˝√˚£∫Store_Image_Date
//π¶ƒ‹£∫ µœ÷Õº∆¨¥Ê¥¢µΩÃ˙µÁ¿Ô
// ‰»Î£∫Œﬁ
// ‰≥ˆ£∫Œﬁ
//∑µªÿ÷µ£∫Œﬁ
//±∏◊¢£∫
***************************************************************************/
u8 Store_Image_Date(u16 pacagenmb,u8 *pBuffer ,u16 length)
{

    static u32 Address;
    static u8 CurrentStep =0;
    static u16 LastPacagenmb ;
    u16 TotalPacage;
    static u16 TotalDataLong;
   // u8 bufer[4] = {0};
    u8 i =0;
    u32 RtcCount ;
    memset(FlashBuffer,0,BUFFER_SIZE);
    memcpy(FlashBuffer,pBuffer,length);
    //ªÒ»°µ±«∞µÿ÷∑
    if(0 == pacagenmb)   //µ⁄“ª∞¸
    {
        
        CurrentStep ++;     //”– ˝æ›
        if(CurrentStep>2)
        {
            CurrentStep =0;
        }
        Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
        for(i=0;i<6;i++)    //√ø∏ˆÕº∆¨∑÷≈‰24Kø’º‰
        {
            
            sFLASH_EraseSector(Address);
            Address += FLASH_ONE_SECTOR_BYTES;
            IWDG_ReloadCounter();
        }
        
        delay();     //ø…”–ø…Œﬁ
        TotalDataLong = 0;
        LastPacagenmb = 0;
        Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN; //µÿ÷∑∏¥Œª
    }
    //–¥ ˝æ› ‘ –Ì—≠ª∑∏≤∏«–¥
    sFLASH_WriteBuffer(FlashBuffer, Address, length);
    Address += length; //
    TotalDataLong += length;
    
     if(0xAA == pacagenmb)
     {
         memset(FlashBuffer,0,2);
         TotalPacage = LastPacagenmb+1;
         TotalPacage += 1;              //∞¸¥”µ⁄0∞¸ø™ º£¨À˘“‘‘Ÿº”1∞¸
         FlashBuffer[0] = (TotalPacage&0xff00)>>8;
         FlashBuffer[1] = TotalPacage&0x00ff;
         Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -2;
         sFLASH_WriteBuffer(FlashBuffer, Address, 2);

         ////±£¥Ê ˝æ›◊‹≥§∂»//////////////
        memset(FlashBuffer,0,2);
        FlashBuffer[0] = (TotalDataLong&0xff00)>>8;
        FlashBuffer[1] = TotalDataLong&0x00ff;
        Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -4;
        sFLASH_WriteBuffer(FlashBuffer, Address, 2);

       ////±£¥Ê≈ƒ’’ ±º‰//////////////
        memset(FlashBuffer,0,4);
        RtcCount = RTC_GetCounter();
        FlashBuffer[0] = (RtcCount&0xff000000)>>24;
        FlashBuffer[1] = (RtcCount&0x00ff0000)>>16;
        FlashBuffer[2] = (RtcCount&0x0000ff00)>>8;
        FlashBuffer[3] = RtcCount&0x000000ff;
        
        Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -8;
        sFLASH_WriteBuffer(FlashBuffer, Address, 4);
     }
    LastPacagenmb =  pacagenmb ; 
    return 0;
}
/**************************************************************************
//∫Ø ˝√˚£∫Get_ID
//π¶ƒ‹£∫ªÒµ√“™ºÏÀ˜µƒÕº∆¨id
// ‰»Î£∫Œﬁ
// ‰≥ˆ£∫Œﬁ
//∑µªÿ÷µ£∫Œﬁ
//±∏◊¢£∫
***************************************************************************/
void Get_ID(u32 MediaID)
{
    SendMediaID = MediaID;

}
/**************************************************************************
//∫Ø ˝√˚£∫MediaSearch_GetMediaIndex
//π¶ƒ‹£∫≤È—Ø÷∏∂® ±º‰µƒÕº∆¨–≈œ¢
// ‰»Î£∫Œﬁ
// ‰≥ˆ£∫Œﬁ
//∑µªÿ÷µ£∫Œﬁ
//±∏◊¢£∫
***************************************************************************/
u16 MediaSearch_GetMediaIndex(u8 *pBuffer, u8 MediaType, TIME_T *StartTime, TIME_T *EndTime)
{
    u32 Address;
    u32 FindTime;
    u32 Start_Time;
    u32 End_Time;
    u16 Length;
    u8 CurrentStep =0;
    u8 i;
    u8 Time[4];
    static u8 *p=NULL;
        
    p= pBuffer;
    Length = 0;
    if(!(Public_CheckTimeStruct(StartTime))||(!Public_CheckTimeStruct(EndTime))) // ±º‰Ω·ππ≥ˆ¥Ì
    {
        return 0;
    }
     //÷ª÷ß≥÷Õº∆¨ºÏÀ˜
	if(MediaType  ==1)   //“Ù∆µ
	{
        //∂‡√ΩÃÂID
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x01;
        //∂‡√ΩÃÂ¿‡–Õ
        *p++ = 0x01;
        //Õ®µ¿ID
        *p++ = 0x01;
        // ¬º˛±‡¬Î
        *p++ = 0;
        //Œª÷√–≈œ¢
        Report_GetPositionBasicInfo(p);
        Length += 35;
        p = NULL;
        return Length ;
    }
    else if(MediaType  ==2)  // ”∆µ
    {
        //∂‡√ΩÃÂID
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x01;
        //∂‡√ΩÃÂ¿‡–Õ
        *p++ = 0x02;
        //Õ®µ¿ID
        *p++ = 0x01;
        // ¬º˛±‡¬Î
        *p++ = 0;
        //Œª÷√–≈œ¢
        Report_GetPositionBasicInfo(p);
        Length += 35;
        p = NULL;
        return Length ;
    }
    else if(MediaType  ==0)   //ÕºœÒ
    {
        CurrentStep = 0;
      for(i=0;i<STORE_IMAGE_NMB;i++)

        {

            IWDG_ReloadCounter();
            CurrentStep ++;     //”– ˝æ›
            if(CurrentStep>2)
            {
                CurrentStep =0;
            }
            
            Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -8;
            sFLASH_ReadBuffer(Time, Address, 4);
            FindTime = 0;
            FindTime |= Time[0]<<24;
            FindTime |= Time[1]<<16;
            FindTime |= Time[2]<<8;
            FindTime |= Time[3];
            
            //Public_ConvertBCDToTime(tt,Time);
            Start_Time = ConverseGmtime(StartTime);
            End_Time = ConverseGmtime(EndTime);
            if((FindTime>Start_Time)&&(FindTime<End_Time))
            {
                memset(FlashBuffer,0,50);
                Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
                sFLASH_ReadBuffer(FlashBuffer, Address, 36);
                //∂‡√ΩÃÂID
                *p++ = FlashBuffer[0];
                *p++ = FlashBuffer[1];
                *p++ = FlashBuffer[2];
                *p++ = FlashBuffer[3];
                //∂‡√ΩÃÂ¿‡–Õ
                *p++ = FlashBuffer[4];
                //Õ®µ¿ID
                *p++ = FlashBuffer[7];
                // ¬º˛±‡¬Î
                *p++ = FlashBuffer[6];
                //Œª÷√–≈œ¢
                memcpy(p,FlashBuffer+8,28);
                p += 28;
                //Report_GetPositionBasicInfo(p);
                Length += 35;
        
            }
        }
        p = NULL;
        return Length ;
    }
    return 0;
}
/**************************************************************************
//∫Ø ˝√˚£∫SendImage_TimeTask
//π¶ƒ‹£∫ µœ÷Õº∆¨∞¥IDºÏÀ˜∑¢ÀÕ
// ‰»Î£∫Œﬁ
// ‰≥ˆ£∫Œﬁ
//∑µªÿ÷µ£∫Œﬁ
//±∏◊¢£∫
***************************************************************************/
FunctionalState SendImage_TimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    u32 ReadID;
    static u8 CurrentStep = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u16 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//Œ™±‡“ÎÕ®π˝∂¯ÃÌº”
    u8 i =0;

    if(!flag)
    {
        CurrentStep = 0 ;
        for(i=0;i<STORE_IMAGE_NMB;i++)
        {
            IWDG_ReloadCounter();
            CurrentStep ++;     //”– ˝æ›
            if(CurrentStep>2)
            {
                CurrentStep =0;
            }
            
            Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 100);
            ReadID = 0;
            ReadID |= FlashBuffer[0]<<24;
            ReadID |= FlashBuffer[1]<<16;
            ReadID |= FlashBuffer[2]<<8;
            ReadID |= FlashBuffer[3];
            
            if(SendMediaID == ReadID)
            {
                memset(FlashBuffer,0,2);
                TotalPacage = 0;
                TotalDataLong = 0;

                Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -2;
                sFLASH_ReadBuffer(FlashBuffer, Address, 2);
                TotalPacage |= FlashBuffer[0]<<8;
                TotalPacage |= FlashBuffer[1];

                memset(FlashBuffer,0,2);
                Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -4;
                sFLASH_ReadBuffer(FlashBuffer, Address, 2);
                TotalDataLong |= FlashBuffer[0]<<8;
                TotalDataLong |= FlashBuffer[1];

                flag = 1;
                SendPacage = 1;    // ¥”µ⁄“ª∞¸ø™ º
                Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
        
                break;
            }
        
        }
        if(3==i)
        {
            flag = 0;
            return DISABLE;
        }
    }
    else
    {
        if(SendPacage<=TotalPacage)
        {

            if(SendPacage==TotalPacage)
            {
                Length = TotalDataLong%512;
            }
            else
            {
                Length = 512;
            }
            memset(FlashBuffer,0,BUFFER_SIZE);
            sFLASH_ReadBuffer(FlashBuffer, Address, Length);
            
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;

              return ENABLE; 
            }
        }
        else               //∑¢ÀÕÕÍ±œ
        {
            flag = 0;                   
            return DISABLE;
        }

    }
    return ENABLE;
}

/*********************************************************************
//∫Ø ˝√˚≥∆  :Media_UploadTimeTask(void)
//π¶ƒ‹      :¥Ê¥¢∂‡√ΩÃÂ ˝æ›…œ¥´∂® ±»ŒŒÒ£¨∑¢ÀÕ ”∆µŒƒº˛
// ‰»Î      :
// ‰≥ˆ      :
// π”√◊ ‘¥  :
//»´æ÷±‰¡ø  :
//µ˜”√∫Ø ˝  :
//÷–∂œ◊ ‘¥  :
//∑µªÿ      :
//±∏◊¢      :
*********************************************************************/
FunctionalState MediaUploadTimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u32 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//Œ™±‡“ÎÕ®π˝∂¯ÃÌº”
    u8 *p = NULL;
        
        if(! flag)
        {
            
            Address = VIDEO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
            
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 4);
            
            TotalDataLong |= FlashBuffer[0]<<24;
            TotalDataLong |= FlashBuffer[1]<<16;
            TotalDataLong |= FlashBuffer[2]<<8;
            TotalDataLong |= FlashBuffer[3];
            
            TotalPacage = TotalDataLong/512 + 1;
            memset(FlashBuffer,0,BUFFER_SIZE);
             p=FlashBuffer;
            *p++ = 0;//∂‡√ΩÃÂID
            *p++ = 0;
            *p++ = 0;
            *p++ = 0x01;
            
            *p++ = 2;//∂‡√ΩÃÂ¿‡–Õ£¨2Œ™ ”∆µ
            *p++ = 0xaa;//∂‡√ΩÃÂ∏Ò Ω£ ±Ì æ∆‰À¸¿‡–Õ
            *p++ = 0x00;//∂‡√ΩÃÂ ¬º˛  0:∆ΩÃ®◊‘∂Ø…œ∑¢
            *p++ = 0x01;//∂‡√ΩÃÂÕ®µ¿
            Report_GetPositionBasicInfo(p);
            Address = VIDEO_START_SECTOR*FLASH_ONE_SECTOR_BYTES+4;
            sFLASH_ReadBuffer(FlashBuffer+36, Address, 512);
            flag = 1;
            SendPacage = 1;
            Length =  512+36;
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;
            
              return ENABLE; 
            }
            
        }

        else
        {

            if(SendPacage<=TotalPacage)
            {

                if(SendPacage==TotalPacage)
                {
                    Length = TotalDataLong%512;
                }
                else
                {
                    Length = 512;
                }
                memset(FlashBuffer,0,BUFFER_SIZE);
                sFLASH_ReadBuffer(FlashBuffer, Address, Length);
                
                if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
                {
                    SendPacage++;
                    Address += 512;

                  return ENABLE; 
                }
            }
            else               //∑¢ÀÕÕÍ±œ
            {
                flag = 0;                   
                return DISABLE;
            }


        }
     return ENABLE;     
}
/*********************************************************************
//∫Ø ˝√˚≥∆  :SendAudio_TimeTask(void)
//π¶ƒ‹      :∑¢ÀÕ¬º“ÙŒƒº˛
// ‰»Î      :
// ‰≥ˆ      :
// π”√◊ ‘¥  :
//»´æ÷±‰¡ø  :
//µ˜”√∫Ø ˝  :
//÷–∂œ◊ ‘¥  :
//∑µªÿ      :
//±∏◊¢      :
*********************************************************************/
FunctionalState SendAudio_TimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u32 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//Œ™±‡“ÎÕ®π˝∂¯ÃÌº”
    u8 *p = NULL;
        
        if(! flag)
        {
            
            Address = AUDIO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
            
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 4);
            
            TotalDataLong |= FlashBuffer[0]<<24;
            TotalDataLong |= FlashBuffer[1]<<16;
            TotalDataLong |= FlashBuffer[2]<<8;
            TotalDataLong |= FlashBuffer[3];            
            TotalPacage = TotalDataLong/512 + 1;
            memset(FlashBuffer,0,BUFFER_SIZE);
            p=FlashBuffer;
            *p++ = 0;//∂‡√ΩÃÂID
            *p++ = 0;
            *p++ = 0;
            *p++ = 0x01;
            *p++ = 1;//∂‡√ΩÃÂ¿‡–Õ£¨1Œ™“Ù∆µ
            *p++ = 0x02;//∂‡√ΩÃÂ∏Ò Ω£ 2Œ™MP3
            *p++ = 0x00;//∂‡√ΩÃÂ ¬º˛  0:∆ΩÃ®◊‘∂Ø…œ∑¢
            *p++ = 0x01;//∂‡√ΩÃÂÕ®µ¿
            Report_GetPositionBasicInfo(p);
            Address = AUDIO_START_SECTOR*FLASH_ONE_SECTOR_BYTES+4;
            sFLASH_ReadBuffer(FlashBuffer+36, Address, 512);
            flag = 1;
            SendPacage = 1;
            Length =  512+36;
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;
            
              return ENABLE; 
            }
            
        }

        else
        {

            if(SendPacage<=TotalPacage)
            {

                if(SendPacage==TotalPacage)
                {
                    Length = TotalDataLong%512;
                }
                else
                {
                    Length = 512;
                }
                memset(FlashBuffer,0,BUFFER_SIZE);
                sFLASH_ReadBuffer(FlashBuffer, Address, Length);
                
                if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
                {
                    SendPacage++;
                    Address += 512;

                  return ENABLE; 
                }
            }
            else               //∑¢ÀÕÕÍ±œ
            {
                flag = 0;                   
                return DISABLE;
            }


        }
        
    return ENABLE; 
}









