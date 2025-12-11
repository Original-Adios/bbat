#pragma once
#include "CheckStation.h"

//////////////////////////////////////////////////////////////////////////
class CUpdateStation : public CCheckStation
{
    DECLARE_RUNTIME_CLASS(CUpdateStation)
public:
    CUpdateStation(void);
    virtual ~CUpdateStation(void);

protected:
    virtual SPRESULT __PollAction(void);


private:
    SPTEST_RESULT_T m_result;
};
