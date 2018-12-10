#ifndef __A2_OBD_H__
#define __A2_OBD_H__

#include"include.h"


//A2OBD模块命令字
#define A2OBD_BPS               0x8100//设置波特率
#define A2OBD_HARD_VERSION      0x8200//硬件版本信息
#define A2OBD_SOFTE_VERSION     0x8201//软件版本信息
#define A2OBD_RESET							0x8300//软件复位
#define	A2OBD_ON_STREAM_DATA 		0x8500//数据流开关
#define	A2OBD_OFF_STREAM_DATA 	0x8501//数据流开关
#define A2OBD_PRA_SET           0x8700//数据设置
#define A2OBD_PRA_READ          0x8701//参数查询
#define A2OBD_STREM_DATA        0x8800//实时数据流
#define A2OBD_ALARM							0x8B00//报警输出
#define	A2OBD_ROUTE_END					0x8A00//行程数据结束
#define A2OBD_ROUTE_BEGIN       0x8A01//行程开始

void A2OBD_UartDataProtocolParse(unsigned char *pBuffer,unsigned short Len);
//A2OBD数据流
typedef struct A2OBD_STREAM_DATA
{
	int Speed;//车速
	int Rpm;//转速
	int Coot;//水温
	int Load;//负荷
  int Volt;//电瓶电压	
	int OilPosition;//油门踏板位置
	int ClutchState;//离合状态
	int BrakeState;//刹车状态
	int CurON;//瞬时油耗（L/H）
	int FuelKilometer;//百公里油耗（L/100KM）
	int EngineFuelPressure;//机油压力
	int FuelRailPressure;//实际油轨压力
	int ExhaustGasAlarm;//废气报警信号灯
	int OBD_State;//OBD模块状态
}A2OBD_StreamData;
//A2OBD行程数据
typedef struct A2OBD_ROUTE_DATA
{
	int IdlingTime;//怠速时间 单位1s
	int DriveTime;//行驶时间 单位1s
	int HighBrakeTime;//最高档时间 单位1s
	int RouteTime;//行程时间 单位1s
  int ECU_TatolFuel;//ECU总耗油 单位0.01L
	int IdlingFuel;//怠速油耗 单位0.01L
	int SlideFuel;//滑行节约油耗 单位0.01L
	int GunFuel;//大油门油耗 单位0.01L
	int HighBrakeFuel;//最高档油耗 单位0.01L
	int RouteFuel;//行程油耗 单位0.01L
	int ECU_Mileage;//ECU总里程 单位0.01Km
	int BrakeMileage;//制动里程 单位0.01Km
	int RouteMileage;//行程里程 单位0.01Km
	int IdlingBrakeMileage;//空挡滑行里程 单位0.01Km
	int GunMileag;//大油门里程 单位0.01Km
	int HighBrakeMileage;//最高档里程 单位0.01Km
	int OverSpeedMileage;//超速里程 单位0.01Km
	int SlideMileage;//带档滑行里程 单位0.01Km
	int OverRpmMileage;//超经济转速里程 单位0.01Km
	int AverageRpm;//平均转速 单位0.01RPM
	int AverageGasPedal;//平均油门  单位0.01%
	int AverageLoad;//平均发动机负荷 单位0.01%
	int AverageSpeed;//平均车速 单位0.01km/h
	int HighSpeed;//最高车速 单位0.01km/h
	int HighRpm;//最高转速 单位0.01RPM
	int EcoRpmMileage;//经济转速里程 单位 0.01km
	int GunTimes;//大油门次数 
	int EmerBrakeTimes;//急刹车次数
	int BrakeTimes;//刹车次数
	int OverSpeedTimes;//超速次数
	int ClutchTimes;//离合使用次数
	int AverageFuel;//平均油耗（L/100km）
	int StartTimes;//起步次数
	int OnlyAverageRpm;//净平均转速
	int OnlyAverageGasPedal;//净平均油门
	int OnlyAverageLoad;//净平均负荷
	int OnlyAverageSpeed;//净平均车速
	int OnlyAverageFuel;//净平均油耗（L/100km）
	int EmerTurnTime;//急转向次数
	int EmerGasPedal;//急踩油门次数
	int NoGearTimes;//换挡不及时次数
	int InclineAlarmTimes;//倾斜报警次数
	int BadlyBumpTimes;//严重颠簸次数
	int ThreeViolateTimes;//3s违例次数
	int OverTimeIdlingTimes;//长时间怠速次数
	int UnderVoltageAlarmTimes;//电压过低报警次数
}A2OBD_RouteData;
//A2OBD报警状态
typedef enum
{
	EMER_GAS_PEDAL=1,//急踩油门
	EMER_BRAKE,//急踩刹车
	IDLING_SLIDE,//空挡滑行
	OVER_SPEED,//超速
	NO_GEAR,//换挡不及时
	UNDER_ENGINGE_FUEL,//机油压力过低
	HIGEH_COOT,//发动机水温过高
	OVER_TIME_GEAR,//长时间踩离合
	UNDER_VOLTAGE,//电压过低
	TROUBLE_DRIVE,//带故障行车
	ACC_OFF_SLIDE,//熄火滑行报警
	CRASH_PRE,//碰撞预警
	CRASH,//碰撞报警
	EMER_TURN,//急转向
	OVER_TIME_IDLING,//长时间怠速
	INCLINE,//倾斜
	ROLLOVER,//侧翻
	SMALL_BUMP,//轻微颠簸
	MEDIUM_BUMP,//中等颠簸
	BADLY_BUMP,//严重颠簸
}A2OBD_Alarm;//状态

void A2OBD_SentCmd(u16 cmd,u8 *pBuffer,u16 datalen);
void A2OBD_Protocol_Paramter(u8 cmd,u8 *pBuffer,u8 datalen);
void A2OBD_UartDataProtocolParse(unsigned char *pBuffer,unsigned short Len);
u16  ThreeEmerInfo(u8 *pDst);
FunctionalState A2OBD_TimeTask(void);
#endif
