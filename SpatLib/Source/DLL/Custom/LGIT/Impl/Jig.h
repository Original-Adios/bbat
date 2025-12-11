#pragma once
#include <Windows.h>
#include <vector>
#include "ICommChannel.h"
#include "ISpLogExport.h"

typedef enum
{
    JIG_OK = 0,
    JIG_CREATE_UART_DRIVER_FAIL,
    JIG_UART_OPEN_FAIL,
    JIG_UART_SEND_DATA_FAIL

} JIG_ERROR;

//////////////////////////////////////////////////////////////////////////
class CJig sealed
{
public:
    CJig(ISpLog* pLogUtil);
   ~CJig(void);

    /*
        Connect JIG port and send jig start command.
        @param - dwPort [IN]: Specifies the UART port number.
        @param - dwBps  [IN]: Specifies the UART port baud rate.
        @param - bPass  [IN]: Specifies the test result.
        @return: SP_OK if success, otherwise failure.
    */
    JIG_ERROR Start(DWORD dwPort, DWORD dwBps);
    JIG_ERROR Close(DWORD dwPort, DWORD dwBps, BOOL bPass);

    CONST CHAR* GetErrMsg(CONST JIG_ERROR& e);

private:
    // Convert string command to binary command (ASCII)
    void GetJigData(CONST CHAR* pszCmd, std::vector<BYTE>& data);

private:
    static CONST CHAR* JIG_OPEN_CMD;
    static CONST CHAR* JIG_PASS_CMD;
    static CONST CHAR* JIG_FAIL_CMD;
    static CONST CHAR* JIG_CLOSE_CMD;
    ISpLog* m_pLogUtil;
};


class CChannelFactory sealed
{
public:
     CChannelFactory(ICommChannel*& pChannel, CHANNEL_TYPE eType = CHANNEL_TYPE_COM)
         : m_pChannel(NULL)
     {
         CreateChannel(&pChannel, eType);
         m_pChannel = pChannel;
     }
    ~CChannelFactory(void)
     {
         if (NULL != m_pChannel)
         {
             ReleaseChannel(m_pChannel);
             m_pChannel = NULL;
         }
     }
private:
    ICommChannel* m_pChannel;
};