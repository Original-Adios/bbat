#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////


class CAfcbCtrlVol : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAfcbCtrlVol)
public:
    CAfcbCtrlVol(void);
    virtual ~CAfcbCtrlVol(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
    SPRESULT AFCB_Ext1PowerOn(bool bStatus);
    SPRESULT AFCB_Ext2PowerOn(bool bStatus);

private:
    BOOL m_bExtPower1Enable = FALSE;
    BOOL m_bExtPower1Open = FALSE;
    BOOL m_bExtPower2Enable = FALSE;
    BOOL m_bExtPower2Open = FALSE;
};