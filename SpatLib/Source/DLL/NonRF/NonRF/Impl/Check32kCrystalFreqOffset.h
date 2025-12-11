#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheck32kCrystalFreqOffset : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheck32kCrystalFreqOffset)
public:
    CCheck32kCrystalFreqOffset(void);
    virtual ~CCheck32kCrystalFreqOffset(void);

protected:
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    uint32 m_uCheckSpec;
};
