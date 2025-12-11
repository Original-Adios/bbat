#include "Stdafx.h"
#include "AMU_Keypad.h"

IMPLEMENT_RUNTIME_CLASS(CAMU_Keypad)

CAMU_Keypad::CAMU_Keypad(void)
{

}

CAMU_Keypad::~CAMU_Keypad(void)
{
}


SPRESULT CAMU_Keypad::__PollAction(void)
{
	CHKRESULT(RunOpen());
	Sleep(1000);
	if (m_iKeyNumber)
	{
		MessageBox(NULL, _T("Please press Keys"), _T("Keypad"), NULL);
		CHKRESULT(RunMultiRead())
	}
	CHKRESULT(RunClose());
	return SP_OK;
}

BOOL CAMU_Keypad::LoadXMLConfig(void)
{
	m_iKeyNumber = GetConfigValue(L"Option:KeyNumber", 0);
	// std::wstring strKey = GetConfigValue(L"Option:Expect_KeyValue", L"");
	m_strKeyValue = _W2CA(GetConfigValue(L"Option:Expect_KeyValue", L""));
	return TRUE;
}

SPRESULT CAMU_Keypad::RunOpen()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_KEYPAD;
	keyValue DataSend;
	DataSend.iOperate = BBAT_KEYPAD::OPEN;
	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		nullptr, 0, Timeout_PHONECOMMAND_BBAT),
		"BBAT Keypad Open: Open Fail",
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT Keypad Open: Open Pass",LEVEL_ITEM);
	
	return SP_OK;
}

SPRESULT CAMU_Keypad::RunMultiRead()
{
	
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_KEYPAD;
	keyValue DataSend;
	keyValueRecv DataRecv;
	DataSend.iOperate = BBAT_KEYPAD::MULTIREAD;
	memset(&DataRecv, NULL, sizeof(DataRecv));

	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		&DataRecv, sizeof(DataRecv), Timeout_PHONECOMMAND_BBAT),
		"BBAT Keypad Multiread: Multiread Fail",
		LEVEL_ITEM));
	   
	CHKRESULT(PrintErrorMsg(CheckKeyVal(DataRecv),
		"BBAT Keypad Multiread: Multiread Check Fail",
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT Keypad Multiread: Multiread Pass", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CAMU_Keypad::CheckKeyVal(keyValueRecv DataRecv)
{
	//sos:0x75, sound+:0x73, sound-: 0x72, power: 0x74

	if (DataRecv.KeyCount != m_iKeyNumber)
	{
		PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "BBAT Keypad Multiread: Key Count Fail", LEVEL_ITEM);
		return SP_E_BBAT_VALUE_FAIL;
	}
	bool bKeyPass = true;
	for (int iCount = 0; iCount < m_iKeyNumber; iCount++) {
		char c[5];
		_itoa_s(DataRecv.iKeyValue[iCount], c, 10);
		std::string strTemp = c;
		int iTemp = m_strKeyValue.find(strTemp);
		if (iTemp < 0)
		{
			_UiSendMsg("Keypad Number", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, 
				"按键%x不在目标键值中", DataRecv.iKeyValue[iCount]);
			bKeyPass = false;
		}		
	}
	if (!bKeyPass)
		return SP_E_BBAT_VALUE_FAIL;
	PrintSuccessMsg(SP_OK, "Multikey Check Pass", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CAMU_Keypad::RunClose()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_KEYPAD;
	keyValue DataSend;
	DataSend.iOperate = BBAT_KEYPAD::CLOSE;
	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		nullptr, 0, Timeout_PHONECOMMAND_BBAT),
		"BBAT Keypad Close: Close Fail",
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT Keypad Close: Close Pass", LEVEL_ITEM);

	return SP_OK;
}
