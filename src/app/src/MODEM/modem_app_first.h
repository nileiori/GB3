/*
********************************************************************************
*
*                                        
*                          
*
*                        
*
* Filename      : modem_app_first.h
* Version       : V1.00
* Programmer(s) : miaoyahan
* @date         : 2014-10-28 
********************************************************************************
*/

/*
********************************************************************************
*                               MODULE
*
* Note(s) : 
********************************************************************************
*/
#include "my_typedef.h"
#include "modem_core.h"
#ifndef  MODEM_APP_FIRST_H
#define  MODEM_APP_FIRST_H


/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/


/*
********************************************************************************
*                            DEFINES VALUE
********************************************************************************
*/


/*
********************************************************************************
*                               DEFINES
********************************************************************************
*/

/*
********************************************************************************
*                                 ENUM
********************************************************************************
*/



/*
********************************************************************************
*                                STRUCT
********************************************************************************
*/



/*
********************************************************************************
*                          DEFINES FUNCTIONS
********************************************************************************
*/



/*
********************************************************************************
*                               FUNCTIONS
********************************************************************************
*/
APP_TYPE_VAL First_IP(void);

void Net_First_Run(void);
void Net_First_Init(void);
void Net_First_Open(void);
void Net_First_Close(void);
u16  Net_First_Send(u8 *p, u16 len);
u8   Net_First_ConSta(void);
void Net_First_ConTo(SOC_IPVAL ip_value, u16 time);
void First_connect_To_Specific_Server(void);
void First_Switch_Net_Specific_To_MainIP(void);
/*
********************************************************************************
*                               MODULE END
********************************************************************************
*/

#endif






