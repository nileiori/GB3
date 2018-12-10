/*******************************************************************************
 * File Name:           BMA250.h 
 * Function Describe:   
 * Relate Module:       ��ײ\�෭����
 * Writer:              Joneming
 * Date:                2014-07-16
 * ReWriter:            
 * Date:                
 *******************************************************************************/
//��ײ������BMA250E
//��Ӳ����Ʋ�ȡIICģʽ����ͨѶ�����������ϱ��뱣��ѡ��IICģʽ
#include"include.h"
//////////////////////////
#define USE_ACC_RANGE   ACC_RANGE_4G//��ǰʹ�õ�����
////////////////////////
////////////////////////////////////////
//���ٶȷ�Χ��
#define ACC_RANGE_2G         3
#define ACC_RANGE_4G         5
#define ACC_RANGE_8G         8
#define ACC_RANGE_16G       12

//�˲���ΧƵ�� HZ�� 
#define FILTER_1000HZ           15
#define FILTER_500HZ            14
#define FILTER_250HZ            13
#define FILTER_125HZ            12
#define FILTER_62HZ             11
#define FILTER_31HZ             10
#define FILTER_15HZ             9
#define FILTER_7HZ              8                                                                        

//ʹ��X,Y,Z�ᡣ
#define EN_HIGH_X       1
#define EN_HIGH_Y       2
#define EN_HIGH_Z       4

