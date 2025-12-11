#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CConnect : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CConnect)
public:
    CConnect(void);
    virtual ~CConnect(void);

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
