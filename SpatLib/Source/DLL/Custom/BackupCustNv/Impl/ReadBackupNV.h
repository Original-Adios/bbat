#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CReadBackupNV : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CReadBackupNV)
public:
    CReadBackupNV(void);
    virtual ~CReadBackupNV(void);

protected:
    virtual SPRESULT __PollAction (void);
    virtual BOOL     LoadXMLConfig(void);
	SPRESULT ReadCustNvFile(uint8 *u8pNvData, uint32 u32Size, uint32 *u32offset);
	SPRESULT ReadCustMiscData(uint8 *u8pNvData, uint32 u32Size, uint32 *u32offset);
	SPRESULT ReadCustProdNv(uint8 *u8pNvData, uint32 u32Size, uint32 *u32offset);
	SPRESULT WriteNvData2File(LPCWSTR lpcwFileName, uint8 *u8pNvData, uint32 u32Filesize);

private:
	std::wstring m_strFileName;
	uint32	m_u32Magic;

	BACKUP_FILE_HEAD m_fileHead;
	std::wstring m_strFileFolder;
};