//���ٶȴ�����BMA250E���ڲ��Ĵ����ֲ���
#define BMA250_DEVICE_ID             0x00  //����ID��
#define BMA250_ACCEL_X_L             0x02  
#define BMA250_ACCEL_X_H             0x03  //X����ٶȡ�
#define BMA250_ACCEL_Y_L             0x04
#define BMA250_ACCEL_Y_H             0x05  //Y����ٶȡ�
#define BMA250_ACCEL_Z_L             0x06  
#define BMA250_ACCEL_Z_H             0x07  //Z����ٶȡ�**_L>>6+**_H<<2
#define BMA250_TEMP                  0x08  //TEMPֵ��
#define BMA250_FOTS_INT              0x09  //BMA250E��ƽ̹�����򡢶�����б���жϡ�
#define BMA250_DATA_FIFO_INT         0x0A  //BMA250E�����ݡ�FIFO�жϡ�
#define BMA250_TAP_SLOPE_INT         0x0B  //BMA250E�Ķ�����б�ʡ�
#define BMA250_FLAT_ORI_HIGH_INT     0x0C  //BMA250E��ƽ̹�������XYZ�Ķ��塣
#define BMA250_FIFO                  0x0E  //BMA250E��FIFO���塣
#define BMA250_ACC_RANGE             0x0F  //BMA250E��ACCELERATION ��Χ��
#define BMA250_FILTER_BW             0x10  //BMA250E���˲�������
#define BMA250_POWER_SET             0x11  //BMA250E�ĵ�Դ����ģʽ���á�
#define BMA250_POWER_MODE            0x12  //BMA250E�ĵ�Դ����ģʽ��
#define BMA250_DATASHADOW            0x13  //BMA250E�ļ��ٶ����ݴ洢��ʽ��shadow��8bit��2bit��ϳ�һ��10bit�����ݣ���
#define BMA250_SOFTRESET             0x14  //BMA250E�������λ��
#define BMA250_FOTS_EN               0x16  //BMA250E��ƽ̹�����򡢶�����б�ʵ�ʹ�ܡ�
#define BMA250_DATA_FIFO_EN          0x17  //BMA250E��FIFO�����ݺ�XYZ��ʹ�ܡ�
#define BMA250_SLOPE_EN              0x18  //BMA250E��б�ʼ���ʹ�ܡ�
#define BMA250_FOTS_INT1             0x19  //BMA250E��ƽ̹�����򡢶�����б���ж�1��
#define BMA250_DATA_FIFO_INT1_2      0x1A  //BMA250E�����ݺ�FIFO���ж�1��2��
#define BMA250_FOTS_INT2             0x1B  //BMA250E��ƽ̹�����򡢶�����б���ж�2��
#define BMA250_SRC                   0x1E  //BMA250E��SRC�жϡ�
#define BMA250_OD_v_INT              0x20  //BMA250E��OD��|v|�жϡ�
#define BMA250_RESET_LATCH_INT       0x21  //BMA250E�ĸ�λ�������жϡ�
#define BMA250_LOW_DUR               0x22  //BMA250E��LOW�ļ����
#define BMA250_LOW_TH                0x23  //BMA250E��LOW����ֵ��
#define BMA250_HL_HY                 0x24  //BMA250E��HIGH��LOW��HY��
#define BMA250_HIGH_DUR              0x25  //BMA250E��HIGH�ļ����
#define BMA250_HIGH_TH               0x26  //BMA250E��HIGH����ֵ��
#define BMA250_SLOPE_NOMOTION_DUR    0x27  //BMA250E��SLOPE�ļ����
#define BMA250_SLOPE_TH              0x28  //BMA250E��SLOPE����ֵ��
#define BMA250_SLOPE_NOMOTION_TH     0x29  //BMA250E��SLOPE_NOMOTION����ֵ��
#define BMA250_TAP_DUR               0x2A  //BMA250E��TAP�ľ�ֹ���𶯺ͼ����
#define BMA250_TAP_TH                0x2B  //BMA250E��TAP�ĳ�������ֵ��
#define BMA250_ORI_HBM               0x2C  //BMA250E�ķ����HY�����������ģʽ��
#define BMA250_ORI_EN_THETA          0x2D  //BMA250E�ķ����ʹ�ܼ�theta�ǡ�
#define BMA250_FLAT_THETA            0x2E  //BMA250E��ƽ̹��theta�ǡ�
#define BMA250_FLAT_HT_HY            0x2F  //BMA250E��ƽ̹�ı���ʱ�估HY��
#define BMA250_FIFO_WMARK            0x30  //BMA250E��FIFO��ˮӡ��
#define BMA250_SELFTEST_SET          0x32  //BMA250E���Բ����á�
#define BMA250_NVM_SET               0x33  //BMA250E��NVM���á�
#define BMA250_IICSPI_WDT_SET        0x34  //BMA250E��IIC��SPI���á�
#define BMA250_OFFSET_CAL_HP         0x36  //BMA250E��ƫ�ơ����㼰XYZʹ�ܡ�
#define BMA250_OFFSET_TARGET_XYZ     0x37  //BMA250E��XYZ��OFFSET TARGET�����á�
#define BMA250_OFFSET_X_SET            0x38  //BMA250E��X��OFFSET���á�
#define BMA250_OFFSET_Y_SET            0x39  //BMA250E��Y��OFFSET���á�
#define BMA250_OFFSET_Z_SET            0x3A  //BMA250E��Z��OFFSET���á�    
#define BMA250_GP0                   0x3B  //BMA250E��GPO���á�
#define BMA250_GP1                   0x3C  //BMA250E��GP1���á�
#define BMA250_FIFO_MODE_DATA        0x3E  //BMA250E��FIFO��ģʽ������ѡ��
#define BMA250_FIFO_DATAREG          0x3F  //BMA250E��FIFO����������Ĵ�����
////////////////////////////////
////////////////////////////
#define BMA250_G_SENSOR_ADDR       (0x18<<1)      //BMA250��ַ
////////////////////////
enum 
{
    BMA_TYPE_X,                 
    BMA_TYPE_Y,               
    BMA_TYPE_Z,
    BMA_TYPE_MAX
}E_BMA_TYPE;
////////////////////////
typedef struct 
{
    signed short angle[BMA_TYPE_MAX];     //�Ƕ�    
    unsigned char saveflag;   //
    unsigned char bak;//
}ST_BMA250_INIT;

typedef struct 
{
    float angle[BMA_TYPE_MAX];//�Ƕ�
    float anglelimit;//�Ƕ�
    float gravity;//1G��Ӧ��ֵ
    unsigned char status;     //״̬
    unsigned char setCnt;
    unsigned char resetCnt;
    unsigned char range;      //
}ST_BMA250_ATTRIB;

static ST_BMA250_ATTRIB s_stBmaAttrib;
///////////////////////////////////
#define BMA_PI              3.14159265358979
#define BMA_2_PI            6.28318530717958

const unsigned short c_usGravity[]={256,128,64,32};

#define BMA250_MIN(val,tmp) (val>tmp)?tmp:val
#define BMA250_MAX(val,tmp) (val>tmp)?val:tmp

