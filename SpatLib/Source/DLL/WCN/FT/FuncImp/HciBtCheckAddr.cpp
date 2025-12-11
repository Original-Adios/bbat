#include "StdAfx.h"
#include "HciBtCheckAddr.h"


IMPLEMENT_RUNTIME_CLASS(CHciBtCheckAddr)
//////////////////////////////////////////////////////////////////////////
CHciBtCheckAddr::CHciBtCheckAddr(void)
	: m_bCheckInfoFrTxt(FALSE)
{
}

CHciBtCheckAddr::~CHciBtCheckAddr(void)
{
}

BOOL CHciBtCheckAddr::LoadXMLConfig(void)
{
	m_bCheckInfoFrTxt = (BOOL)GetConfigValue(L"Option:CheckInfoFrTxt", 0);

	std::wstring strTXT = GetConfigValue(L"Option:TXT", L"..\\HciBtAddress.txt");
	m_strTXTpath = GetAbsoluteFilePathW(strTXT.c_str());
	if (0 == m_strTXTpath.length())
	{
		LogFmtStrA(SPLOGLV_ERROR, "%s", "Option:TXT path Set Fail.");
		return FALSE;
	}

	return TRUE;
}

SPRESULT CHciBtCheckAddr::__PollAction(void)
{
	CBTApiHCI ue(this->m_hDUT);
	CHAR szCode[BARCODEMAXLENGTH + 1] = { 0 };

	// Get Input codes from share memory
	GetShareMemory(ShareMemory_My_UserInputSN, (void*)&m_InputSN, sizeof(m_InputSN));

	CHKRESULT_WITH_NOTIFY(ue.DUT_Reset(), "DUT_Reset");
	// Read BT & WIFI MAC address
	CHKRESULT_WITH_NOTIFY(ue.DUT_ReadBtAddr(szCode, 12), "DUT_ReadBtAddr");

	CHKRESULT_WITH_NOTIFY(CheckBTCodeFrTxt(szCode), "CheckBTCodeFrTxt");

	_UiSendMsg("Check Hci Bt Addr", LEVEL_ITEM | LEVEL_FT, 1, 1, 1, szCode);
	if (m_InputSN[BC_BT].bEnable)
	{
		LogFmtStrA(SPLOGLV_INFO, "InputCode %s Read %s.", m_InputSN[BC_BT].szCode, szCode);
		if (strcmp(m_InputSN[BC_BT].szCode, szCode))
		{

			_UiSendMsg("Compare Hci Bt Addr", LEVEL_ITEM | LEVEL_FT, 1, 0, 1, "InputCode != Read.");
			return SP_E_INVALID_PARAMETER;
		}
		else
		{
			_UiSendMsg("Compare Hci Bt Addr", LEVEL_ITEM | LEVEL_FT, 1, 1, 1, "InputCode = Read.");
		}
	}

	return SP_OK;
}

SPRESULT CHciBtCheckAddr::CheckBTCodeFrTxt(const char* lpAddr)
{
	if (!m_bCheckInfoFrTxt)
	{
		return SP_OK;
	}
	if (!IsExistFileA(_W2CA(m_strTXTpath.c_str())))
	{
		LogFmtStrW(SPLOGLV_ERROR, L"Open %s fail.", m_strTXTpath.c_str());
		return SP_E_FILE_NOT_EXIST;
	}

	// 从.txt文件读取
	FILE* fp = NULL;
	errno_t err = 0;
	err = fopen_s(&fp, _W2CA(m_strTXTpath.c_str()), "r");
	if (0 != err)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"Open %s fail.", m_strTXTpath.c_str());
		return SP_E_OPEN_FILE;
	}

	fseek(fp, 0, SEEK_END);
	size_t nBuffSize = ftell(fp);
	if (0 == nBuffSize)
	{
		fclose(fp);
		fp = NULL;
		LogFmtStrW(SPLOGLV_ERROR, L"%s SP_E_EMPTY_FILE.", m_strTXTpath.c_str());
		return SP_E_EMPTY_FILE;
	}
	rewind(fp);

	char szBuff[1024] = {0};
	SPRESULT nRet = SP_OK;
	while (0 == feof(fp))
	{
		if (_IsUserStop())
		{
			fclose(fp);
			fp = NULL;
			return SP_E_USER_ABORT;
		}
		if (NULL != fgets(szBuff, 1024, fp))
		{
			if (NULL != strstr(szBuff, lpAddr))
			{
				fclose(fp);
				fp = NULL;
				return SP_OK;
			}
		}
	}

	fclose(fp);
	fp = NULL;
	LogFmtStrA(SPLOGLV_ERROR, "%s search %s fail.", _W2CA(m_strTXTpath.c_str()), lpAddr);
	return SP_E_INVALID_PARAMETER;
}