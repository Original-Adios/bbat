#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CUpdateStationToMisc : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUpdateStationToMisc)
public:
    CUpdateStationToMisc(void);
    virtual ~CUpdateStationToMisc(void);

protected:
    virtual SPRESULT __PollAction (void);
	virtual BOOL     LoadXMLConfig(void);

private:
	UINT8 m_nStation;
};
