#pragma once
#include "ImpBase.h"
#include "../drv/MesData.h"
//////////////////////////////////////////////////////////////////////////
class CMES_Upload : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_Upload)
public:
    CMES_Upload(void);
    virtual ~CMES_Upload(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

	SPRESULT UploadMesLog(char*pszDirMesLog,_pMES_LOG_HEAD p_log_head,char*pszPathMesLog,int iSize);
private:
    std::string m_strProject;

    enum UPLOAD
    {
        NONE  = 0,
        SYNC  = 1,
        ASYNC = 2
    } m_eUpload;
};
