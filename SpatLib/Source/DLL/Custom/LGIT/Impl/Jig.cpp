#include "StdAfx.h"
#include "Jig.h"
#include <assert.h>
#pragma comment(lib, "Channel9.lib")

//
// Delay 4sec to close the jig
#define JIG_STABLE_TIME         (4000)
#define SAFE_JIG_LOG            if (NULL != m_pLogUtil)  m_pLogUtil

//////////////////////////////////////////////////////////////////////////
CONST CHAR* CJig::JIG_OPEN_CMD  = "00WSS0107%DW00010008";
CONST CHAR* CJig::JIG_PASS_CMD  = "00WSS0107%DW00010004";
CONST CHAR* CJig::JIG_FAIL_CMD  = "00WSS0107%DW00010002";
CONST CHAR* CJig::JIG_CLOSE_CMD = JIG_OPEN_CMD;

///
CJig::CJig(ISpLog* pLogUtil)
    : m_pLogUtil(pLogUtil)
{

}

CJig::~CJig(void)
{
}

JIG_ERROR CJig::Start(DWORD dwPort, DWORD dwBps)
{
    // Create UART data transmitter driver instance
    ICommChannel* pDrv = NULL;
    CChannelFactory Factory(pDrv);
    if (NULL == pDrv)
    {
        return JIG_CREATE_UART_DRIVER_FAIL;
    }

    // Open UART
    CHANNEL_ATTRIBUTE ca;
    ca.ChannelType    = CHANNEL_TYPE_COM;
    ca.Com.dwPortNum  = dwPort;
    ca.Com.dwBaudRate = dwBps;
    if (!pDrv->Open(&ca))
    {
        return JIG_UART_OPEN_FAIL;
    }

    // Send JIG Open Command
    std::vector<BYTE> data(0);
    GetJigData(JIG_OPEN_CMD, data);
    DWORD dwSize = data.size();
    SAFE_JIG_LOG->LogBufData(SPLOGLV_INFO, (LPCVOID)&data[0], dwSize, LOG_WRITE);
    if (dwSize != pDrv->Write((LPVOID)&data[0], dwSize))
    {
        pDrv->Close();
        SAFE_JIG_LOG->LogFmtStrA(SPLOGLV_ERROR, "Send JIG command <%s> fail!", JIG_OPEN_CMD);
        return JIG_UART_SEND_DATA_FAIL;
    }
    
    pDrv->Close();

    // Delay 4sec to close the jig
    Sleep(JIG_STABLE_TIME);

    return JIG_OK;
}

JIG_ERROR CJig::Close(DWORD dwPort, DWORD dwBps, BOOL bPass)
{
    ICommChannel* pDrv = NULL;
    CChannelFactory Factory(pDrv);
    if (NULL == pDrv)
    {
        return JIG_CREATE_UART_DRIVER_FAIL;
    }

    CHANNEL_ATTRIBUTE ca;
    ca.ChannelType    = CHANNEL_TYPE_COM;
    ca.Com.dwPortNum  = dwPort;
    ca.Com.dwBaudRate = dwBps;
    if (!pDrv->Open(&ca))
    {
        return JIG_UART_OPEN_FAIL;
    }

    // Send Jig PASS/FAIL Command
    std::vector<BYTE> data(0);
    GetJigData(bPass ? JIG_PASS_CMD : JIG_FAIL_CMD, data);
    DWORD dwSize = data.size();
    SAFE_JIG_LOG->LogBufData(SPLOGLV_INFO, (LPCVOID)&data[0], dwSize, LOG_WRITE);
    if (dwSize != pDrv->Write((LPVOID)&data[0], dwSize))
    {
        pDrv->Close();
        SAFE_JIG_LOG->LogFmtStrA(SPLOGLV_ERROR, "Send JIG command <%s> fail!", bPass ? JIG_PASS_CMD : JIG_FAIL_CMD);
        return JIG_UART_SEND_DATA_FAIL;
    }

    // Delay 4sec to close the jig
    Sleep(JIG_STABLE_TIME);

    // Send Jig Close Command
    GetJigData(JIG_CLOSE_CMD, data);
    dwSize = data.size();
    SAFE_JIG_LOG->LogBufData(SPLOGLV_INFO, (LPCVOID)&data[0], dwSize, LOG_WRITE);
    if (dwSize != pDrv->Write((LPVOID)&data[0], dwSize))
    {
        pDrv->Close();
        SAFE_JIG_LOG->LogFmtStrA(SPLOGLV_ERROR, "Send JIG command <%s> fail!", JIG_CLOSE_CMD);
        return JIG_UART_SEND_DATA_FAIL;
    }
    
    pDrv->Close();
    
    // Delay 4sec to close the jig
    Sleep(JIG_STABLE_TIME);

    return JIG_OK;
}

void CJig::GetJigData(CONST CHAR* pszCmd, std::vector<BYTE>& data)
{
    CONST BYTE ENQ = 0x05;
    CONST BYTE EOT = 0x04;
    INT nLen = strlen(pszCmd);
    data.resize(1/*ENQ*/ + nLen + 1/*EOT*/);

    INT  i = 0;
    data[i++] = ENQ;

    for (INT j=0; j<nLen; j++)
    {
        data[i++] = (BYTE)((INT)pszCmd[j]);
    }

    data[i++] = EOT;
}

CONST CHAR* CJig::GetErrMsg(CONST JIG_ERROR& e)
{
    switch(e)
    {
    case JIG_OK:
        return "OK";
    case JIG_CREATE_UART_DRIVER_FAIL:
        return "Create JIG UART driver fail";
    case JIG_UART_OPEN_FAIL:
        return "Open JIG UART fail";
    case JIG_UART_SEND_DATA_FAIL:
        return "Send JIG command fail";
    default:
        return "Unknown JIG error";
    }
}
