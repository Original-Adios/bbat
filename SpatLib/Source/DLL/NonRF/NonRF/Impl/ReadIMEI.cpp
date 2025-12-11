#include "StdAfx.h"
#include "ReadIMEI.h"
#include "SharedDefine.h"

IMPLEMENT_RUNTIME_CLASS(CReadIMEI)

CReadIMEI::CReadIMEI(void)
{
}

CReadIMEI::~CReadIMEI(void)
{
}

BOOL CReadIMEI::LoadXMLConfig(void)
{
    std::wstring strKEY[] = {L"Option:IMEI1", L"Option:IMEI2", L"Option:IMEI3", L"Option:IMEI4"};
    std::wstring strKeyName;
    std::wstring strBuf;
    for (INT i=0; i<4; i++)
    {
        strKeyName = strKEY[i] + L":Enable";
        m_ReadIMEI[i].bEnable = (BOOL)GetConfigValue(strKeyName.c_str(), 1);

        //strKeyName = strKEY[i] + L":Check" + L":CheckEnable";
        //m_ReadIMEI[i].bCheck = (BOOL)GetConfigValue(strKeyName.c_str(), 0);

        strKeyName = strKEY[i] + L":Check:Prefix";
        strBuf = GetConfigValue(strKeyName.c_str(), L"");
        WideCharToMultiByte(CP_ACP, 0, strBuf.c_str(), -1, m_ReadIMEI[i].szPrefix, MAX_IMEI_STR_LENGTH, 0, 0);

        strKeyName = strKEY[i] + L":Check:CheckBarcode";
        strBuf = GetConfigValue(strKeyName.c_str(), L"NONE");
        for (INT j=0; j< MAX_BARCODE_SOURCE; j++)
        {
            if (0 == strBuf.compare(g_BARCODE_SOURCE[j]))
            {
                m_ReadIMEI[i].emBarcodeSource = (EM_BARCODE_SOURCE)j;
                break;
            }   
        }
    }

    return TRUE;
}

SPRESULT CReadIMEI::__PollAction(void)
{
    BARCODE_T InputBarcode;
    SPRESULT res = SP_OK;
    for (INT i=0; i<4; i++)
    {
        if (m_ReadIMEI[i].emBarcodeSource != BARCODE_NONE)
        {
            res = GetShareMemory(ShareMemory_My_UserInputBarcode, (void* )&InputBarcode, sizeof(InputBarcode));
            if (SP_OK != res)
            {
                NOTIFY("GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "InputBarCode");
                return res;
            }
            break;
        }
    }

    for (INT i=0; i<4; i++)
    {
        if (m_ReadIMEI[i].emBarcodeSource != BARCODE_NONE)
        {
            strcpy_s(m_ReadIMEI[i].szCheckCodesFormUI, InputBarcode.szBarcode[m_ReadIMEI[i].emBarcodeSource-1]);
        }
    }

    PC_PRODUCT_DATA ProductData;
    ZeroMemory(&ProductData, sizeof(PC_PRODUCT_DATA));
    if (m_ReadIMEI[0].bEnable)
    {
        ProductData.u32OperMask |= FNAMASK_RW_IMEI1;
    }

    if (m_ReadIMEI[1].bEnable)
    {
        ProductData.u32OperMask |= FNAMASK_RW_IMEI2;
    }

    if (m_ReadIMEI[2].bEnable)
    {
        ProductData.u32OperMask |= FNAMASK_RW_IMEI3;
    }

    if (m_ReadIMEI[3].bEnable)
    {
        ProductData.u32OperMask |= FNAMASK_RW_IMEI4;
    }

    if (ProductData.u32OperMask != 0)
    {
        CHKRESULT_WITH_NOTIFY(SP_LoadProductData(m_hDUT, &ProductData, TIMEOUT_3S), "SP_LoadProductData");

        strcpy_s(m_ReadIMEI[0].szReadIMEI, (char* )ProductData.szImei1);
        strcpy_s(m_ReadIMEI[1].szReadIMEI, (char* )ProductData.szImei2);
        strcpy_s(m_ReadIMEI[2].szReadIMEI, (char* )ProductData.szImei3);
        strcpy_s(m_ReadIMEI[3].szReadIMEI, (char* )ProductData.szImei4);
    }

    
    for (INT i = 0; i<4; i++)
    {
        CHAR szItemName[64] = {0};
        // Show Read IMEI
        if (m_ReadIMEI[i].bEnable)
        {
            sprintf_s(szItemName, "IMEI%d", i+1);
            NOTIFY(szItemName, LEVEL_UI, 1, 1, 1, NULL, -1, "-", m_ReadIMEI[i].szReadIMEI);
        }

        // Check IMEI
        std::string strBuf;
        strBuf = m_ReadIMEI[i].szReadIMEI;
        INT nPrefixLen = strlen(m_ReadIMEI[i].szPrefix);
        if (nPrefixLen != 0)
        {
            sprintf_s(szItemName, "CheckIMEI%dPrefix", i+1);
            if (0 != strBuf.compare(0, nPrefixLen, m_ReadIMEI[i].szPrefix))
            {
                NOTIFY(szItemName, LEVEL_UI, 1, 0, 1, NULL, -1, "-", m_ReadIMEI[i].szPrefix);
                return SP_E_SPAT_TEST_FAIL;
            }
            else
            {
                NOTIFY(szItemName, LEVEL_UI, 1, 1, 1, NULL, -1, "-", m_ReadIMEI[i].szPrefix);
            }
        }

        if (m_ReadIMEI[i].emBarcodeSource != BARCODE_NONE)
        {
            sprintf_s(szItemName, "CheckIMEI%d", i+1);
            if (0 != strBuf.compare(m_ReadIMEI[i].szCheckCodesFormUI))
            {
                NOTIFY(szItemName, LEVEL_UI, 1, 0, 1, NULL, -1, "-", m_ReadIMEI[i].szCheckCodesFormUI);
                return SP_E_SPAT_TEST_FAIL;
            }
            else
            {
                NOTIFY(szItemName, LEVEL_UI, 1, 1, 1, NULL, -1, "-", m_ReadIMEI[i].szCheckCodesFormUI);
            }
        }
    }

    if (IS_BIT_SET(ProductData.u32OperMask, FNAMASK_RW_IMEI1))
    {
        SetShareMemory(ShareMemory_IMEI1, (void* )&ProductData.szImei1, ShareMemory_IMEI_SIZE);
		NOTIFY(DUT_INFO_IMEI1, (const char*)ProductData.szImei1);
    }

	if (IS_BIT_SET(ProductData.u32OperMask, FNAMASK_RW_IMEI2))
	{
		SetShareMemory(ShareMemory_IMEI2, (void* )&ProductData.szImei2, sizeof(ProductData.szImei2));
		NOTIFY(DUT_INFO_IMEI2, (const char*)ProductData.szImei2);
	}

    return SP_OK;
}