/************************************************************************
//程序名称：VDR_Usb.c
//功能：采集行驶记录仪数据保存至U盘*.VDR文件中。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2014.10
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：
*************************************************************************/

/********************文件包含*************************/
#include "VDR.h"
#include "ff.h"
#include "Lcd.h"
#include "Gdi.h"
//#include "stm32f2xx.h"
#include "stm32f10x.h"


/********************本地变量*************************/
u8 VdrUsbReadBuffer[VDR_USB_READ_BUFFER_SIZE] = {0};
static WCHAR filename[50];


/********************全局变量*************************/
const u16 CarPlate_Gb2312[]=
{
0xBEA9,0xCFE6,0xBDF2,0xB6F5,0xBBA6,0xD4C1,0xD3E5,0xC7ED,0xBCBD,0xB4A8,
0xBDFA,0xB9F3,0xC7AD,0xC1C9,0xD4C6,0xB5E1,0xBCAA,0xC9C2,0xC7D8,0xD4A5,
0xB8CA,0xC2A4,0xCBD5,0xC7E0,0xD5E3,0xCDEE,0xB2D8,0xC3F6,0xC3C9,0xB8D3,
0xB9F0,0xC2B3,0xC4FE,0xBADA,0xD0C2,0xB8DB,0xB0C4,0xCCA8,0xBEFC,0xBFD5,
0xBAA3,0xB1B1,0xC9F2,0xC0BC,0xBCC3,0xC4CF,0xB9E3,0xB3C9,0xBEAF
};
const u16 CarPlate_Unicode[]=
{
0x4EAC,0x6E58,0x6D25,0x9102,0x6CAA,0x7CA4,0x6E1D,0x743C,0x5180,0x5DDD,
0x664B,0x8D35,0x9ED4,0x8FBD,0x4E91,0x6EC7,0x5409,0x9655,0x79E6,0x8C6B,
0x7518,0x9647,0x82CF,0x9752,0x6D59,0x7696,0x85CF,0x95FD,0x8499,0x8D63,
0x6842,0x9C81,0x5B81,0x9ED1,0x65B0,0x6E2F,0x6FB3,0x53F0,0x519B,0x7A7A,
0x6D77,0x5317,0x6C88,0x5170,0x6D4E,0x5357,0x5E7F,0x6210,0x8B66
};

const u16 CarPlate_Unknown[]=
{
0x672A,0x77E5,0x8F66,0x724C//未知车牌
};

const u16 CarPlate_ExNm[]=
{
0x002E,0x0056,0x0044,0x0052//扩展名 .VDR
};

const u8 Usb_Data_Name[][19]=//数据名称
{
    "执行标准版本年号  ",            
    "当前驾驶人信息    ",
    "实时时间          ",
    "累计行驶里程      ",
    "脉冲系数          ",
    "车辆信息          ",
    "状态信号配置信息  ",
    "记录仪唯一性编号  ",
    "行驶速度记录      ",
    "位置信息记录      ",
    "事故疑点记录      ",
    "超时驾驶记录      ",
    "驾驶人身份记录    ",
    "外部供电记录      ",
    "参数修改记录      ",
    "速度状态日志      "    
};

/********************外部变量*************************/
extern FIL file;
extern const u16 DataCollectLen[];

/********************本地函数声明*********************/
static void VDRUsb_SaveBlockNum(void);
static u8 VDRUsb_SaveNoPacketData(u8 Cmd);
static u16 VDRUsb_ReadNoPacketData(u8 *pBuffer,u8 Cmd);
static u8 VDRUsb_SavePacketData(u8 Cmd);
static void VDRUsb_SaveVerifyCode(void);
static void Vdr_Usb_UniNm(WCHAR *p_name);
static void Vdr_Usb_XorVer(u8 *pVer,u8 *p,u16 len);
/********************函数定义*************************/

