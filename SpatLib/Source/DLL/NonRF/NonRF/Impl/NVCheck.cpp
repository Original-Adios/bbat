#include "StdAfx.h"
#include "NVCheck.h"
#include "NVCheck/GsmFunc.h"
#include "NVCheck/TdFunc.h"
#include "NVCheck/WcdmaFunc.h"
#include "NVCheck/LteFunc.h"
#include "LteUtility.h"

using namespace std;

IMPLEMENT_RUNTIME_CLASS(CNVCheck)

const wchar_t* GSM_BAND[BI_GSM_MAX_BAND] = {L"GSM850", L"GSM", L"DCS", L"PCS"};
///
CNVCheck::CNVCheck(void)
{
    m_pFunc = NULL;
}

CNVCheck::~CNVCheck(void)
{
    if (NULL != m_pFunc)
    {
        delete m_pFunc;
        m_pFunc = NULL;
    }
}

BOOL CNVCheck::LoadXMLConfig(void)
{
    m_wstrType = GetConfigValue(L"Option:Type", L"PreCheck");
    wstring strVal = GetConfigValue(L"Option:NetMode", L"GSM");
    wstring strData;
    size_t nIndex;
    if (strVal.compare(L"GSM") == 0)
    {
        m_pFunc = new CGsmFunc(this);
        // load gsm parameter
        
        strVal = GetConfigValue(L"Option:GSM:NVType", L"CALI_ADC");

        strData = GetConfigValue((L"Param:GSM:"+strVal).c_str(), L"");
        nIndex = strData.find(',');
        if (nIndex != string::npos)
        {
            m_tNVData.nType = _wtoi(strData.substr(0, nIndex).c_str());
            m_tNVData.nLen = _wtoi(strData.substr(nIndex+1).c_str());
        }
        else
        {
            return FALSE;
        }
        strVal = GetConfigValue(L"Option:GSM:Band", L"GSM");
        for (int i = 0; i < ARRAY_SIZE(GSM_BAND); i++)
        {
            if (strVal.compare(GSM_BAND[i]) == 0)
            {
                m_tNVData.band = SP_BAND_INFO(i);
            }
        }
    }
    else if (strVal.compare(L"TD") == 0)
    {
        m_pFunc = new CTdFunc(this);
        // load td parameter
        strVal = GetConfigValue(L"Option:TD:NVType", L"CALI_APC");

        strData = GetConfigValue((L"Param:TD:"+strVal).c_str(), L"");
        nIndex = strData.find(',');
        if (nIndex != string::npos)
        {
            m_tNVData.nType = _wtoi(strData.substr(0, nIndex).c_str());
            m_tNVData.nLen = _wtoi(strData.substr(nIndex+1).c_str());
        }
        else
        {
            return FALSE;
        }
    }
    else if (strVal.compare(L"WCDMA") == 0)
    {
        m_pFunc = new CWcdmaFunc(this);
        // load wcdma parameter
        strVal = GetConfigValue(L"Option:WCDMA:NVType", L"CALI_AFC");

        strData = GetConfigValue((L"Param:WCDMA:"+strVal).c_str(), L"");
        nIndex = strData.find(',');
        if (nIndex != string::npos)
        {
            m_tNVData.nType = _wtoi(strData.substr(0, nIndex).c_str());
            m_tNVData.nLen = _wtoi(strData.substr(nIndex+1).c_str());
        }
        else
        {
            return FALSE;
        }
        strVal = GetConfigValue(L"Option:WCDMA:Band", L"Band1");
        for (int i = 0; i < BI_W_MAX_BAND; i++)
        {
            wstring strBand = L"Band";
            WCHAR szBuf[32] = {0};
            swprintf_s(szBuf, L"%d", i+1);
            strBand += szBuf;
            if (strVal.compare(strBand) == 0)
            {
                m_tNVData.band = SP_BAND_INFO(i);
                break;
            }
        }
    }
    else if (strVal.compare(L"LTE") == 0)
    {
        m_pFunc = new CLteFunc(this);
        // load lte parameter
        strVal = GetConfigValue(L"Option:LTE:NVType", L"CALI_FLAG");
        if (strVal.find(L"V2") != wstring::npos)
        {
            m_tNVData.eIfType = IF_V2;
        }
        else if (strVal.find(L"V3") != wstring::npos)
        {
            m_tNVData.eIfType = IF_V3;
        }
        else
        {
            m_tNVData.eIfType = IF_V1;
        }

        strData = GetConfigValue((L"Param:LTE:"+strVal).c_str(), L"");
        nIndex = strData.find(',');
        if (nIndex != string::npos)
        {
            m_tNVData.nType = _wtoi(strData.substr(0, nIndex).c_str());
            m_tNVData.nLen = _wtoi(strData.substr(nIndex+1).c_str());
        }
        else
        {
            return FALSE;
        }
        strVal = GetConfigValue(L"Option:LTE:Band", L"Band1");
        for (int i = 0; i < MAX_LTE_BAND; i++)
        {
            if (strVal.compare(CLteUtility::m_BandInfo[i].NameW) == 0)
            {
                m_tNVData.band = (SP_BAND_INFO)CLteUtility::m_BandInfo[i].Band;
                m_tNVData.reserved = CLteUtility::m_BandInfo[i].nIndicator;
            }
        }
    }

    
    return TRUE;
}

SPRESULT CNVCheck::__PollAction(void)
{
    if (m_wstrType.compare(L"PreCheck") == 0)
    {
        return m_pFunc->PreCheck();
    }

    return m_pFunc->Check();
}
