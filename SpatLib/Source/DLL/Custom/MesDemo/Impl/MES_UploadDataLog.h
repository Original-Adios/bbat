#pragma once
#include "ImpBase.h"
#include "../drv/MesData.h"
//////////////////////////////////////////////////////////////////////////
class CMES_UploadDataLog : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_UploadDataLog)
public:
    CMES_UploadDataLog(void);
    virtual ~CMES_UploadDataLog(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

	SPRESULT UploadMesLog(char*pszPath,int iSize);
};