/**************************************************************************
//函数名：VDRUsb_CreatVdrFile
//功能：生成*.VDR文件
//输入：无
//输出：无
//返回值：无
//备注：VDR文件的格式见GB/T19056-2012附录B
//该函数仅被USB_USR_ReadDataForUpdataApp函数调用
***************************************************************************/
void VDRUsb_CreatVdrFile(void)
{
	//WCHAR filename[50];
	u8 Cmd[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};
	u8 i;
	u8 flag;
	FRESULT res;

    	LCD_LIGHT_ON();
    
    	Vdr_Usb_UniNm(filename);//创建文件名
    
    	if(f_open(&file,(WCHAR*)&filename,FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
    	{
				  res = f_lseek(&file, 0);//从原点开始
          if(FR_OK !=res)return ; 
				
        	LcdClearScreen();
        	LcdShowCaption("导出数据,请稍候", 2);  

		      VDRUsb_SaveBlockNum();//写入数据块个数,固定为16

				/*
                VDRUsb_SaveNoPacketData(0x00); //00H-07H数据,不用分包读取
                VDRUsb_SaveNoPacketData(0x01); //00H-07H数据,不用分包读取
                VDRUsb_SaveNoPacketData(0x02); //00H-07H数据,不用分包读取
                VDRUsb_SaveNoPacketData(0x03); //00H-07H数据,不用分包读取
                VDRUsb_SaveNoPacketData(0x04); //00H-07H数据,不用分包读取
                VDRUsb_SaveNoPacketData(0x05); //00H-07H数据,不用分包读取
                VDRUsb_SaveNoPacketData(0x06); //00H-07H数据,不用分包读取
                VDRUsb_SaveNoPacketData(0x07); //00H-07H数据,不用分包读取
                
                VDRUsb_SavePacketData(0x08);  //08H-15H记录数据,需要分包读取
                VDRUsb_SavePacketData(0x09);  //08H-15H记录数据,需要分包读取
                VDRUsb_SavePacketData(0x10);  //08H-15H记录数据,需要分包读取
                VDRUsb_SavePacketData(0x11);  //08H-15H记录数据,需要分包读取
                VDRUsb_SavePacketData(0x12);  //08H-15H记录数据,需要分包读取
                VDRUsb_SavePacketData(0x13);  //08H-15H记录数据,需要分包读取
                VDRUsb_SavePacketData(0x14);  //08H-15H记录数据,需要分包读取
                VDRUsb_SavePacketData(0x15);  //08H-15H记录数据,需要分包读取
								*/
							
          flag = 0;							
				  for(i=0; i<8; i++)
					{
						  if(0 == flag)
							{
					        if(1 == VDRUsb_SaveNoPacketData(Cmd[i]))
							    {
							        flag = 1;//出现异常
								      break;
							    }
						  }
					}
					
					for(i=8; i<16; i++)
					{
					    if(0 == flag)
							{
					        if(1 == VDRUsb_SavePacketData(Cmd[i]))
							    {
							        flag = 1;//出现异常
								      break;
							    }
						  }
					}
                
        	VDRUsb_SaveVerifyCode();//写入校验
        
        	LCD_LIGHT_OFF();
        
					if(0 == flag)
					{
        	    LcdShowMsgEx("数据导出完毕",PUBLICSECS(6)); 
					}
					else
					{
					    LcdShowMsgEx("数据导出错误!!",PUBLICSECS(6));
					}
    	}
    	else
    	{
        	LcdShowMsgEx("数据导出错误!!",PUBLICSECS(6));
    	}	

        f_close(&file);    	
}
/**************************************************************************
//函数名：VDRUsb_SaveBlockNum
//功能：写*.VDR文件中数据块个数
//输入：无
//输出：无
//返回值：无
//备注：该函数仅被VDRUsb_SaveData函数调用
***************************************************************************/
static void VDRUsb_SaveBlockNum(void)
{
	u8 Buffer[3] = {0};
	u32 flen;

	Buffer[0] = 0;
    	Buffer[1] = 16;//00H-15H 16个数据块
    	f_write (&file, Buffer, 2, &flen);                 
}
/**************************************************************************
//函数名：VDRUsb_SaveNoPacketData
//功能：写*.VDR文件中数据块1~数据块8
//输入：无
//输出：无
//返回值：0:正常;1:异常
//备注：数据块1~数据块8对应着命令字00H-07H,该函数仅被VDRUsb_SaveData函数调用
***************************************************************************/
static u8 VDRUsb_SaveNoPacketData(u8 Cmd)
{
	u8 	Buffer[200] = {0};
	u16 	BufferLen;
	u32 	flen;

	if(Cmd > 7)
	{
		return 1;
	}

	BufferLen = VDRUsb_ReadNoPacketData(Buffer+23,Cmd);//数据内容

	Buffer[0] = Cmd;//数据代码
	memcpy(Buffer+1,Usb_Data_Name[Cmd],18);//数据名称
	Buffer[19] = 0x00;//数据长度
	Buffer[20] = 0x00;
	Buffer[21] = (BufferLen&0xff00)>>8;
	Buffer[22] = BufferLen&0xff;

	f_write (&file, Buffer, BufferLen+23, &flen);

  if(0 == flen)
	{
	    return 1;
	}		
	else
	{
	    return 0;
	}
	
}

/**************************************************************************
//函数名：VDRUsb_ReadNoPacketData
//功能：获取*.VDR文件中某个指令(00H-07H)的数据
//输入：Cmd:命令字
//输出：pBuffer:指向该命令字对应的数据首地址
//返回值：数据长度
//备注：00H-07H指令的数据不分包，VDR文件的格式见GB/T19056-2012附录B
//数据格式为：数据代码+数据名称+数据长度+数据内容
***************************************************************************/
static u16 VDRUsb_ReadNoPacketData(u8 *pBuffer,u8 Cmd)
{
	u16 length;
	u8 SrcBuffer[1];
	u8 SrcBufferLen;
	u8 *p = NULL;
	//u8 *q = NULL;

	if(Cmd > 7)
	{
		return 0;
	}
	else
	{
		p = pBuffer;
		SrcBufferLen = 0;

		switch(Cmd)
		{
			case VDR_PROTOCOL_CMD_00H:
			{
				length = VDRProtocol_ParseCmd00H(p,SrcBuffer,SrcBufferLen);
				break;
			}
			case VDR_PROTOCOL_CMD_01H:
			{
				length = VDRProtocol_ParseCmd01H(p,SrcBuffer,SrcBufferLen);
				break;
			}
			case VDR_PROTOCOL_CMD_02H:
			{
				length = VDRProtocol_ParseCmd02H(p,SrcBuffer,SrcBufferLen);
				break;
			}
			case VDR_PROTOCOL_CMD_03H:
			{
				length = VDRProtocol_ParseCmd03H(p,SrcBuffer,SrcBufferLen);
				break;
			}
			case VDR_PROTOCOL_CMD_04H:
			{
				length = VDRProtocol_ParseCmd04H(p,SrcBuffer,SrcBufferLen);
				break;
			}
			case VDR_PROTOCOL_CMD_05H:
			{
				length = VDRProtocol_ParseCmd05H(p,SrcBuffer,SrcBufferLen);
				break;
			}
			case VDR_PROTOCOL_CMD_06H:
			{
				length = VDRProtocol_ParseCmd06H(p,SrcBuffer,SrcBufferLen);
				break;
			}
			case VDR_PROTOCOL_CMD_07H:
			{
				length = VDRProtocol_ParseCmd07H(p,SrcBuffer,SrcBufferLen);
				break;
			}
			default: break;

		}

		return length;
	}
}
/**************************************************************************
//函数名：VDRUsb_SavePacketData
//功能：写*.VDR文件中数据块9~数据块16
//输入：无
//输出：无
//返回值：0:正常;1:异常
//备注：数据块9~数据块16对应着命令字08H-15H,该函数仅被VDRUsb_SaveData函数调用
***************************************************************************/
static u8 VDRUsb_SavePacketData(u8 Cmd)
{
	u8 	Type;
        u8      NameIndex;
	u8 	*p = NULL;
	u8	WriteEnableFlag;
	u16 	TotalList;
	s16 	i,j;
	u16	BufferLen;
	u32 	length;
	u32   length2;
	u32 	flen;
	

	if(((Cmd >= 0x0a)&&(Cmd <= 0x0f))||(Cmd > 0x15)||(Cmd < 0x08))
	{
		return 1;
	}

	if(Cmd < 0x10)
	{
		Type = Cmd - 0x08;
                NameIndex = Cmd;
	}
	else
	{
		Type = Cmd - 0x0e;
                NameIndex = Cmd-6;
	}

	WriteEnableFlag = VDRData_ReadWriteEnableFlag(Type);
	if(1 == WriteEnableFlag)//当前写使能
	{
		VDRData_SetWriteEnableFlag(Type, 0);//禁止写,注意如果调用了禁止写,后面就必须调用使能写
	}

	VdrUsbReadBuffer[0] = Cmd;//数据代码

        
	memcpy(VdrUsbReadBuffer+1,Usb_Data_Name[NameIndex],18);//数据名称

	TotalList = VDRData_ReadTotalList(Type);//数据长度
	if(VDR_DATA_TYPE_POSITION == Type)//dxl,2015.12.17,把最新的那个小时的位置信息给补上
	{
	  length = TotalList*DataCollectLen[Type];
		length2 = VDRPosition_GetCurHourPosition(VdrUsbReadBuffer+24);//注意需要偏移24字节
		if(length2 > 0)
		{
		  length += length2;
		}
	}
	else
	{
	  length = TotalList*DataCollectLen[Type];
	}
	
	Public_ConvertLongToBuffer(length,VdrUsbReadBuffer+19);
	f_write (&file, VdrUsbReadBuffer, 23, &flen); 
	if(0 == flen)
	{
	    return 1;
	}
	
	if((length2 > 0)&&(VDR_DATA_TYPE_POSITION == Type))
	{
	  f_write (&file, VdrUsbReadBuffer+24, DataCollectLen[Type], &flen); 
		if(0 == flen)
	  {
	      return 1;
	  }
	}

	if(TotalList > 0)
	{
	  //for(i=0; i<TotalList; i++)
	  for(i=TotalList-1; i>=0; i--)
	  {
		  p = VdrUsbReadBuffer;
		  BufferLen = VDRData_ReadList(p,Type,i);//数据内容
		  if(0 == BufferLen)
		  {
			  p = VdrUsbReadBuffer;
			  for(j=0; j<DataCollectLen[Type]; j++)
			  {
				  *p++ = 0x00;
			  }
		  }
		  f_write (&file, VdrUsbReadBuffer, DataCollectLen[Type], &flen);
			if(0 == flen)
			{
				  i = 0;
			    break;
			}
	  }
 }

	if(1 == WriteEnableFlag)//当前写使能
	{
		VDRData_SetWriteEnableFlag(Type, 1);//使能写
	}
	
	return 0;
}
/**************************************************************************
//函数名：VDRUsb_SaveVerifyCode
//功能：写*.VDR文件中的校验码
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void VDRUsb_SaveVerifyCode(void)
{
	u32 flen;
    	u32 fsize; 
    	u32 freadnum;//文件大小 256的整数倍
    	u32 freadrem;//余数 
    	u32 faddr;
        FRESULT res;
        
    	u32 i;
    	u8  verf;//校验
    	u8  buf[260];

    	verf  = 0;
    	faddr = 0;

    	f_close(&file);//先关闭文件
    
    	if(f_open(&file,(WCHAR*)&filename,FA_READ|FA_WRITE) == FR_OK)
	{
		memset(buf,0,sizeof(buf));
      
    		fsize = f_size(&file);//获取文件大小
    		freadnum = fsize/256;
    		freadrem = fsize%256;

                //指向文件开始位置
                res = f_lseek(&file, 0);
                if(FR_OK !=res) 
                {
                  f_close(&file); 
                  return ; 
                }
                
    		for(i=0;i<freadnum;i++)//计算整数校验
    		{
        		f_read(&file,buf,256,&flen);
        		faddr += 256;
        		f_lseek(&file,faddr);
        		Vdr_Usb_XorVer(&verf,buf,256);
    		}

    		if(freadrem)//计算余数校验
    		{
        		f_read(&file,buf,freadrem,&flen);
        		faddr += freadrem;
        		f_lseek(&file,faddr);
        		Vdr_Usb_XorVer(&verf,buf,freadrem);
    		}
    
    		f_write (&file, &verf, 1, &flen);//写入校验
    		
	}             
}
/**
  * @brief  记录仪unicode编码文件名
  * @param  None
  * @retval None
  */
static void Vdr_Usb_UniNm(WCHAR *p_name)
{
    u8  tab[10];
    u8  plate[20];
    u8  len;
    u8  i;
    u16 tmp;
    WCHAR *pname = p_name;

    memset(plate,0,sizeof(plate));
    EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID,plate);//获取车牌号

    len = sizeof(CarPlate_Gb2312);
    tmp = (plate[0]<<8)|plate[1];
    for(i=0;i<len;i++)
    {
        if(tmp == CarPlate_Gb2312[i])
        {
            tmp = CarPlate_Unicode[i];//转为unicode
            break;
        }
    }
    
    if(strlen((char*)plate) == 0)//检查存储器
    {
        len = 0;
        i   = 0;
    }
    if(i == len)//检查车牌号
    {
        memcpy(pname,CarPlate_Unknown,sizeof(CarPlate_Unknown));
        pname += 4;
        memcpy(pname,CarPlate_ExNm,sizeof(CarPlate_ExNm));
			  pname += (sizeof(CarPlate_ExNm)/2);
        *pname = 0;
        return;
    }
    
    Public_ConvertNowTimeToBCDEx((u8*)&tab);
    *pname++ = 'D';
    *pname++ = (tab[0]>>4)   + 0x30;
    *pname++ = (tab[0]&0x0f) + 0x30;
    *pname++ = (tab[1]>>4)   + 0x30;
    *pname++ = (tab[1]&0x0f) + 0x30;
    *pname++ = (tab[2]>>4)   + 0x30;
    *pname++ = (tab[2]&0x0f) + 0x30;
    *pname++ = '_';
    *pname++ = (tab[3]>>4)   + 0x30;
    *pname++ = (tab[3]&0x0f) + 0x30;
    *pname++ = (tab[4]>>4)   + 0x30;
    *pname++ = (tab[4]&0x0f) + 0x30;  
    *pname++ = '_';
    *pname++ = tmp;
    
    len = strlen((char*)plate)-2;
    for(i=0;i<len;i++)
    {
        *pname++ = plate[2+i];
    }
    memcpy(pname,CarPlate_ExNm,sizeof(CarPlate_ExNm));
    pname += (sizeof(CarPlate_ExNm)/2);

    *pname = 0;
}
/**
  * @brief  记录仪unicode编码文件名
  * @param  None
  * @retval None
  */
void Vdr_Usb_UniNm2(WCHAR *p_name)
{
    u8  tab[10];
    u8  plate[20];
    u8  len;
    u8  i;
    u16 tmp;
    WCHAR *pname = p_name;

    memset(plate,0,sizeof(plate));
    EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID,plate);//获取车牌号

    len = sizeof(CarPlate_Gb2312);
    tmp = (plate[0]<<8)|plate[1];
    for(i=0;i<len;i++)
    {
        if(tmp == CarPlate_Gb2312[i])
        {
            tmp = CarPlate_Unicode[i];//转为unicode
            break;
        }
    }
    
    if(strlen((char*)plate) == 0)//检查存储器
    {
        len = 0;
        i   = 0;
    }
    if(i == len)//检查车牌号
    {
        memcpy(pname,CarPlate_Unknown,sizeof(CarPlate_Unknown));
        pname += 4;
        memcpy(pname,CarPlate_ExNm,sizeof(CarPlate_ExNm));
			  pname += (sizeof(CarPlate_ExNm)/2);
        *pname = 0;
        return;
    }
    
    Public_ConvertNowTimeToBCDEx((u8*)&tab);
    *pname++ = 'L';//与导出的记录仪数据文件相比只有这个字母不同
    *pname++ = (tab[0]>>4)   + 0x30;
    *pname++ = (tab[0]&0x0f) + 0x30;
    *pname++ = (tab[1]>>4)   + 0x30;
    *pname++ = (tab[1]&0x0f) + 0x30;
    *pname++ = (tab[2]>>4)   + 0x30;
    *pname++ = (tab[2]&0x0f) + 0x30;
    *pname++ = '_';
    *pname++ = (tab[3]>>4)   + 0x30;
    *pname++ = (tab[3]&0x0f) + 0x30;
    *pname++ = (tab[4]>>4)   + 0x30;
    *pname++ = (tab[4]&0x0f) + 0x30;  
    *pname++ = '_';
    *pname++ = tmp;
    
    len = strlen((char*)plate)-2;
    for(i=0;i<len;i++)
    {
        *pname++ = plate[2+i];
    }
    memcpy(pname,CarPlate_ExNm,sizeof(CarPlate_ExNm));
    pname += (sizeof(CarPlate_ExNm)/2);

    *pname = 0;
}
/**
* @brief  计算异或校验
* @param  *p:指向校验的数据；
* @param  len:校验数据的长度；
* @retval None
*/
static void Vdr_Usb_XorVer(u8 *pVer,u8 *p,u16 len)
{ 
    u16  i;
    
    for(i=0;i<len;i++)
    {
        *pVer ^= *(p+i);
    }
}







