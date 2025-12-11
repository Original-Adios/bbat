#include "StdAfx.h"
#include "UeHelper.h"
#include <assert.h>
//////////////////////////////////////////////////////////////////////////
CUeHelper::CUeHelper(CImpBase* pImp)
: m_pImp(pImp)
, m_hDUT(pImp->m_hDUT)
{
}

CUeHelper::~CUeHelper(void)
{
}

SPRESULT CUeHelper::SendAT(LPCSTR lpszAT, std::string& strRsp, \
    uint32 u32RetryCount /* = 1 */, uint32 u32Interval /* = 200 */,uint32 u32TimeOut /* = TIMEOUT_3S */
    )
{
    SPRESULT res = SP_OK;
    CHAR   recvBuf[4096] = {0};
    uint32 recvSize = 0;
    for (uint32 i=0; i<u32RetryCount; i++)
    {
        res = SP_SendATCommand(m_hDUT, lpszAT, TRUE, recvBuf, sizeof(recvBuf), &recvSize, u32TimeOut);
        if (SP_OK == res)
        {
            if (NULL != strstr(recvBuf, "+CME ERROR"))
            {
                res = SP_E_PHONE_AT_EXECUTE_FAIL;
            }
            else
            {
                strRsp = recvBuf;
                m_pImp->replace_all(strRsp, "\r", "");
                m_pImp->replace_all(strRsp, "\n", "");
                //  m_pImp->replace_all(strRsp, "OK", "");
                m_pImp->trimA(strRsp);

                return SP_OK;
            }
        }

        Sleep(u32Interval);
    }

    return res;
}

SPRESULT CUeHelper::WriteSN(SN_TYPE eSN, LPCSTR lpSN)
{
    assert(lpSN);

    LPCSTR ITEM_NAME = (SN1 == eSN) ? "WriteSN1" : "WriteSN2";
    SPRESULT res = SP_SaveSN(m_hDUT, eSN, lpSN, strlen(lpSN));
    m_pImp->NOTIFY(ITEM_NAME, (SP_OK == res) ? LEVEL_UI : (LEVEL_UI|LEVEL_INFO), 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, ITEM_NAME);
    m_pImp->NOTIFY(ITEM_NAME, LEVEL_REPORT, 1, (SP_OK == res) ? 1 : 0, 1, NULL, -1, NULL, "\'%s", ITEM_NAME);  // ' prefix is for CSV display
    if (SP_OK == res)
    {
        m_pImp->NOTIFY((SN1 == eSN) ? DUT_INFO_SN1 : DUT_INFO_SN2, lpSN);
        res = m_pImp->SetShareMemory((SN1 == eSN) ? ShareMemory_SN1 : ShareMemory_SN2, (LPCVOID)lpSN, ShareMemory_SN_SIZE);
    }

    return res;
}

