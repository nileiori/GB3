


/*******************************************************************************
* Function Name  : Lock_Before_Use_TimeTask(void)
* Description    : 使用前锁定一
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

FunctionalState LockBeforeUseOne_TimeTask(void);
/*******************************************************************************
* Function Name  : Lock_Before_Use_Two_TimeTask(void)
* Description    : 使用前锁定二
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

FunctionalState LockBeforeUseTwo_TimeTask(void);


/*******************************************************************************
* Function Name  : Lock_Before_Use_TimeTask(void)
* Description    : 使用前锁定一
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 Check_Lock_Two_State(void);


/*******************************************************************************
* Function Name  : Clear_First_Register(void)
* Description    : 清零注册标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Clear_First_Register_Flag(void);
/*******************************************************************************
* Function Name  : Set_First_Register_Flag(void)
* Description    : 清零注册标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Set_First_Register_Flag(void);
/*******************************************************************************
* Function Name  : Check_First_Register_Flag(void)
* Description    : 检查开户完成标志
* Input          : None
* Output         : None
* Return         : 1:开户完成 0:未完成
*******************************************************************************/
u8 Check_First_Register_Flag(void);

/*******************************************************************************
* Function Name  : void BackUp_OpenLockOneDataFlag(u32 PramID,u8 *pBuffer)
* Description    : 备份锁定一开启标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BackUp_OpenLockOneDataFlag(u32 PramID,u8 *pBuffer);


