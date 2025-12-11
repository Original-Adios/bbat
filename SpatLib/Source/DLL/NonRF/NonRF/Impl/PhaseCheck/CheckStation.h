#pragma once
#include "../ImpBase.h"
#include "res_def.h"
#include "PhaseCheckExport.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CCheckStation : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckStation)
public:
    CCheckStation(void);
    virtual ~CCheckStation(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL    LoadXMLConfig(void);

    typedef enum
    {
        CHECK = 0,
        UPDATE,
		RESET
    } CMD;
    SPRESULT ParseResultAndNotify(PHASECHECK_RESULT e, const std::string& strStatioName, CMD eCmd);

protected:
    std::string m_StationName;
};
