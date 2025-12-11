#include "StdAfx.h"
#include "CobAfcb.h"

//
IMPLEMENT_RUNTIME_CLASS(CCob)

CCob::CCob(void)
{
}
CCob::~CCob(void)
{
}

SPRESULT CCob::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return CobAction();
}

BOOL CCob::LoadXMLConfig(void)
{
    m_nSleep = GetConfigValue(L"Option:Sleep", 0);
    m_nUpSpecX = GetConfigValue(L"Option:X_Limit:UpSpec", 0);
    m_nDownSpecX = GetConfigValue(L"Option:X_Limit:DownSpec", 0);
    m_nUpSpecY = GetConfigValue(L"Option:Y_Limit:UpSpec", 0);
    m_nDownSpecY = GetConfigValue(L"Option:Y_Limit:DownSpec", 0);
    m_nTouchPen_Delay = GetConfigValue(L"Param:TouchPen_Delay", 500);
    m_nDutReadResult_Delay = GetConfigValue(L"Param:DutReadResult_Delay", 200);
    return TRUE;
}

SPRESULT CCob::CobAction()
{
    CHKRESULT(Phone_CobInit());
    CHKRESULT(AFCB_PowerOn());
    Sleep(m_nDutReadResult_Delay);
    SPRESULT Res = Phone_CobReadCom();
    CHKRESULT(AFCB_PowerOff());
    return Res;
}

SPRESULT CCob::Phone_CobInit()
{
    CHKRESULT(PrintErrorMsg(SP_BBAT_CobInit(m_hDUT), "Phone: Cob Init", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "Phone: Cob Init", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT CCob::Phone_CobReadCom()
{
    CobReadSigResult DataRecv;
    memset(&DataRecv, NULL, sizeof(DataRecv));

    CHKRESULT(PrintErrorMsg(
        SP_BBAT_CobReadCom(m_hDUT, &DataRecv),
        "Phone: COB Read Com", LEVEL_ITEM));

    _UiSendMsg("Phone: COB Read Com", LEVEL_ITEM,
        m_nDownSpecX, DataRecv.nValueX, m_nUpSpecX,
        nullptr, -1, nullptr,
        "X = %d", DataRecv.nValueX);
    _UiSendMsg("Phone: COB Read Com", LEVEL_ITEM,
        m_nDownSpecY, DataRecv.nValueY, m_nUpSpecY,
        nullptr, -1, nullptr,
        "Y = %d", DataRecv.nValueY);
    if ((!IN_RANGE(m_nDownSpecX, DataRecv.nValueX, m_nUpSpecX)) ||
        (!IN_RANGE(m_nDownSpecY, DataRecv.nValueY, m_nUpSpecY)))
    {
        return SP_E_BBAT_COB_VALUE_FAIL;
    }
    return SP_OK;
}

SPRESULT CCob::AFCB_PowerOn()
{
    ThreadRun5Vdelay();
    return SP_OK;
}

SPRESULT CCob::AFCB_PowerOff()
{
    RETURNSPRESULT(m_pObjFunBox->FB_EXT2_Power_Ctrl(false), "G3: EXT2 Power Off");
    return SP_OK;
}

void CCob::ThreadRun5Vdelay()
{
    DWORD dwSuperXXXID = 0;
    m_hTThreadRun5Vdelay = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadRun5VdelayProc,
        this, NULL, &dwSuperXXXID);
}

DWORD CCob::ThreadRun5VdelayProc(LPVOID lpParam)
{
    CCob* pThis = (CCob*)lpParam;
    return pThis->ThreadRun5VdelayFunc();
}

DWORD CCob::ThreadRun5VdelayFunc()
{
    Sleep(m_nSleep);//为了等手机先发送命令

    RETURNSPRESULT(m_pObjFunBox->FB_EXT2_Power_Ctrl(true),"G3: EXT2 Power On");

    Sleep(m_nTouchPen_Delay);//触控笔点击时间
    RETURNSPRESULT(m_pObjFunBox->FB_EXT2_Power_Ctrl(false),"G3: EXT2 Power Off");   //控制5V下电

    return 1;
}
