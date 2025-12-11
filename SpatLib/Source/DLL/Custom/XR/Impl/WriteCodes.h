#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CWriteCodes : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteCodes)
public:
    CWriteCodes(void);
    virtual ~CWriteCodes(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

    SPRESULT SaveDataIntoLocal(void);
    SPRESULT CheckUniqueIMEI(void);

    SPRESULT UpdateSectionAllocCodes(void);


protected:
    INPUT_CODES_T m_InputSN[BC_MAX_NUM];

    BOOL m_bCheckUniqueIMEI;
    BOOL m_bSaveDataIntoMDB;
    BOOL m_bSaveDataIntoTXT;

    std::wstring m_strMDBpath;
    std::wstring m_strTXTpath;
    std::wstring m_strMDBConn;
};