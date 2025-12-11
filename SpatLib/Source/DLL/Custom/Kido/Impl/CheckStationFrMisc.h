#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CCheckStationFrMisc : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckStationFrMisc)
public:
    CCheckStationFrMisc(void);
    virtual ~CCheckStationFrMisc(void);

protected:
    virtual SPRESULT __PollAction (void);
    virtual BOOL     LoadXMLConfig(void);

private:
	UINT8 m_nStation;
};
