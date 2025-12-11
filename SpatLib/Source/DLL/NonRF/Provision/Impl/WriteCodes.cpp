#include "StdAfx.h"
#include "WriteCodes.h"
#include "Utility.h"
#include <atltime.h>
#include "GenCodes.h"
#include "../DBHelper.h"
#include "UeHelper.h"
#include "MesHelper.h"

IMPLEMENT_RUNTIME_CLASS(CWriteCodes)

//////////////////////////////////////////////////////////////////////////
CWriteCodes::CWriteCodes(void)
: m_bSaveDataIntoMDB(FALSE)
, m_bSaveDataIntoTXT(FALSE)
, m_bCheckUniqueIMEI(FALSE)
{
}

CWriteCodes::~CWriteCodes(void)
{
}

SPRESULT CWriteCodes::__PollAction(void)
{
	SPRESULT sp_result = SP_OK;
	CUeHelper ue(this);
	CMesHelper mes(this);

	MES_RESULT mes_result = mes.UnisocMesActive();
	if (MES_SUCCESS == mes_result)
	{
		sp_result = mes.UnisocMesGetAssignedCodes(m_InputSN,sizeof(m_InputSN));
		if (SP_OK != sp_result)
		{
			return sp_result;
		}
	}
	else
	{
		CHKRESULT_WITH_NOTIFY(GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN)), "GetShareMemory(InputCode)");
		CHKRESULT(CheckUniqueIMEI());
	}

	for (INT i=BC_SN1; i<=BC_SN2; i++)
	{
		if (!m_InputSN[i].bEnable)
		{
			continue;
		}
		CHKRESULT(ue.WriteSN((BC_SN1 == i) ? SN1 : SN2, m_InputSN[i].szCode));
	}

	// Write IMEI for ProductData
	PC_PRODUCT_DATA ProductData;
	ZeroMemory(&ProductData, sizeof(PC_PRODUCT_DATA));
	uint8 *pStr[BC_MAX_NUM - 2] = {0};
	pStr[BC_BT] = ProductData.szBTAddr;
	pStr[BC_WIFI] = ProductData.szWIFIAddr;
	pStr[BC_IMEI1] = ProductData.szImei1;
	pStr[BC_IMEI2] = ProductData.szImei2;
	pStr[BC_IMEI3] = ProductData.szImei3;
	pStr[BC_IMEI4] = ProductData.szImei4;
	for (int i = BC_BT; i <= BC_IMEI4; i++)
	{
		if(m_InputSN[i].bEnable)
		{
			ProductData.u32OperMask |= CBarCodeUtility::m_BarCodeInfo[i].Mask;
			memcpy(pStr[i], m_InputSN[i].szCode, CBarCodeUtility::m_BarCodeInfo[i].SNlength);
		}
	}

	 if (0 != ProductData.u32OperMask)
	 {
		 CHKRESULT(ue.WriteProductData(ProductData));
	 }
	
    BOOL bOK = TRUE;
    SetShareMemory(ShareMemory_My_UpdateSN, (void* )&bOK, sizeof(bOK), IContainer::System);
    if (IS_BIT_SET(ProductData.u32OperMask, FNAMASK_RW_IMEI1))
    {
        SetShareMemory(ShareMemory_IMEI1, (void* )&ProductData.szImei1, ShareMemory_IMEI_SIZE);
    }

    CHKRESULT(SaveDataIntoLocal());
    CHKRESULT(UpdateSectionAllocCodes());

    return SP_OK;
}

SPRESULT CWriteCodes::UpdateSectionAllocCodes(void)
{
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (m_InputSN[i].bEnable)
        {
            if (E_GENCODE_SECTION == m_InputSN[i].eGenCodeType )
            {
                CGenCodes GenCode;
                GenCode.Init(CBarCodeUtility::m_BarCodeInfo[i].nameW, this);
                CHKRESULT_WITH_NOTIFY(GenCode.CompleteCodes(&m_InputSN[i]), "CompleteCodes");
            }
            else if(BC_IMEI2 == i && E_GENCODE_SECTION == m_InputSN[BC_IMEI1].eGenCodeType 
                && E_GENCODE_FROMIMEI1 == m_InputSN[i].eGenCodeType)
            {
                CGenCodes GenCode;
                GenCode.Init(CBarCodeUtility::m_BarCodeInfo[BC_IMEI1].nameW, this);
                CHKRESULT_WITH_NOTIFY(GenCode.CompleteCodes(&m_InputSN[i]), "CompleteCodes");
            }
        }
    }

    return SP_OK;
}

BOOL CWriteCodes::LoadXMLConfig(void)
{
    m_bCheckUniqueIMEI  = (BOOL)GetConfigValue(L"Option:CheckIMEIUnique", 1);
    m_bSaveDataIntoMDB  = (BOOL)GetConfigValue(L"Option:SaveInfoToMdb", 1);
    m_bSaveDataIntoTXT  = (BOOL)GetConfigValue(L"Option:SaveInfoToTxt", 1);

    std::wstring strMDB = GetConfigValue(L"Option:MDB", L"..\\..\\UserData\\IMEI.mdb");
    m_strMDBpath = GetAbsFilePath(strMDB);
    if (0 == m_strMDBpath.length())
    {
        return FALSE;
    }
    else
    {
        m_strMDBConn = L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + m_strMDBpath;
    }

    std::wstring strTXT = GetConfigValue(L"Option:TXT", L"..\\IMEI.txt");
    m_strTXTpath = GetAbsFilePath(strTXT);
    if (0 == m_strTXTpath.length())
    {
        return FALSE;
    }
   
    return TRUE;
}