//////////////////////////////
//��ȡ�жϱ�־ ��how many interrupts in total)
unsigned char Bma250_GetInterruptFlag_A(void);
unsigned char Bma250_GetInterruptFlag_B(void);
unsigned char Bma250_GetInterruptFlag_C(void);
unsigned char Bma250_GetInterruptFlag_D(void);
////////////////////////////
static void (*BMA250_WriteData)(unsigned char addr,unsigned char value);//
static unsigned char (*BMA250_ReadData)(unsigned char addr);//
static unsigned char (*BMA250_GetStatus)(void);//
////////////////////////

///////////////////////////////////
/*************************************************************
** ��������: BMA250_I2C_WriteData
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_I2C_WriteData(unsigned char addr,unsigned char value)
{
    I2C_WriteDat(addr,value);
}
/*************************************************************
** ��������: BMA250_I2C_ReadData
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char BMA250_I2C_ReadData(unsigned char addr)
{
    unsigned char temp;    
    I2C_ReadDat(addr,&temp);    
    return temp;
}

/*************************************************************
** ��������: BMA250_E2_WriteData
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_E2_WriteData(unsigned char addr,unsigned char value)
{
    s_stBmaAttrib.status = E2_WriteOneByte(value, BMA250_G_SENSOR_ADDR, addr);    
}
/*************************************************************
** ��������: BMA250_E2_ReadData
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char BMA250_E2_ReadData(unsigned char addr)
{
    unsigned char temp;
    s_stBmaAttrib.status = E2_ReadOneByte(&temp,BMA250_G_SENSOR_ADDR,addr); 
    return temp;
}
/*************************************************************
** ��������: Bma250_GetInterruptFlag_D
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char BMA250_E2_GetStatus(void)
{
    return (s_stBmaAttrib.status)?0:1;
}
/*************************************************************
** ��������: BMA250_Delay
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_Delay(unsigned short m)
{
    unsigned short i;
    for(i=0;i<m;i++);
}

/*************************************************************
** ��������: BMA250_ReadAccelValue
** ��������: X/Y/Z����ٶ�ֵ
            ���ݼ��ٶ��������÷�Χ��+-2G/+-4G/+-8G/+-16G,��Ӧ��Gravity_Acc_1GΪ16LSB/8LSB/4LSB/2LSB��
            ��ȡ���ٶ�ֵ
            0-->+31;0-->+2G
            0-->-32;0-->-2G
            ���ٶȼĴ�����ַ���䣺ACC_X-->0x02(L2),0x03(H8)����H8�ŵ�16bits��L8��(�����з���λ)�����Ŷ�16bits������λ��Ȼ��16bits������L2��ӣ������͵õ���10bits��X����ٶ�ֵ��
            ACC_Y-->0x04(L2),0x05(H8)����H8�ŵ�16bits��L8��(�����з���λ)�����Ŷ�16bits������λ��Ȼ��16bits������L2��ӣ������͵õ���10bits��Y����ٶ�ֵ��
            ACC_Z-->0x06(L2),0x07(H8)����H8�ŵ�16bits��L8��(�����з���λ)�����Ŷ�16bits������λ��Ȼ��16bits������L2��ӣ������͵õ���10bits��Z����ٶ�ֵ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
float BMA250_ReadAccelValue(unsigned char type)
{
    unsigned char data[2];
    short sval;                   //�������������ͱ���temp1��temp2
    unsigned char accel_L,accel_H;
    if(BMA_TYPE_X==type)
    {
        accel_L=BMA250_ACCEL_X_L;
        accel_H=BMA250_ACCEL_X_H;
    }
    else
    if(BMA_TYPE_Y==type)
    {
        accel_L=BMA250_ACCEL_Y_L;
        accel_H=BMA250_ACCEL_Y_H;
    }
    else
    {
        accel_L=BMA250_ACCEL_Z_L;
        accel_H=BMA250_ACCEL_Z_H;
    }
    data[0]=BMA250_ReadData(accel_L);            //��ȡX����ٶ�ֵ�ĵ���λ
    data[1]=BMA250_ReadData(accel_H);             //��ȡX����ٶ�ֵ�ĸ߰�λ
    sval = data[0]>>6;//��X����ٶ�ֵ�ĵ���λ������λ
    sval |=data[1]<<2;//��X����ٶ�ֵ�ĸ߰�λ������λ
    sval = sval << 6;//����з���
    sval = sval >> 6;//����з���
    return (float)sval;                                   //���ش�����X����ٶ�ֵ
}

/*************************************************************
** ��������: Bma250_GetInterruptFlag_D
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Bma250_GetInterruptFlag_A(void)
{
    unsigned char temp;                                               //�������temp
    temp=BMA250_ReadData(BMA250_FOTS_INT);  //��ȡ�ж��¼�A�ı�־λ
    return temp;                                                  //�����ж��¼�A�ı�־λ
}
/*************************************************************
** ��������: Bma250_GetInterruptFlag_D
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Bma250_GetInterruptFlag_B(void)
{
    unsigned char temp;                                               //�������temp
    temp=BMA250_ReadData(BMA250_DATA_FIFO_INT); //��ȡ�ж��¼�B�ı�־λ
    return temp;                                                      //�����ж��¼�B�ı�־λ
}
/*************************************************************
** ��������: Bma250_GetInterruptFlag_D
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Bma250_GetInterruptFlag_C(void)
{
    unsigned char temp;                                               //�������temp
    temp=BMA250_ReadData(BMA250_TAP_SLOPE_INT); //��ȡ�ж��¼�C�ı�־λ
    return temp;                                                      //�����ж��¼�C�ı�־λ
}
/*************************************************************
** ��������: Bma250_GetInterruptFlag_D
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Bma250_GetInterruptFlag_D(void)
{
    unsigned char temp;                                                  //�������temp
    temp=BMA250_ReadData(BMA250_FLAT_ORI_HIGH_INT);//��ȡ�ж��¼�D�ı�־λ
    return temp;                                                         //�����ж��¼�D�ı�־λ
}

/*************************************************************
** ��������: BMA250_ReadAndSetCrashAlarmParam
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_ReadAndSetCrashAlarmParam(void)                                      
{
    unsigned char buf[4];
    unsigned char temp;
    unsigned char crashTime,accelerate;
    float fval;    
    if(EepromPram_ReadPram(E2_SET_CRASH_ALARM_ID,buf))
    {
        crashTime = buf[1];
        accelerate = buf[0];
        if(crashTime==0||crashTime>8)
        {
            crashTime=4;
        }
        ////////////////////
        if(accelerate==0||accelerate>s_stBmaAttrib.range)
        {
            accelerate = s_stBmaAttrib.range;            //���ٶ�����ѡ����+-4G,'20'��ʾ20*0.1(2),��4G��һ�롣
        }
    }
    else
    {
        crashTime = 4;
        accelerate = s_stBmaAttrib.range;
    }
    //----------------------------��ײ������������--------------------------------------------
    temp=(crashTime/2)-1;                 //((crashTime*4)/8)-1;     '4'��ʾƽ̨���øò�����LSB,'8'��ʾBMA250Eѡ��BW=62HZ��SLOPE_DURATION��LSB,'-1'����DURATION�ļ�����ƾ����ġ�
    BMA250_WriteData(BMA250_SLOPE_NOMOTION_DUR,temp);//����ʱ����Ϊtemp�������

    fval=accelerate;//��λ0.1g
    fval=fval/10.0;//�ж���1g
    fval=fval*s_stBmaAttrib.gravity;//��Ӧ������
    temp=(unsigned char)(fval-1); 
    BMA250_WriteData(BMA250_SLOPE_TH,temp);         //������ֵΪ
}

/*************************************************************
** ��������: BMA250_ReadAndSetRollOverAlarmParam
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_ReadAndSetRollOverAlarmParam(void)                                      
{
    unsigned char buf[5];
    unsigned short angle,i;
    ST_BMA250_INIT stBmaInit;
    if(EepromPram_ReadPram(E2_SET_ROLLOVER_ALARM_ID,buf))
    {
        angle=Public_ConvertBufferToShort(buf);
        if(angle==0||angle>180)
        {
            angle=30;
        }
    }
    else
    {
        angle = 30;
    }
    //////////////
    Public_ReadDataFromFRAM(FRAM_ROLL_OVER_INIT_ANGLE_ADDR, (unsigned char *)&stBmaInit, sizeof(ST_BMA250_INIT));
    if(0x55!=stBmaInit.saveflag)
    {
        memset(&stBmaInit,0,sizeof(ST_BMA250_INIT));
        stBmaInit.angle[0] = (signed short)(BMA_PI*5000.0);
        stBmaInit.angle[1] = (signed short)(BMA_PI*5000.0);
        stBmaInit.angle[2] = 0; //       
    }
    /////////////////////
    s_stBmaAttrib.anglelimit = angle/180.0*BMA_PI;
    ////////////////////
    for(i = BMA_TYPE_X;i<= BMA_TYPE_Z; i++)
    {
        s_stBmaAttrib.angle[i] = stBmaInit.angle[i]/10000.0;//��׼��
    }
    //////////////////////
}
/*************************************************************
** ��������: BMA250_RollOverInitAngleCalibration
** ��������: �෭��ʼ�Ƕ�У׼
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
signed short BMA250_GetAngleCalibration(unsigned char type)
{
    float temp,min,max;
    float sum;
    unsigned char i;
    sum = 0.0 ;
    min = 512;
    max =-512;
    for(i=0; i<10; i++)
    {
        temp = BMA250_ReadAccelValue(type); //��ȡZ����ٶ�ֵ
        min = BMA250_MIN(min,temp);
        max = BMA250_MAX(max,temp);
        sum += temp; //��ȡZ����ٶ�ֵ        
    }  
    sum=sum-max-min;
    ///////
    temp=(sum/8.0);
    /////////////
    temp =temp/s_stBmaAttrib.gravity;
    if(temp>1.0)
        temp = 1.0;
    else 
    if(temp<-1.0)
        temp = -1.0;
    
    return (signed short)(acos(temp)*10000.0);
}
/*************************************************************
** ��������: BMA250_RollOverInitAngleCalibration
** ��������: �෭��ʼ�Ƕ�У׼
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char BMA250_RollOverInitAngleCalibration(void)
{
    unsigned char i;
    ST_BMA250_INIT stBmaInit;
    ///////////////
    BMA250_ReadAccelValue(BMA_TYPE_Z); //��ȡZ����ٶ�ֵ
    ///////////////////
    if(BMA250_GetStatus())
    {
        return 0;
    }
    /////////////////
    memset(&stBmaInit,0,sizeof(ST_BMA250_INIT));
    //////////////////
    for(i = BMA_TYPE_X; i <= BMA_TYPE_Z; i++)
    {
        stBmaInit.angle[i] = BMA250_GetAngleCalibration(i);
    }
    ////////////////////
    stBmaInit.saveflag = 0x55;
    //////////////////
    Public_WriteDataToFRAM(FRAM_ROLL_OVER_INIT_ANGLE_ADDR, (unsigned char *)&stBmaInit, sizeof(ST_BMA250_INIT));
    ////////////////
    BMA250_ReadAndSetRollOverAlarmParam();
    return 1;
}
/*************************************************************
** ��������: BMA250_GetAccRange
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_GetAccRange(void)
{
    unsigned char val,index,range;
    val=BMA250_ReadData(BMA250_ACC_RANGE);  //ѡ����ٶȷ�ΧΪ+-4G,���ѡ��+-2G��+-8G��+-16G��
    if(ACC_RANGE_16G == val)
    {
        index = 3;
        range = 8;//
    }
    else
    if(ACC_RANGE_8G==val)
    {
        index = 2;
        range = 4;//
    }
    else
    if(ACC_RANGE_4G==val)
    {
        index = 1;
        range = 2;//
    }
    else
    {
        index = 0;
        range = 1;//
    }

    s_stBmaAttrib.gravity = (float)c_usGravity[index];
    s_stBmaAttrib.range = (range *10);//��λ0.1g
}
/*************************************************************
** ��������: BMA250_UpdataSetParam
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_UpdataSetParam(void)
{    
    BMA250_WriteData(BMA250_SOFTRESET,0xB6);
    BMA250_Delay(1000);
    BMA250_Delay(1000);
    BMA250_WriteData(BMA250_OFFSET_CAL_HP,0x80);        //����Ư�Ʋ���Ŀ��ֵ��
    BMA250_WriteData(BMA250_IICSPI_WDT_SET,6);        //ʹ�ܿ��Ź���ʱ��������Ϊ50ms����4   ����Ϊ1ms��
    BMA250_WriteData(BMA250_ACC_RANGE,USE_ACC_RANGE);   //ѡ����ٶȷ�ΧΪ+-4G,���ѡ��+-2G��+-8G��+-16G��
    BMA250_WriteData(BMA250_FILTER_BW,FILTER_62HZ);   //�����˲�������62Hz    
    BMA250_WriteData(BMA250_FOTS_EN,0xf7);          //ʹ��XYZ��������ļ��ٶȼ��,slopexyz_en(SET)��
    BMA250_WriteData(BMA250_DATA_FIFO_EN,0x07);     //ʹ��XYZ���������HIGH-Gģʽ��
    //BMA250_WriteData(BMA250_OFFSET_CAL_HP,0x60);      //����Ư�Ʋ���Ŀ��ֵ��//��ײ
    //BMA250_WriteData(BMA250_OFFSET_TARGET_XYZ,0x20);  //����Z��Ϊ����Ŀ�ꡣ
    BMA250_WriteData(BMA250_RESET_LATCH_INT,0x80);  //ʹ�ܸ�λ�жϡ�
    BMA250_WriteData(BMA250_OFFSET_Z_SET,0x00);     //ʹ��ƽ̹�����򡢶�������б�ж�
    //BMA250_WriteData(BMA250_NVM_SET,0x0B);
    //BMA250_WriteData(BMA250_HL_HY,0x80);              //����high_hyΪ2��
    /////////////////////////
    BMA250_GetAccRange();
    //////////////////////
    BMA250_ReadAndSetCrashAlarmParam();
    ///////////////
    BMA250_ReadAndSetRollOverAlarmParam();    
}
/*************************************************************
** ��������: BMA250_ParamInitialize
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_ParamInitialize(void)
{
    ///////////////////
    memset(&s_stBmaAttrib,0,sizeof(s_stBmaAttrib));
    ////////////////////
//    if(ICCardBase_GetUseIntFlag()) dxl,2015.9,
    if(1)
    {
        BMA250_WriteData = BMA250_E2_WriteData;
        BMA250_ReadData  = BMA250_E2_ReadData;
        BMA250_GetStatus = BMA250_E2_GetStatus;
        if(ICCARD_M3_I2C_CheckDevice( BMA250_G_SENSOR_ADDR ) == 0)
        {
            BMA250_Delay(20000);
        }
        ///////////////
        if(ICCARD_M3_I2C_CheckDevice( BMA250_G_SENSOR_ADDR ) == 0)
        {
            BMA250_Delay(20000);
        }
        //LOG_PR_N("ʹ�� E2 \r\n"); 
    }
    
    else
    {
        BMA250_WriteData = BMA250_I2C_WriteData;
        BMA250_ReadData  = BMA250_I2C_ReadData;
        BMA250_GetStatus = I2c_GetStatus;
        //////////////////////////
        if(!I2C_Config())
        {
            BMA250_Delay(20000);
            //////////
            if(!I2C_Config())
            {
                if(!I2C_Config());
            }
        } 
        //LOG_PR_N("ʹ�� I2C \r\n"); 
    }   
    
    //////////////////////
    BMA250_Delay(20000);
    BMA250_Delay(20000);
    ///////////////////////
    BMA250_UpdataSetParam();
}
/*************************************************************
** ��������: BMA250_CheckCrashAlarm
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
float BMA250_GetAngleDifference(unsigned char type)
{
    float temp,angle,base;
    base=s_stBmaAttrib.angle[type];
    temp=BMA250_ReadAccelValue(type); //��ȡZ����ٶ�ֵ    
    //////////////
    if(BMA250_GetStatus())
    {
        return 0.0;
    }
    //////////////////    
    temp = temp/s_stBmaAttrib.gravity;
    if(temp>1.0)
    {
        temp=1.0;
    }
    else
    if(temp<-1.0)
    {
        temp=-1.0;
    }
    //////////////////
    angle=acos(temp);//����Ƕ�
    angle=fabs(angle-base);
    if(angle>BMA_PI)
    {
        angle=fabs(BMA_2_PI-angle);
    }
    return angle;
}
/*************************************************************
** ��������: BMA250_CheckCrashAlarm
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_CheckCrashAlarm(void)
{
    float sign[BMA_TYPE_MAX];                   //�������
    unsigned char i;
    ////////////
    BMA250_ReadAccelValue(BMA_TYPE_Z); //��ȡZ����ٶ�ֵ
    ///////////////
    for(i = BMA_TYPE_X;i<= BMA_TYPE_Z; i++)
    {
        sign[i] = BMA250_GetAngleDifference(i);
    }
    ////////////////
    if(BMA250_GetStatus())
    {
        return;
    }
    ////////////////////////////
    if(sign[BMA_TYPE_X]>=s_stBmaAttrib.anglelimit||sign[BMA_TYPE_Y]>=s_stBmaAttrib.anglelimit||sign[BMA_TYPE_Z]>=s_stBmaAttrib.anglelimit)
    {
        s_stBmaAttrib.resetCnt = 0;
        if(s_stBmaAttrib.setCnt < 4)
        {
            s_stBmaAttrib.setCnt++;
        }
        else
        {
            Io_WriteAlarmBit(ALARM_BIT_SIDE_TURN_PRE_ALARM,SET);
        }
    }
    else
    {
        s_stBmaAttrib.setCnt = 0;        
        if(s_stBmaAttrib.resetCnt < 4)
        {
            s_stBmaAttrib.resetCnt++;
        }
        else
        {
            Io_WriteAlarmBit(ALARM_BIT_SIDE_TURN_PRE_ALARM,RESET);
        }
    }
}
/*************************************************************
** ��������: BMA250_CheckAccelOver_App
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_CheckAccelOver_App(void)
{
    unsigned char temp;
    static unsigned char num=0;
    static unsigned char semph=0;
    
    temp=Bma250_GetInterruptFlag_A();
    if(temp&4)
    {
        Io_WriteAlarmBit(ALARM_BIT_IMPACT_PRE_ALARM,SET);//���淢����ײ��  
        if(!semph)
        {
            //SetEvTask(EV_REPORT);
            Report_UploadPositionInfo(CHANNEL_DATA_1);
            Report_UploadPositionInfo(CHANNEL_DATA_2);
        }
        num=0;
        semph=true;                                                                                             
    }
    else
    {
        num++;
        if(num>100)
        {
            Io_WriteAlarmBit(ALARM_BIT_IMPACT_PRE_ALARM,RESET);//��ײ��־λ��λ��
        }
        semph=false;
    }
}

/*************************************************************
** ��������: BMA250_UpdataCrashAlarmParam
** ��������: ������ײ������־λ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_UpdataCrashAlarmParam(void)
{
    BMA250_ReadAndSetCrashAlarmParam();
}
/*************************************************************
** ��������: BMA250_UpdataRollOverAlarmParam
** ��������: ���²෭������־λ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_UpdataRollOverAlarmParam(void)
{
    BMA250_ReadAndSetRollOverAlarmParam();
}

/*************************************************************
** ��������: BMA250_UpdataSetParam
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void BMA250_ShowDebugInfo(void)
{ 
    #if 0
    float temp[BMA_TYPE_MAX];
    unsigned char val,val1;
    char buffer[100];
    unsigned char i;
    for(i=0;i<BMA_TYPE_MAX; i++ )
    temp[i]=BMA250_ReadAccelValue(i); //��ȡZ����ٶ�ֵ

    val = BMA250_ReadData(BMA250_ACC_RANGE);

    val1=BMA250_ReadData(BMA250_SLOPE_TH);
    //////////////
    sprintf(buffer,"x=%5.2f;y=%5.2f;z=%5.2f  range =%d th =%d \r\n",temp[0],temp[1],temp[2],val,val1);
    LOG_PR_N(buffer);
    #endif
}
/*************************************************************
** ��������: BMA250_TimeTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState BMA250_TimeTask(void)
{
    /* dxl,2015.9,
    if(ICCardBase_GetUseIntFlag())
    {
        if(ICCardBase_GetReadFlag())return ENABLE;        
    }
    */
    ////////////////////
    BMA250_CheckCrashAlarm();//��б���
    ///////////
    BMA250_CheckAccelOver_App(); //��ײ���
    /////////////////////
    //BMA250_ShowDebugInfo();
    
    return ENABLE;
}
            
/******************************************************************************
**                            End Of File
******************************************************************************/
                                                                                  
