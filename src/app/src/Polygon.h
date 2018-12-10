/*****************************************************************************
*Function Name:  Polygon.h
*Input  Parameter:  None
*Output Parameter:  None 
*Function Describtion:Polygon area recognition 
*Writer:mark
*Update Data:2013.10.15
******************************************************************************/
#include "stm32f10x.h"


#ifndef __POLYGON__H_
#define __POLYGON__H_
/*****************************************************************************
*Function Name:  Polygon_ParameterInitialize
*Input  Parameter:  None
*Output Parameter:  None 
*Function Describtion:Initialize the area parameter 
*Writer:mark
*Update Data:2013.10.15
******************************************************************************/
void Polygon_ParameterInitialize(void);
/*****************************************************************************
*Function Name:  Polygon_TimeTask
*Input  Parameter:  None
*Output Parameter:  None 
*Function Describtion:Arrange all timer task 
*Writer:mark
*Update Data:2013.10.15
******************************************************************************/
FunctionalState Polygon_TimeTask(void) ;
/*****************************************************************************
*Function Name:  Polygon_SetArea(unsigned char *pBuffer, unsigned short BufferLen)
*Input  Parameter:  *pbuffer,bufferlen
*Output Parameter:  Polygon area 
*Function Describtion:Set polygon area
*Writer:mark
*Update Data:2013.10.15
******************************************************************************/
unsigned char Polygon_SetArea(unsigned char *pBuffer, unsigned short BufferLen);

/*****************************************************************************
*Function Name:  Polygon_DeleteArea(unsigned char *pBuffer, unsigned short BufferLen)
*Input  Parameter:  None
*Output Parameter:  None 
*Function Describtion:Delete polygon area 
*Writer:mark
*Update Data:2013.10.15
******************************************************************************/
unsigned char Polygon_DeleteArea(unsigned char *pBuffer, unsigned short BufferLen);
/*************************************************************
** ��������: Polygon_AreaModifyUserData
** ��������: �޸Ķ���������Զ��岿��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*************************************************************/ 
unsigned char Polygon_AreaModifyUserData(unsigned char *pBuffer, unsigned short BufferLen);
/*************************************************************
** ��������: Polygon_GetAreaTotalNum
** ��������: ��ö������������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: �������������
*************************************************************/ 
unsigned char Polygon_GetAreaTotalNum(void);
/*****************************************************************************
*Function Name: Polygon_ClearArea()
*Input  Parameter:  None
*Output Parameter:  None
*Function Describtion:clear Polygon area flash sector  
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_ClearArea(void);
#endif