#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CWriteBackupNV : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteBackupNV)
public:
    CWriteBackupNV(void);
    virtual ~CWriteBackupNV(void);

protected:
    virtual SPRESULT __PollAction (void);
	virtual BOOL     LoadXMLConfig(void);
	SPRESULT WriteCustNvFile(bool bWrite, uint8 *pBuff, uint32 u32BytesToWrite);
	SPRESULT WriteCustMiscData(bool bWrite, uint8 *pBuff, uint32 u32BytesToWrite);
	SPRESULT WriteCustProdNv(bool bWrite, uint8 *pBuff, uint32 u32BytesToWrite);
	bool SearchFilesByWildChar(LPCWSTR lpcPath, LPWSTR lpNewpath, uint32 u32Newpathsize);
private:
	std::wstring m_strFileName;
	std::wstring m_strFileFolder;
	bool m_bCalinv;
	bool m_bProdnv;
	bool m_bMiscdata;
	bool m_bClearNvVerFlag;
};
