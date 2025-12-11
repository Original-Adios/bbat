#include "StdAfx.h"
#include "HciBtWriteAddr.h"


IMPLEMENT_RUNTIME_CLASS(CHciBtWriteAddr)
//////////////////////////////////////////////////////////////////////////
CHciBtWriteAddr::CHciBtWriteAddr(void)
	: m_bSaveDataIntoTXT(FALSE)
	, m_bCheckUniqueBT(FALSE)
{
}

CHciBtWriteAddr::~CHciBtWriteAddr(void)
{
}

BOOL CHciBtWriteAddr::LoadXMLConfig(void)
{
	m_bCheckUniqueBT = (BOOL)GetConfigValue(L"Option:CheckBtUnique", 0);
	m_bSaveDataIntoTXT = (BOOL)GetConfigValue(L"Option:SaveInfoToTxt", 1);

	std::wstring strTXT = GetConfigValue(L"Option:TXT", L"..\\HciBtAddress.txt");
	m_strTXTpath = GetAbsoluteFilePathW(strTXT.c_str());
	if (0 == m_strTXTpath.length())
	{
		LogFmtStrA(SPLOGLV_ERROR, "%s", "Option:TXT path Set Fail.");
		return FALSE;
	}

	return TRUE;

}

SPRESULT CHciBtWriteAddr::__PollAction(void)
{
	CBTApiHCI ue(this->m_hDUT);
	// Get Input codes from share memory
	CHKRESULT_WITH_NOTIFY(GetShareMemory(ShareMemory_My_UserInputSN, (void*)&m_InputSN, sizeof(m_InputSN)), "GetShareMemory(InputCode)");

	CHKRESULT_WITH_NOTIFY(CheckUniqueBT(m_InputSN[BC_BT].szCode), "CheckUniqueBT");

	// Write BT address
	if (m_InputSN[BC_BT].bEnable)
	{
		CHKRESULT_WITH_NOTIFY(ue.DUT_Reset(), "DUT_Reset");
		CHKRESULT_WITH_NOTIFY(ue.DUT_WriteBtAddr(m_InputSN[BC_BT].szCode, m_InputSN[BC_BT].nMaxLength), "DUT_WriteBtAddr");
	}

	BOOL bOK = TRUE;
	CHKRESULT(SetShareMemory(ShareMemory_My_UpdateSN, (void*)&bOK, sizeof(bOK), IContainer::System));

	CHKRESULT_WITH_NOTIFY(SaveDataIntoLocal(), "SaveDataIntoLocal");

	_UiSendMsg("Write Hci Bt Addr", LEVEL_ITEM | LEVEL_FT, 1, 1, 1, m_InputSN[BC_BT].szCode);

	return SP_OK;
}

SPRESULT CHciBtWriteAddr::SaveDataIntoLocal(void)
{
	BOOL bNeedSave = FALSE;
	for (INT i = BC_START; i < BC_MAX_NUM; i++)
	{
		if (m_InputSN[i].bEnable)
		{
			bNeedSave = TRUE;
			break;
		}
	}
	if (!bNeedSave || !m_bSaveDataIntoTXT)
	{
		return SP_OK;
	}

	CHAR szTime[64] = { 0 };
	SYSTEMTIME     tm;
	GetLocalTime(&tm);
	sprintf_s(szTime, "%04d-%02d-%02d %02d:%02d:%02d", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);


	// 存储到.txt文件
	FILE* fp = NULL;
	errno_t err = 0;
	err = fopen_s(&fp, _W2CA(m_strTXTpath.c_str()), "a+");
	if (0 != err)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"Open %s fail.", m_strTXTpath.c_str());
		return SP_E_OPEN_FILE;
	}

	std::string strHead = "";
	std::string strLine = "";
	for (INT i = BC_START; i < BC_MAX_NUM; i++)
	{
		if (!m_InputSN[i].bEnable)
		{
			continue;
		}
		CHAR szHead[64] = { 0 };
		CHAR szText[128] = { 0 };
		if (BC_BT == i)
		{
			sprintf_s(szHead, "%-13s,", "BT");
			sprintf_s(szText, "%-13s,", m_InputSN[i].bEnable ? m_InputSN[i].szCode : "");
		}

		strHead += szHead;
		strLine += szText;
	}

	strHead = strHead + "TIME\n";
	strLine = strLine + szTime + "\n";

	fseek(fp, 0, SEEK_END);
	if (0 == ftell(fp))
	{
		fwrite(strHead.c_str(),1, strHead.size(), fp);
	}
	fwrite(strLine.c_str(), 1, strLine.size(), fp);
	fclose(fp);
	fp = NULL;

	return SP_OK;
}


SPRESULT CHciBtWriteAddr::CheckUniqueBT(const char* lpAddr)
{
	if (!m_bCheckUniqueBT)
	{
		return SP_OK;
	}

	if (!IsExistFileA(_W2CA(m_strTXTpath.c_str())))
	{
		return SP_OK;
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
	fseek(fp, 0, SEEK_SET);

	char szBuff[1024] = { 0 };
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
				LogFmtStrW(SPLOGLV_ERROR, L"%s search %s success.", m_strTXTpath.c_str(), lpAddr);
				return SP_E_INVALID_PARAMETER;
			}
		}
	}

	fclose(fp);
	fp = NULL;
	return SP_OK;
}