#pragma once
#include "WriteCodes.h"

//////////////////////////////////////////////////////////////////////////
class CEthernet_WriteCodes : public CWriteCodes
{
    DECLARE_RUNTIME_CLASS(CEthernet_WriteCodes)
public:
    CEthernet_WriteCodes(void);
    virtual ~CEthernet_WriteCodes(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);


protected:
    INPUT_CODES_T m_InputSN[BC_MAX_NUM];
};