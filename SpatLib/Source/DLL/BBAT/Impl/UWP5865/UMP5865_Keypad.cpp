#include "StdAfx.h"
#include "UMP5865_Keypad.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMP5865_Keypad)

//////////////////////////////////////////////////////////////////////////
CUMP5865_Keypad::CUMP5865_Keypad(void)
{
	m_iKeyNumber = 1;
	
}

CUMP5865_Keypad::~CUMP5865_Keypad(void)
{
}

SPRESULT CUMP5865_Keypad::__PollAction(void)
{
	SPRESULT res = SP_OK;
    BYTE bySend[1024] = {0x01};
    BYTE byRecv[1024] = {0x00};
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_KEYPAD;
	bySend[0]=1;  
    res = SP_AutoBBTest(m_hDUT, eDeviceCmd, (LPCVOID)bySend, 1, NULL, 0, Timeout_PHONECOMMAND_BBAT);
    NOTIFY("UMP5865_Keypad", LEVEL_ITEM, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "Cmd = %d", eDeviceCmd);
    if(res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}

	Sleep(1000); //
	MessageBox(NULL,_T("Please press Keys"),_T("Keypad"),NULL);
	bySend[0]=5;  
	
	memset(byRecv,NULL,sizeof(byRecv));
	res = SP_AutoBBTest(m_hDUT,eDeviceCmd,bySend,1,byRecv, 50, Timeout_PHONECOMMAND_BBAT);
	NOTIFY("UMP5865_Keypad", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "KeyValue Pass");
	if(res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL;
	}
	else
	{

		for(int iCount = 0;iCount<byRecv[0];iCount++)
		{	
			NOTIFY("读取按键", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "读取到按键:%x", byRecv[iCount+1]);
		}

		NOTIFY("Keypad Number", LEVEL_ITEM, 1, (byRecv[0] == m_iKeyNumber) ? 1 : 0, 1, NULL, -1, NULL, "读取到了%d个按键，spec：%d个按键", byRecv[0],m_iKeyNumber);
		if(byRecv[0] != m_iKeyNumber)
		{
			return SP_E_BBAT_VALUE_FAIL;
		}
	}

	bySend[0]=3;  
	res = SP_AutoBBTest(m_hDUT,eDeviceCmd,bySend,1,NULL, 0, Timeout_PHONECOMMAND_BBAT);
	if(res != SP_OK)
	{
		return SP_E_BBAT_CMD_FAIL; 
	}
	//判断键值
	//NOTIFY("Keypad Value", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "目标键值是%s", m_strKeyValue);		
	bool bKeyPass = true;
	for(int iCount = 0;iCount<byRecv[0];iCount++)
	{
		char c[5] ;
		_itoa_s(byRecv[iCount+1],c,16);	
		int iTemp = m_strKeyValue.find(c[0]);
		if(iTemp < 0)
		{
			NOTIFY("Keypad Number", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "按键%x不在目标键值中", byRecv[iCount+1]);
			bKeyPass = false;
		}
	}
	if(!bKeyPass)
		return SP_E_BBAT_VALUE_FAIL;
	return res;
}
BOOL CUMP5865_Keypad::LoadXMLConfig(void)
{
	m_iKeyNumber = GetConfigValue(L"Option:KeyNumber", 0);
   // std::wstring strKey = GetConfigValue(L"Option:Expect_KeyValue", L"");
    m_strKeyValue = _W2CA(GetConfigValue(L"Option:Expect_KeyValue", L""));
	return TRUE;
}