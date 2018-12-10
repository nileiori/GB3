


/*******************************************************************************
* Function Name  : Lock_Before_Use_TimeTask(void)
* Description    : ʹ��ǰ����һ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

FunctionalState LockBeforeUseOne_TimeTask(void);
/*******************************************************************************
* Function Name  : Lock_Before_Use_Two_TimeTask(void)
* Description    : ʹ��ǰ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

FunctionalState LockBeforeUseTwo_TimeTask(void);


/*******************************************************************************
* Function Name  : Lock_Before_Use_TimeTask(void)
* Description    : ʹ��ǰ����һ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 Check_Lock_Two_State(void);


/*******************************************************************************
* Function Name  : Clear_First_Register(void)
* Description    : ����ע���־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Clear_First_Register_Flag(void);
/*******************************************************************************
* Function Name  : Set_First_Register_Flag(void)
* Description    : ����ע���־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Set_First_Register_Flag(void);
/*******************************************************************************
* Function Name  : Check_First_Register_Flag(void)
* Description    : ��鿪����ɱ�־
* Input          : None
* Output         : None
* Return         : 1:������� 0:δ���
*******************************************************************************/
u8 Check_First_Register_Flag(void);

/*******************************************************************************
* Function Name  : void BackUp_OpenLockOneDataFlag(u32 PramID,u8 *pBuffer)
* Description    : ��������һ������־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BackUp_OpenLockOneDataFlag(u32 PramID,u8 *pBuffer);


