#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////


class CVibratorAfcb : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CVibratorAfcb)
public:
    CVibratorAfcb(void);
    virtual ~CVibratorAfcb(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

public:
    virtual SPRESULT VibAction();
    virtual SPRESULT VibratorOpen();
    virtual SPRESULT VibratorClose();
    virtual SPRESULT GetCurrent();

    BOOL m_bFp = FALSE;
    double m_dUpSpec = 0.0;
    double m_dDownSpec = 0.0;
};