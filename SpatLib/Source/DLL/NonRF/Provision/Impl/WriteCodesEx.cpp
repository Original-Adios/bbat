#include "StdAfx.h"
#include "WriteCodesEx.h"
#include "DBHelper.h"
#include "UeHelper.h"
#include "MesHelper.h"

IMPLEMENT_RUNTIME_CLASS(CWriteCodesEx)

//////////////////////////////////////////////////////////////////////////
CWriteCodesEx::CWriteCodesEx(void)
: m_bCheckUniqueMEID(FALSE)
{
    m_bCheckUniqueIMEI = FALSE;
}

CWriteCodesEx::~CWriteCodesEx(void)
{
}

BOOL CWriteCodesEx::LoadXMLConfig(void)
{
    if (!__super::LoadXMLConfig())
    {
        return FALSE;
    }
    m_bCheckUniqueIMEI = (BOOL)GetConfigValue(L"Option:CheckIMEIUnique", 1);
    m_bCheckUniqueMEID = (BOOL)GetConfigValue(L"Option:CheckMEIDUnique", 1);

    return TRUE;
}

SPRESULT CWriteCodesEx::__PollAction(void)
{
	SPRESULT sp_result = SP_OK;
	CUeHelper ue(this);
	CMesHelper mes(this);

	MES_RESULT mes_result = mes.UnisocMesActive();
	if (MES_SUCCESS == mes_result)
	{
		// Get Input codes from Unisoc MES
		sp_result = mes.UnisocMesGetAssignedCodes(m_InputSN,sizeof(m_InputSN));
		if (SP_OK != sp_result)
		{
			return sp_result;
		}
	}
	else
	{
		// Get Input codes from share memory
		CHKRESULT_WITH_NOTIFY(GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN)), "GetShareMemory(InputCode)");

		// Check whether IMEI OR MEID is unique or not
		CHKRESULT(CheckUniqueGEID());
	}

    // Write SN
    for (INT i=BC_SN1; i<=BC_SN2; i++)
    {
        if (!m_InputSN[i].bEnable)
        {
            continue;
        }

        CHKRESULT(ue.WriteSN((BC_SN1 == i) ? SN1 : SN2, m_InputSN[i].szCode));
    }

    // Write GEID (IMEI & MEID) for UMS312
    PC_GEID_T GEID;
    CONST INT MAX_GEID_COUNT = 6;
    struct 
    {
        BC_INDEX bcIndex;
        INT Mask;
        CHAR* pChars;
    } arrayGEID[MAX_GEID_COUNT] = {
        {BC_IMEI1, GEIDMASK_RW_IMEI1, GEID.IMEI1},
        {BC_IMEI2, GEIDMASK_RW_IMEI2, GEID.IMEI2},
        {BC_IMEI3, GEIDMASK_RW_IMEI3, GEID.IMEI3},
        {BC_IMEI4, GEIDMASK_RW_IMEI4, GEID.IMEI4},
        {BC_MEID1, GEIDMASK_RW_MEID1, GEID.MEID1},
        {BC_MEID2, GEIDMASK_RW_MEID2, GEID.MEID2}
    };
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (!m_InputSN[i].bEnable)
        {
            continue;
        }

        for (INT j=0; j<MAX_GEID_COUNT; j++)
        {
            if (i == arrayGEID[j].bcIndex)
            {
                GEID.u32Mask |= arrayGEID[j].Mask;
                strncpy_s(arrayGEID[j].pChars, MAX_GEID_STR_LENGTH, m_InputSN[i].szCode, MAX_GEID_STR_LENGTH - 1);
            }
        }
    }
    if (0 != GEID.u32Mask)
    {
        CHKRESULT(ue.WriteGEID(GEID));
    }

    // Write BT & WIFI MAC address
    if (m_InputSN[BC_BT].bEnable)
    {
        CHKRESULT(ue.WriteBTAddrByAT(m_InputSN[BC_BT].szCode));
    }

    if (m_InputSN[BC_WIFI].bEnable)
    {
        CHKRESULT(ue.WriteWIFIAddrByAT(m_InputSN[BC_WIFI].szCode));
    }

    BOOL bOK = TRUE;
    CHKRESULT(SetShareMemory(ShareMemory_My_UpdateSN, (void* )&bOK, sizeof(bOK), IContainer::System));

    CHKRESULT(SaveDataIntoLocal());
    CHKRESULT(UpdateSectionAllocCodes());

    return SP_OK;
}

SPRESULT CWriteCodesEx::CheckUniqueGEID(void)
{
    if (!m_bCheckUniqueMEID && !m_bCheckUniqueIMEI)
    {
        return SP_OK;
    }

    BOOL bSaveGEID = FALSE;
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (m_InputSN[i].bEnable && IS_GEID_CODE(i))
        {
            bSaveGEID = TRUE;
            break;
        }
    }
    if (!bSaveGEID)
    {
        return SP_OK;
    }

    CDBHelper db(m_strMDBConn, GetISpLogObject());
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (!(m_InputSN[i].bEnable && IS_GEID_CODE(i)))
        {
            continue;
        }

        if (!m_bCheckUniqueMEID && IS_MEID_CODE(i))
        {
            continue;
        }
        if (!m_bCheckUniqueIMEI && IS_IMEI_CODE(i))
        {
            continue;
        }

        CHAR szSQL[256] = {0};
        sprintf_s(szSQL, "SELECT * FROM [IMEI] WHERE [%s] = \'%s\'", CBarCodeUtility::m_BarCodeInfo[i].nameA, m_InputSN[i].szCode);

        LONG lCount = 0;
        std::string   strSQL = szSQL;
        if (db.QueryCount(strSQL, lCount) && lCount > 0)
        {
            if (IS_IMEI_CODE(i))
            {
                NOTIFY("IMEI UNIQUE CHECK", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "%s: %s is used", CBarCodeUtility::m_BarCodeInfo[i].nameA, m_InputSN[i].szCode);
                return SP_E_CHECK_WRITEX_IMEI_IS_USED;
            }
            else
            {
                NOTIFY("MEID UNIQUE CHECK", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "%s: %s is used", CBarCodeUtility::m_BarCodeInfo[i].nameA, m_InputSN[i].szCode);
                return SP_E_CHECK_WRITEX_MEID_IS_USED;
            }
        }
    }

    return SP_OK;
}