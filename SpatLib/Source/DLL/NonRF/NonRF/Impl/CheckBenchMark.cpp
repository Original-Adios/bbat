#include "StdAfx.h"
#include "CheckBenchMark.h"
#include <regex>
#include "../../../../../Common/thread/Thread.h"
#include <functional>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckBenchMark)

///
CCheckBenchMark::CCheckBenchMark(void)
: m_dwSpan(0)
, m_bFinished(FALSE)
{
}

CCheckBenchMark::~CCheckBenchMark(void)
{
}

BOOL CCheckBenchMark::LoadXMLConfig(void)
{
    m_strArgs = _W2CA(GetConfigValue(L"Option:BenchMark", L"T10D5"));

    // T*D??    *: time, ?? percent
    // T05D10    unmatch
    // T5D51     matched
    // T10D05    unmatch
    // T10D5     matched
    std::regex  re("^T([1-9][0-9]*)D([1-9][0-9]?)$");
    std::smatch results;
    if (std::regex_match(m_strArgs, results, re) && 3 == results.size())
    {
        m_dwSpan = atoi(results[1].str().c_str());
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void CCheckBenchMark::DisplayProgress(void)
{
    m_bFinished = FALSE;

    INT nProgVal = 0;
    INT nElapsed = 1;
    while (!m_bFinished && nProgVal < 100)
    {
        /*
        // Here seems no effect, because it will be blocked by SendAT() function
        if (_IsUserStop())
        {
            break;
        }
        */
        nProgVal = 100 * nElapsed / m_dwSpan;
        SendProgress(nProgVal);
        nElapsed++;

        // Every 1 seconds update the progress
        Sleep(1000);
    }
}

SPRESULT CCheckBenchMark::__PollAction(void)
{
    std::string strCmd = (std::string)"AT+BENCHMARK=" + m_strArgs;
    std::string strRsp = "";

    // Start a thread to show the progress if user configures a long test time
    CThread thread;
    thread.bind_runner(std::bind(&CCheckBenchMark::DisplayProgress, this));
    thread.start();

    // Add extra 10 seconds to wait the test finished.
    SPRESULT res = SendAT(strCmd.c_str(), strRsp, 1, 200, m_dwSpan*1000+TIMEOUT_10S);
    
    // Test is finished, notify thread to stop.
    m_bFinished = TRUE;
    thread.join();
    
    if (SP_OK == res && std::string::npos != strRsp.find("Ucpubench Benchmark test Pass"))
    {
        replace_all(strRsp, "Ucpubench Benchmark test Pass", "");
        replace_all(strRsp, "\r\n", "");
        NOTIFY("BenchMark", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
        return SP_OK;
    }
    else
    {
        NOTIFY("BenchMark", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
        return (SP_OK != res) ? res : SP_E_FAIL;
    }
}

