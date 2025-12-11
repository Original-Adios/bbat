#pragma once
#include "ImpBase.h"
#include <string>
#include "WriteBackupNV.h"

//////////////////////////////////////////////////////////////////////////
class CWriteBackupNvV2 : public CWriteBackupNV
{
    DECLARE_RUNTIME_CLASS(CWriteBackupNvV2)
public:
    CWriteBackupNvV2(void);
    virtual ~CWriteBackupNvV2(void);

protected:
    virtual SPRESULT __PollAction (void);
	virtual BOOL     LoadXMLConfig(void);
	SPRESULT WriteCustNvFile(bool bWrite, uint8 *pBuff, uint32 u32BytesToWrite);
protected:
	std::wstring m_strFileName;
	std::wstring m_strFileFolder;
	bool m_bCalinv;
	bool m_bProdnv;
	bool m_bMiscdata;
	bool m_bClearNvVerFlag;
};
