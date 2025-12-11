#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CLoadWidevineDeviceID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadWidevineDeviceID)
public:
    CLoadWidevineDeviceID(void);
    virtual ~CLoadWidevineDeviceID(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    string_t m_strFilePath;
};