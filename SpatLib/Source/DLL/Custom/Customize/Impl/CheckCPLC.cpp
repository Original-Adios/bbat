#include "stdafx.h"
#include "CheckCPLC.h"

IMPLEMENT_RUNTIME_CLASS(CCheckCPLC)

//////////////////////////////////////////////////////////////////////////
CCheckCPLC::CCheckCPLC()
{
	m_bCompareCPLC = FALSE;
}

CCheckCPLC::~CCheckCPLC()
{

}

BOOL CCheckCPLC::LoadXMLConfig(void)
{
    m_strCPLC = _W2CA(GetConfigValue(L"Option:CPLC", L""));
    m_bCompareCPLC = (BOOL)GetConfigValue(L"Option:CompareCPLC", 1);
    return TRUE;
}

SPRESULT CCheckCPLC::__PollAction(void)
{
    LPCSTR NFC_CMD = "AT+GETNFCCPLC";
    SPRESULT res = SendATCommand(NFC_CMD);
    if (SP_OK != res)
    {
        NOTIFY("NFC-CPLC", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, NFC_CMD);
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }
    else
    {
		if (!m_bCompareCPLC)
		{
			NOTIFY("NFC-CPLC", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
		}
		else
		{
			if (NULL == strstr(m_strRevBuf.c_str(), m_strCPLC.c_str()))
			{
				NOTIFY("NFC-CPLC", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, m_strRevBuf.c_str());
				return SP_E_CHECK_TP_TYPE_ERROR;
			}
			else
			{
				NOTIFY("NFC-CPLC", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
			}
		}
    }

	if (MES_SUCCESS == UnisocMesActive())
	{
		_UNISOC_MES_CMN_CODES mes_codes;
		if (SP_OK == GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes)))
		{
			strncpy_s(mes_codes.sz_cmn_nfc_cplc,m_strRevBuf.data(),CopySize(mes_codes.sz_cmn_nfc_cplc));
			CHKRESULT(SetShareMemory(ShareMemory_MES_Assigned_Codes, (const void*)&mes_codes, sizeof(mes_codes)));
		}
	}

    return SP_OK;
}