SPRESULT CWriteCodes::SaveDataIntoLocal(void)
{
    BOOL bNeedSave = FALSE;
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (m_InputSN[i].bEnable)
        {
            bNeedSave = TRUE;
            break;
        }
    }
    if (!bNeedSave)
    {
        return SP_OK;
    }

    CHAR szTime[64] = {0};
    SYSTEMTIME     tm;
    GetLocalTime(&tm);
    sprintf_s(szTime, "%04d-%02d-%02d %02d:%02d:%02d", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);

    if (m_bSaveDataIntoMDB)
    {
        std::string strSQL = "INSERT INTO [IMEI] (";
        for (INT i=BC_START; i<BC_MAX_NUM; i++)
        {
			if(i == BC_NETCODE)
				continue;
            if (m_InputSN[i].bEnable)
            {
                strSQL = strSQL + "[" + CBarCodeUtility::m_BarCodeInfo[i].nameA + "]" + ",";
            }
        }
        
        strSQL = strSQL + "[TIME]) VALUES (";
        for (INT i=BC_START; i<BC_MAX_NUM; i++)
        {
			if(i == BC_NETCODE)
				continue;
            if (m_InputSN[i].bEnable)
            {
                strSQL = strSQL + "\'" + m_InputSN[i].szCode + "\'" + ",";
            }
        }
        strSQL = strSQL + "\'" + szTime + "\'" + ")";

        CDBHelper db(m_strMDBConn, GetISpLogObject());
        db.Insert(strSQL);
    }

    if (m_bSaveDataIntoTXT)
    {
        // ´æ´¢µ½IMEI.txtÎÄ¼þ
        FILE *fp = NULL ;  
        errno_t err = _wfopen_s(&fp, m_strTXTpath.c_str(), L"a+");
        if (0 != err)
        {
            LogFmtStrW(SPLOGLV_ERROR, L"Open %s fail.", m_strTXTpath.c_str());
            return SP_E_OPEN_FILE;
        }
        
        std::string strHead = "";
        std::string strLine = "";
        for (INT i=BC_START; i<BC_MAX_NUM; i++)
        {
            CHAR szHead[64] = {0};
            CHAR szText[128] = {0};
            if (BC_BT == i || BC_WIFI == i)
            {
                sprintf_s(szHead, "%-13s,", CBarCodeUtility::m_BarCodeInfo[i].nameA);
                sprintf_s(szText, "%-13s,", m_InputSN[i].bEnable ? m_InputSN[i].szCode : "");
            }
            else if (BC_IMEI1 == i || BC_IMEI2 == i || BC_IMEI3 == i || BC_IMEI4 == i || BC_MEID1 == i || BC_MEID2 == i)
            {
                sprintf_s(szHead, "%-16s,", CBarCodeUtility::m_BarCodeInfo[i].nameA);
                sprintf_s(szText, "%-16s,", m_InputSN[i].bEnable ? m_InputSN[i].szCode : "");
            }
            else
            {
                sprintf_s(szHead, "%-24s,", CBarCodeUtility::m_BarCodeInfo[i].nameA);
                sprintf_s(szText, "%-24s,", m_InputSN[i].bEnable ? m_InputSN[i].szCode : "");
            }
            strHead += szHead;
            strLine += szText;
        }

        strHead = strHead + "TIME\n";
        strLine = strLine + szTime + "\n";

        fseek(fp, 0, SEEK_END); 
        if (0 == ftell(fp))
        {
            fprintf_s(fp, strHead.c_str());
        }
        fprintf_s(fp, strLine.c_str());
        fclose(fp);
    }

    return SP_OK;
}

SPRESULT CWriteCodes::CheckUniqueIMEI(void)
{
    if (!m_bCheckUniqueIMEI)
    {
        return SP_OK;
    }

    BOOL bSaveIMEI = FALSE;
    for (INT i=BC_IMEI1; i<=BC_IMEI4; i++)
    {
        if (m_InputSN[i].bEnable)
        {
            bSaveIMEI = TRUE;
            break;
        }
    }
    if (!bSaveIMEI)
    {
        return SP_OK;
    }

    CDBHelper db(m_strMDBConn, GetISpLogObject());
    for (INT i=BC_IMEI1; i<=BC_IMEI4; i++)
    {
        if (!m_InputSN[i].bEnable)
        {
            continue;
        }

        CHAR szSQL[256] = {0};
        sprintf_s(szSQL, "SELECT * FROM [IMEI] WHERE [%s] = \'%s\'", CBarCodeUtility::m_BarCodeInfo[i].nameA, m_InputSN[i].szCode);
        
        LONG lCount = 0;
        std::string   strSQL = szSQL;
        if (db.QueryCount(strSQL, lCount) && lCount > 0)
        {
            NOTIFY("IMEI UNIQUE CHECK", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "%s: %s is used", CBarCodeUtility::m_BarCodeInfo[i].nameA, m_InputSN[i].szCode);
            return SP_E_CHECK_WRITEX_IMEI_IS_USED;
        }
    }

    return SP_OK;
}
