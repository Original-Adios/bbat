#include "StdAfx.h"
#include "SLData.h"
#include "callback_def.h"
#include "SLDriver.h"
#include "ISpLogExport.h"
#include <algorithm>
#include <regex>

//////////////////////////////////////////////////////////////////////////
CSLData::CSLData(CSLDriver* pDrv, ISpLog* pLogUtil)
    : m_pSLDrv(pDrv)
    , m_pLogUtil(pLogUtil)
    , m_bStopUpload(FALSE) // Should be FALSE to enable push data
{
}

CSLData::~CSLData(void)
{
}

void CSLData::Clear(void)
{
    m_LockQ.clear();
}

void CSLData::PushData(CONST ITEMDATA_T& item)
{
    if (!m_bStopUpload 
        && IS_BIT_SET(item.nLv, LEVEL_FT))
    {
        m_LockQ.push_back(item);
    }
}

void CSLData::run(void)
{
    std::string strName  = "";
    std::string strHead  = "";
    std::string strValue = "";
    std::string strField = "";
    enum
    {
        GSM = 0,
        TD,
        WCDMA,
        LTE,
        UNKNOWN
    } eMode;

    std::deque<ITEMDATA_T> dqe;

    while (m_LockQ.size() > 0)   
    { 
        INT nCount = m_LockQ.pop_all(dqe, INFINITE);

        INT nIndex = 0;
        while (nIndex < nCount)
        {
            CHAR szArfcn[32] = {0};
            CHAR szValue[32] = {0};

            ITEMDATA_T head = dqe[nIndex];
            std::string strBand = "";
            std::string strPCL  = "";
            if (NULL != strstr(head.szActionName, "GSM"))
            {
                eMode   = GSM;
                strBand = head.szBand;
            }
            else if (NULL != strstr(head.szActionName, "TD"))
            {
                eMode   =  TD;
                strBand = "TD";
            }
            else if (NULL != strstr(head.szActionName, "WCDMA"))
            {
                eMode   =  WCDMA;
                strBand = "WCDMA";
            }
            else if (NULL != strstr(head.szActionName, "LTE"))
            {
                eMode   =  LTE;
                strBand = "LTE";
            }
            else
            {
                eMode   = UNKNOWN;
                strBand = head.szBand;
            }
            
            sprintf_s(szArfcn, "%d", head.nChannel);
            if (GSM == eMode)
            {
                get_pcl_from_condition(head.szCond, strPCL);  
                strName  = (std::string)head.szActionName + "_" + strBand + "_" + szArfcn + "_" + strPCL;  
                strHead  = (std::string)"BAND=" + strBand + "|" + "CHANNEL=" + szArfcn + "|" + "PCL=" + strPCL;
            }
            else
            {
                strName  = (std::string)head.szActionName + "_" + strBand + "_" + szArfcn;  
                strHead  = (std::string)"BAND=" + strBand + "|" + "CHANNEL=" + szArfcn;
            }
            strField = (std::string)head.szItemName;
            sprintf_s(szValue, "%.2f", head.dValue);
            strValue = (std::string)szValue;

            for (INT j=++nIndex; j<nCount; nIndex++,j++)
            {
                BOOL bSameCond = FALSE;
                if (GSM == eMode)
                {
                    std::string pcl1 = "";
                    get_pcl_from_condition(dqe[j].szCond, pcl1);
                    bSameCond = (0 == strcmp(head.szBand, dqe[j].szBand) && head.nChannel == dqe[j].nChannel && strPCL == pcl1);
                }
                else
                {
                    bSameCond = (0 == strcmp(head.szBand, dqe[j].szBand) && head.nChannel == dqe[j].nChannel);
                }

                if (bSameCond)
                {
                    strField += (std::string)"," + dqe[j].szItemName;
                    sprintf_s(szValue, "%.2f", dqe[j].dValue);
                    strValue += (std::string)"," + szValue;
                }
                else
                {
                    if (!m_pSLDrv->SL_PutDatatoPhone(strName.c_str(), true, 0, strHead.c_str(), strField.c_str(), strValue.c_str()))
                    {
                        m_pLogUtil->LogFmtStrA(SPLOGLV_ERROR, "SL_Put fail %s", m_pSLDrv->SL_GetLastError());
                    }
                    if (NULL != m_pLogUtil)
                    {
                        m_pLogUtil->LogFmtStrA(SPLOGLV_VERBOSE, "[SLData] Name  : %s",  strName.c_str());
                        m_pLogUtil->LogFmtStrA(SPLOGLV_VERBOSE, "[SLData] Head  : %s",  strHead.c_str());
                        m_pLogUtil->LogFmtStrA(SPLOGLV_VERBOSE, "[SLData] Field : %s", strField.c_str());
                        m_pLogUtil->LogFmtStrA(SPLOGLV_VERBOSE, "[SLData] Value : %s", strValue.c_str());
                    }
                    break;
                }
            }
        } // end of while (nIndex < nCount)
    } // end of while (m_LockQ.size() > 0)   
    
    m_LockQ.clear();
}

BOOL CSLData::StartUpload(BOOL bStart)
{
    if (bStart)
    {
        m_bStopUpload = FALSE;
        return CThread::start();
    }
    else
    {
        m_bStopUpload = TRUE;
        m_LockQ.wakeup();
        CThread::join(INFINITE);
        return TRUE;
    }
}

std::string& CSLData::RemoveKeyword(std::string& str, const std::string& keyword)
{
    if (str.length() > 0)
    {
        trimA(str);
        std::transform(str.begin(), str.end(), str.begin(), tolower);
        std::string::size_type pos = str.find(keyword.c_str());
        if (0 == pos)
        {
            str.erase(pos, keyword.length());
            trimA(str);
        }
    }

    return str;
}

std::string& CSLData::trimA(std::string& str)
{
    std::string::size_type pos = str.find_last_not_of(' ');
    if (pos != std::string::npos) 
    {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if(pos != std::string::npos) 
        {
            str.erase(0, pos);
        }
    }
    else 
    {
        str.erase(str.begin(), str.end());
    }

    return str;
}

std::string& CSLData::get_pcl_from_condition(const std::string& cond, std::string& pcl)
{
    // PCL = x; AA = yy ....
    pcl = "-1";
    std::tr1::regex pattern("PCL\\s*=\\s*(\\d+)", std::tr1::regex::icase);
    std::string::const_iterator iter = cond.begin();
    std::tr1::smatch sm;
    while (std::tr1::regex_search(cond.begin(), cond.end(), sm, pattern))
    {
        pcl = sm[1];
        return pcl;
    }

    return pcl;
}

