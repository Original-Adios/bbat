#include "StdAfx.h"
#include "Sleep.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSleep)

///
CSleep::CSleep(void)
: m_u32SleepTime(0)
{

}

CSleep::~CSleep(void)
{
}

BOOL CSleep::LoadXMLConfig(void)
{
    m_u32SleepTime = GetConfigValue(L"Option:SleepTime", 0);
    return TRUE;
}

SPRESULT CSleep::__PollAction(void)
{ 
    CHAR szMsg[64] = {0};
    sprintf_s(szMsg, "Sleep(%d)", m_u32SleepTime);
    NOTIFY(szMsg, LEVEL_UI, 1, 1, 1, NULL, -1, "ms");

    CSPTimer timer;
    do
    {
        if (_IsUserStop())          
        {                           
            return SP_E_USER_ABORT; 
        }

        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))        
        {        
            if (msg.message == WM_QUIT) 
            {
                break;    
            }

            TranslateMessage(&msg);        
            DispatchMessage(&msg);        
        }        
        else        
        {        
            Sleep(10);      
        } 

    } while (!timer.IsTimeOut(m_u32SleepTime));

    return SP_OK;
}
