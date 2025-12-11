#pragma once
#include "ImpBase.h"
#include <string>
#define MAX_MDB_COL_NUM 25
//////////////////////////////////////////////////////////////////////////
class CLoadCodesFromMdb : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadCodesFromMdb)
public:
    CLoadCodesFromMdb(void);
    virtual ~CLoadCodesFromMdb(void);

protected:
	virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction (void);
	std::wstring m_strBatchName;
	std::wstring m_strMdbPath;
};
