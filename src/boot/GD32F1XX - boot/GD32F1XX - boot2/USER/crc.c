
#include "stm32f10x.h"

	u32 ulCrc =0;

	void MOD_InitCRC(void)
	{
	ulCrc = 0xFFFFFFFF;
	}
	u32 MOD_CalcCRC(u8 *blockdata, u32 blocksize)
	{
	u32 j;
	u32 temp;
	u32 len = blocksize;
	u8 *data = blockdata;
	while (len--)
	{
	temp = (u32)*data++;
	for (j=32; j>0; j--)
	{
	if ((ulCrc ^ temp) & 0x80000000)
	ulCrc = 0x04C11DB7 ^ (ulCrc<<1);
	else
	ulCrc <<= 1;
	temp<<=1;
	}
	}
	return ulCrc;
	}
