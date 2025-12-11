#include "StdAfx.h"
#include "CameraOper.h"

//
IMPLEMENT_RUNTIME_CLASS(CCameraOper)

//////////////////////////////////////////////////////////////////////////
CCameraOper::CCameraOper(void)
{
}

CCameraOper::~CCameraOper(void)
{

}

SPRESULT CCameraOper::__PollAction(void)
{
    if (m_bOpen)
    {
        CHKRESULT(CameraOpen());
    }
    if (m_bClose)
    {
        CHKRESULT(CameraClose());
    }
    return SP_OK;
}

BOOL CCameraOper::LoadXMLConfig(void)
{
    m_bOpen = GetConfigValue(L"Option:Camera_Open", 0);
    m_bClose = GetConfigValue(L"Option:Camera_Close", 0);
    m_bMcu = GetConfigValue(L"Option:MCU", 0);
    return TRUE;
}

SPRESULT CCameraOper::CameraOpen()
{
    if (m_bMcu)
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_McuCameraOpen(m_hDUT), "MCU Camera Open", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "MCU Camera Open", LEVEL_ITEM);

    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_MipiCameraOpen(m_hDUT), "Camera Open", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Camera Open", LEVEL_ITEM);

    }
    return SP_OK;
}

SPRESULT CCameraOper::CameraClose()
{
    if (m_bMcu)
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_McuCameraClose(m_hDUT), "MCU Camera Close", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "MCU Camera Close", LEVEL_ITEM);
    }
    else
    {
        CHKRESULT(PrintErrorMsg(
            SP_BBAT_MipiCameraClose(m_hDUT), "Camera Close", LEVEL_ITEM));
        PrintSuccessMsg(SP_OK, "Camera Close", LEVEL_ITEM);

    }
   return SP_OK;
}


