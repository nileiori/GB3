//AD键盘扫描函数
//AD键盘的硬件原理分析，根据不同的AD采样值，建立对应的AD键盘关系。
//每隔电阻的偏差值是+-5%
#define     _EXT_KEY_VOL_ 

#include "include.h"
//#include "Adc.h"
#include "adc_tran_voltag.h"
#include "adc_key.h"

#define     CONST_OFFSET    100
/**********************************************************************************/
/*function  name:           void  KeyBoardAdc_TimeTask(void)        */
/*input parameter:          none  */
/*output parameter:         none */
/*FUNCTION:             键盘扫描函数 定时100mS调用
*/
/**********************************************************************************/
FunctionalState KeyBoardAdc_TimeTask(void)
{
   uint value,key_tmp;
    value=Adc_GetKeyValue();
    key_tmp=Ad_GetValue(ADC_KEY);
      //u16 value; //电压值
  
  // value=Ad_GetValue(KEY_CHANNEL);
   if(key_tmp>value)
        {
            if(key_tmp-value>CONST_OFFSET)
                {
                    key_code=   KEY_NONE;
                    
                    return ENABLE;
                }
            }
    else
         if(value-key_tmp>CONST_OFFSET)
            {
                    key_code=   KEY_NONE;
                    
                    return ENABLE;
                }  
         
    //无键按下
    if(value>KEY_NONE_MIN)
        key_code=   KEY_NONE;
    else    //键1按下
        if(value>KEY1_ADC_MIN&&value<KEY1_ADC_MAX)
            key_code=   KEY_VALUE1;
    else    //键2按下
        if(value>KEY2_ADC_MIN&&value<KEY2_ADC_MAX)
            key_code=   KEY_VALUE2;
    else    //键3按下
        if(value>KEY3_ADC_MIN&&value<KEY3_ADC_MAX)
            key_code=   KEY_VALUE3;     
    else    //键4按下
        if(value>KEY4_ADC_MIN&&value<KEY4_ADC_MAX)
            key_code=   KEY_VALUE4; 
    else    //错误数据
        key_code=   KEY_NONE;//KEY_ERR0;
    
    return ENABLE;
}
//返回具体键盘的虚拟键值
uchar KeyValue_Read(void)
{
    static uchar key;

    if(key!=key_code)
    {
        key=key_code;
                
                return  KEY_NONE;
    }
    else
    {
        return key;

        }
}

/*******************************************************************************
* Function Name  : 按键电压
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u16 GetKeyV(void) 
{
    u16 Voltage =0;
  // Voltage = DriveAdc_getVar(ADC_KEY);
  //  Voltage = Voltage *330/0xfff;
    return Voltage;
}

