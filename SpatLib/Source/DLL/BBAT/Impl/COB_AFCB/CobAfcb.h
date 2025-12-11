#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCob : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCob)
public:
    CCob(void);
    virtual ~CCob(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    SPRESULT CobAction();
    SPRESULT Phone_CobInit();
    SPRESULT Phone_CobReadCom();
    SPRESULT AFCB_PowerOn();
    SPRESULT AFCB_PowerOff();

    int m_nUpSpecX = 0;
    int m_nDownSpecX = 0;
    int m_nUpSpecY = 0;
    int m_nDownSpecY = 0;
    int m_nSleep = 0;
    int m_nTouchPen_Delay = 500;
    int m_nDutReadResult_Delay = 200;

    void ThreadRun5Vdelay(); //cob测试power2 5v开启时需要延时开启
    static DWORD ThreadRun5VdelayProc(LPVOID lpParam);
    DWORD ThreadRun5VdelayFunc();
    HANDLE m_hTThreadRun5Vdelay = NULL;
};