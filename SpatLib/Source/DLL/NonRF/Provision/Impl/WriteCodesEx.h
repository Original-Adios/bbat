#pragma once
#include "WriteCodes.h"
//////////////////////////////////////////////////////////////////////////

class CWriteCodesEx : public CWriteCodes
{
    DECLARE_RUNTIME_CLASS(CWriteCodesEx)
public:
    CWriteCodesEx(void);
    virtual ~CWriteCodesEx(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

    // Check IMEI & MEID UNIQUE
    SPRESULT CheckUniqueGEID(void);

private:
    BOOL m_bCheckUniqueMEID;
};
