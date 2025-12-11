#include "StdAfx.h"
#include "ReadSN.h"
#include "ShareMemoryDefine.h"
#include "CLocks.h"
#include "BarCodeUtility.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CReadSN)

/// 
CReadSN::CReadSN(void)
: m_bSN1(TRUE)
, m_bSN2(FALSE)
{
    ZeroMemory((void* )m_szSN1, sizeof(m_szSN1));
    ZeroMemory((void* )m_szSN2, sizeof(m_szSN2));
}

CReadSN::~CReadSN(void)
{
}

BOOL CReadSN::LoadXMLConfig(void)
{
    m_bSN1 = GetConfigValue(L"Option:SN1", 1);
    m_bSN2 = GetConfigValue(L"Option:SN2", 2);
    return TRUE;
}

SPRESULT CReadSN::__PollAction(void)
{
    SPRESULT res = SP_OK;
    if (m_bSN1)
    {
        res = SP_LoadSN(m_hDUT, SN1, m_szSN1, sizeof(m_szSN1));
        if (SP_OK != res)
        {
            if (SP_E_PHONE_GOLDEN_SAMPLE == res)
            {
                // Golden Sample is verified by CCheckGoldenSample class.
                // Here we just skip to load SN1 and SN2, because there is no meaning to parse the SN for golen sample.
                return SP_OK;
            }
            /* Replaced by CCheckGoldenSample
            if (SP_E_PHONE_GOLDEN_SAMPLE == res)
            {
                NOTIFY("GoldenSample", LEVEL_UI, 1, 0, 1);
            }
            else
            {
                NOTIFY("LoadSN1", LEVEL_UI, 1, 0, 1);
            }
            */
            return res;
        }

        SetShareMemory(ShareMemory_SN1, (const void* )&m_szSN1[0], ShareMemory_SN_SIZE);
        string strInfo = m_szSN1;
        NOTIFY(DUT_INFO_SN1, (strInfo).c_str());
        NOTIFY("SN1", LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, NULL, m_szSN1);
        NOTIFY("SN1", LEVEL_REPORT|LEVEL_INFO, NOLOWLMT, 1, NOUPPLMT, NULL, -1, NULL, ("'" +strInfo).c_str());
    }
    
    if (SP_OK == res && m_bSN2)
    {
        res = SP_LoadSN(m_hDUT, SN2, m_szSN2, sizeof(m_szSN2));
        if (SP_OK != res)
        {
            if (SP_E_PHONE_GOLDEN_SAMPLE == res)
            {
                // Golden Sample is verified by CCheckGoldenSample class.
                // Here we just skip to load SN1 and SN2, because there is no meaning to parse the SN for golen sample.
                return SP_OK;
            }
            /* Replaced by CCheckGoldenSample
            if (SP_E_PHONE_GOLDEN_SAMPLE == res)
            {
                NOTIFY("GoldenSample", LEVEL_UI, 1, 0, 1);
            }
            else
            {
                NOTIFY("LoadSN2", LEVEL_UI, 1, 0, 1);
            }
            */
            return res;
        }

        SetShareMemory(ShareMemory_SN2, (const void* )&m_szSN2[0], sizeof(m_szSN2));
        string strInfo = m_szSN2;
        NOTIFY(DUT_INFO_SN2, (strInfo).c_str());
        NOTIFY("SN2", LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, NULL, (strInfo).c_str());
        NOTIFY("SN2", LEVEL_REPORT|LEVEL_INFO, NOLOWLMT, 1, NOUPPLMT, NULL, -1, NULL, ("'" +strInfo).c_str());
		NOTIFY(DUT_INFO_SN2, (strInfo).c_str());
    }

    return res;
}
