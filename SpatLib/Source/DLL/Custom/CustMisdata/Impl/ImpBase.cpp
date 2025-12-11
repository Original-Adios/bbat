#include "StdAfx.h"
#include "ImpBase.h"
#include "ActionApp.h"


extern CActionApp myApp;
//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
}

CImpBase::~CImpBase(void)
{
}

SPRESULT CImpBase::SendATCommand(LPCSTR lpszCmd, uint32 u32TimeOut /* = TIMEOUT_3S */)
{
    CHAR   szRevBuf[4096] = {0};
    uint32 u32RevSize = 0;
    SPRESULT res = SP_SendATCommand(m_hDUT, lpszCmd, TRUE, szRevBuf, sizeof(szRevBuf), &u32RevSize, u32TimeOut);
    if (SP_OK != res)
    {
        return res;
    }

    if (NULL != strstr(szRevBuf, "CME ERROR"))
    {
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }

    m_strRevBuf = szRevBuf;
    replace_all(m_strRevBuf, "\r", "");
    replace_all(m_strRevBuf, "\n", "");
    replace_all(m_strRevBuf, "OK", "");

    return SP_OK;
}


BOOL CImpBase::PopupHtmlView( const WINDOW_PROPERTY& Wnd, LPCWSTR lpszHtmlFile, LPCWSTR lpszJsonFile/* = NULL */ )
{
    if ( NULL == lpszHtmlFile )
    {
        return FALSE;
    }
    
    return GetCallback()->UiMsgPopupHtmlView( Wnd, lpszHtmlFile, ( NULL != lpszJsonFile ) ? lpszJsonFile : _T2CW( m_szJsonPath ) );
}

BOOL CImpBase::LoadJsonFile(LPCTSTR lpszJsonFile /* = NULL */)
{
    return m_Json.LoadJson((NULL != lpszJsonFile) ? lpszJsonFile : m_szJsonPath);
}

BOOL CImpBase::SaveJsonFile(LPCTSTR lpszJsonFile /* = NULL */)
{
    return m_Json.SaveJson((NULL != lpszJsonFile) ? lpszJsonFile : m_szJsonPath);
}

SPRESULT CImpBase::__InitAction(void)
{
    /*
        Jason file: Bin\sys\InputBarcode_1(2/3/4).Json
    */
    _stprintf_s(m_szJsonPath, _T("%s\\InputCodes_%d.Json"), myApp.GetSysPath(), GetAdjParam().nTASK_ID);

    /*
        X:\Bin\App\SpatLib\Html
    */
    _stprintf_s(m_szHtmlPath, _T("%s\\Html"), myApp.GetAppPath());

    return SP_OK;
}


SPRESULT CImpBase::WriteCustMisdata(uint32 u32InBase, const void * pData, const int length, ESYSTEM eSys)
{
	CONST INT MAX_MISDATA_LENGTH = (eSys == ESYS_Android) ? MAX_CUSTOMIZED_ANDROID_MISCDATA_SIZE : MAX_CUSTOMIZED_RTOS_MISCDATA_SIZE;
	if (!IN_RANGE(1, length, MAX_MISDATA_LENGTH))
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid CustMisdata code, length = \'%d\'", length);
		return SP_E_INVALID_PARAMETER;
	}

	CONST INT MAX_MISDATA_BASE_OFFSET = (eSys == ESYS_Android) ? MISCDATA_BASE_ANDROID_OFFSET : MISCDATA_BASE_RTOS_OFFSET;
	CONST INT MAX_MISDATA_BASE_MAX_OFFSET = (eSys == ESYS_Android) ? (MISCDATA_BASE_ANDROID_OFFSET + MAX_CUSTOMIZED_ANDROID_MISCDATA_SIZE) : (MISCDATA_BASE_RTOS_OFFSET + MAX_CUSTOMIZED_RTOS_MISCDATA_SIZE);
	if (!IN_RANGE(MAX_MISDATA_BASE_OFFSET, (const int)u32InBase, MAX_MISDATA_BASE_MAX_OFFSET - length))
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid CustMisdata code, base range = \'%d\'", u32InBase);
		return SP_E_INVALID_PARAMETER;
	}

	//写入miscdata
	CHKRESULT_WITH_NOTIFY(SP_SaveMiscData(m_hDUT, u32InBase, (uint8*)pData, length, TIMEOUT_3S), "SaveCustomerMiscData");

	Sleep(100);

	// 读取出来和写入的数据进行比较
	uint8 *pu8Rbuff = (uint8*)malloc(sizeof(uint8)*length + 1);
	if (!pu8Rbuff)
	{
		LogFmtStrA(SPLOGLV_ERROR, "malloc memory failed");
		return SP_E_ALLOC_MEMORY;
	}
	memset(pu8Rbuff, 0, sizeof(uint8)*length + 1);
	CHKRESULT_WITH_NOTIFY(SP_LoadMiscData(m_hDUT, u32InBase, pu8Rbuff, length, TIMEOUT_3S), "LoadCustomerMiscData");
	if (0 != memcmp(pData, pu8Rbuff, length))
	{
			NOTIFY("VerifyCustomerMiscData", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Verify after write fails");
			free(pu8Rbuff);
			pu8Rbuff = NULL;
			return SP_E_MISMATCHED_CU;
	}

	free(pu8Rbuff);
	pu8Rbuff = NULL;

	return SP_OK;
}