#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_Connect : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_Connect)
public:
    CMES_Connect(void);
    virtual ~CMES_Connect(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strProject;

    enum UPLOAD
    {
        NONE  = 0,
        SYNC  = 1,
        ASYNC = 2
    } m_eUpload;
};