SPRESULT CUeHelper::WriteProductData(CONST PC_PRODUCT_DATA& data)
{
    SPRESULT res = SP_SaveProductData(m_hDUT, &data, TIMEOUT_3S);
    if (SP_OK != res)
    {
        m_pImp->NOTIFY("SP_SaveProductData", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return res;
    }
    else
    {
        if (IS_BIT_SET(data.u32OperMask, FNAMASK_RW_IMEI1))
        {
            m_pImp->NOTIFY("WriteIMEI1", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  (CHAR* )data.szImei1);
            m_pImp->NOTIFY("WriteIMEI1", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", (CHAR* )data.szImei1);
            m_pImp->SetShareMemory(ShareMemory_IMEI1, (const void* )data.szImei1, ShareMemory_IMEI_SIZE);
        }

        if (IS_BIT_SET(data.u32OperMask, FNAMASK_RW_IMEI2))
        {
            m_pImp->NOTIFY("WriteIMEI2", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  (CHAR* )data.szImei2);
            m_pImp->NOTIFY("WriteIMEI2", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", (CHAR* )data.szImei2);
            m_pImp->SetShareMemory(ShareMemory_IMEI2, (const void* )data.szImei2, ShareMemory_IMEI_SIZE);
        }

        if (IS_BIT_SET(data.u32OperMask, FNAMASK_RW_IMEI3))
        {
            m_pImp->NOTIFY("WriteIMEI3", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  (CHAR* )data.szImei3);
            m_pImp->NOTIFY("WriteIMEI3", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", (CHAR* )data.szImei3);
        }

        if (IS_BIT_SET(data.u32OperMask, FNAMASK_RW_IMEI4))
        {
            m_pImp->NOTIFY("WriteIMEI4", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  (CHAR* )data.szImei4);
            m_pImp->NOTIFY("WriteIMEI4", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", (CHAR* )data.szImei4);
        }

        if (IS_BIT_SET(data.u32OperMask, FNAMASK_RW_BTADDR))
        {
            m_pImp->NOTIFY("WriteBT", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  (CHAR* )data.szBTAddr);
            m_pImp->NOTIFY("WriteBT", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", (CHAR* )data.szBTAddr);
        }

        if (IS_BIT_SET(data.u32OperMask, FNAMASK_RW_WIFIADDR))
        {
            m_pImp->NOTIFY("WriteWIFI", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  (CHAR* )data.szWIFIAddr);
            m_pImp->NOTIFY("WriteWIFI", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", (CHAR* )data.szWIFIAddr);
        }
    }

    return res;
}

SPRESULT CUeHelper::WriteGEID(CONST PC_GEID_T& GEID)
{
    SPRESULT res = SP_SaveGEID(m_hDUT, &GEID, TIMEOUT_30S);
    if (SP_OK != res)
    {
        m_pImp->NOTIFY("SP_SaveGEID", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return res;
    }
    else
    {
        if (IS_BIT_SET(GEID.u32Mask, GEIDMASK_RW_IMEI1))
        {
            m_pImp->NOTIFY("WriteIMEI1", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  GEID.IMEI1);
            m_pImp->NOTIFY("WriteIMEI1", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", GEID.IMEI1);
            m_pImp->SetShareMemory(ShareMemory_IMEI1, (const void* )GEID.IMEI1, ShareMemory_IMEI_SIZE);
        }

        if (IS_BIT_SET(GEID.u32Mask, GEIDMASK_RW_IMEI2))
        {
            m_pImp->NOTIFY("WriteIMEI2", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  GEID.IMEI2);
            m_pImp->NOTIFY("WriteIMEI2", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", GEID.IMEI2);
            m_pImp->SetShareMemory(ShareMemory_IMEI2, (const void* )GEID.IMEI2, ShareMemory_IMEI_SIZE);
        }

        if (IS_BIT_SET(GEID.u32Mask, GEIDMASK_RW_IMEI3))
        {
            m_pImp->NOTIFY("WriteIMEI3", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  GEID.IMEI3);
            m_pImp->NOTIFY("WriteIMEI3", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", GEID.IMEI3);
        }

        if (IS_BIT_SET(GEID.u32Mask, GEIDMASK_RW_IMEI4))
        {
            m_pImp->NOTIFY("WriteIMEI4", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  GEID.IMEI4);
            m_pImp->NOTIFY("WriteIMEI4", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", GEID.IMEI4);
        }

        if (IS_BIT_SET(GEID.u32Mask, GEIDMASK_RW_MEID1))
        {
            m_pImp->NOTIFY("WriteMEID1", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  GEID.MEID1);
            m_pImp->NOTIFY("WriteMEID1", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", GEID.MEID1);
            m_pImp->SetShareMemory(ShareMemory_MEID1, (const void* )GEID.MEID1, ShareMemory_MEID_SIZE);
        }

        if (IS_BIT_SET(GEID.u32Mask, GEIDMASK_RW_MEID2))
        {
            m_pImp->NOTIFY("WriteMEID2", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  GEID.MEID2);
            m_pImp->NOTIFY("WriteMEID2", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", GEID.MEID2);
            m_pImp->SetShareMemory(ShareMemory_MEID2, (const void* )GEID.MEID2, ShareMemory_MEID_SIZE);
        }
    }

    return res;
}

SPRESULT CUeHelper::WriteBTAddrByAT(LPCSTR lpAddr)
{
    assert(lpAddr);

    // Android 9.x and above Support
    // AT+SPBTTEST=MAC,491307135821
    // +SPBTTEST:OK
    // OK    
    std::string strCmd = (std::string)"AT+SPBTTEST=MAC," + lpAddr;
    std::string strRsp = "";
    SPRESULT res = SendAT(strCmd.c_str(), strRsp);
    if (SP_OK != res || strRsp.find("+SPBTTEST:OK") == std::string::npos)
    {
        m_pImp->NOTIFY(strCmd.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
        res = SP_E_PHONE_AT_EXECUTE_FAIL;
    }
    else
    {
        m_pImp->NOTIFY("WriteBT", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  lpAddr);
        m_pImp->NOTIFY("WriteBT", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", lpAddr);
    }

    return res;
}

SPRESULT CUeHelper::WriteWIFIAddrByAT(LPCSTR lpAddr)
{
    assert(lpAddr);

    // Android 9.x and above Support
    // AT+SPWIFITEST=MAC,491307135821
    // +SPWIFITEST:OK
    // OK    
    std::string strCmd = (std::string)"AT+SPWIFITEST=MAC," + lpAddr;
    std::string strRsp = "";
    SPRESULT res = SendAT(strCmd.c_str(), strRsp);
    if (SP_OK != res || strRsp.find("+SPWIFITEST:OK") == std::string::npos)
    {
        m_pImp->NOTIFY(strCmd.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
        res = SP_E_PHONE_AT_EXECUTE_FAIL;
    }
    else
    {
        m_pImp->NOTIFY("WriteWIFI", LEVEL_UI|LEVEL_INFO, 1, 1, 1, NULL, -1, NULL,  lpAddr);
        m_pImp->NOTIFY("WriteWIFI", LEVEL_REPORT, 1, 1, 1, NULL, -1, NULL, "\'%s", lpAddr);
    }

    return res;
}


