#include "StdAfx.h"
#include "Camera.h"

//
IMPLEMENT_RUNTIME_CLASS(CCamera)

//////////////////////////////////////////////////////////////////////////
CCamera::CCamera(void)
{
    m_nCameraOpen = 0;
    m_nCameraClose = 0;
    m_nCameraRead = 0;
    m_nCameraId = 0;
    m_pCameraMsg = nullptr;
}

CCamera::~CCamera(void)
{

}

SPRESULT CCamera::__PollAction(void)
{
    SPRESULT Res = SP_OK;
    CHKRESULT(CameraSwitch());

    if (m_nCameraOpen)
    {
        CHKRESULT(CameraOpen());
    }

    if (m_nCameraRead)
    {
        Sleep(m_nCameraOpenDelay);
        Res = CameraRead();
    }

    if (m_nCameraClose)
    {
        CHKRESULT(CameraClose());
    }

    return Res;
}
BOOL CCamera::LoadXMLConfig(void)
{
    m_nCameraOpen = GetConfigValue(L"Option:Camera_Open", 0);
    m_nCameraClose = GetConfigValue(L"Option:Camera_Close", 0);
    m_nCameraRead = GetConfigValue(L"Option:Camera_Read", 0);
    m_nCameraId = GetConfigValue(L"Option:Camera_Id", 0);
    m_nCameraOpenDelay = GetConfigValue(L"Param:Camera_Open_Delay", 2500);
    return TRUE;
}

SPRESULT CCamera::CameraSwitch()
{
    BBAT_CAMERA_ID_BYTE byCameraId = CAMERA_0;
    switch (m_nCameraId)
    {
    case 0:
    {
        byCameraId = CAMERA_0;
        m_pCameraMsg = "CAMERA 0";
        break;
    }
    case 1:
    {
        byCameraId = CAMERA_1;
        m_pCameraMsg = "CAMERA 1";
        break;
    }
    case 2:
    {
        byCameraId = CAMERA_2;
        m_pCameraMsg = "CAMERA 2";
        break;
    }
    case 3:
    {
        byCameraId = CAMERA_3;
        m_pCameraMsg = "CAMERA 3";
        break;
    }
    case 4:
    {
        byCameraId = CAMERA_4;
        m_pCameraMsg = "CAMERA 4";
        break;
    }
    case 5:
    {
        byCameraId = CAMERA_5;
        m_pCameraMsg = "CAMERA 5";
        break;
    }
    }

    SPRESULT Res = SP_BBAT_CameraSwitch(m_hDUT, byCameraId);
    if (Res != SP_OK)
    {
        _UiSendMsg(m_pCameraMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Switch Fail");
        return Res;
    }
    _UiSendMsg(m_pCameraMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Switch Pass");
    return SP_OK;
}

SPRESULT CCamera::CameraOpen()
{
    SPRESULT Res = SP_BBAT_MipiCameraOpen(m_hDUT);
    if (Res != SP_OK)
    {
        _UiSendMsg(m_pCameraMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Open Fail");
        return Res;
    }
    _UiSendMsg(m_pCameraMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Open Pass");

    return SP_OK;
}

SPRESULT CCamera::CameraClose()
{
    SPRESULT Res = SP_BBAT_MipiCameraClose(m_hDUT);
    if (Res != SP_OK)
    {
        _UiSendMsg(m_pCameraMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Close Fail");
        return Res;
    }
    _UiSendMsg(m_pCameraMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Close Pass");

    return SP_OK;
}

SPRESULT CCamera::CameraRead()
{
    SPRESULT Res = SP_BBAT_MipiCameraRead(m_hDUT);
    if (Res != SP_OK)
    {
        _UiSendMsg(m_pCameraMsg, LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr, "Read Fail");
        return Res;
    }
    _UiSendMsg(m_pCameraMsg, LEVEL_ITEM, 0, 0, 0, nullptr, -1, nullptr, "Read Pass");

    return SP_OK;
}


