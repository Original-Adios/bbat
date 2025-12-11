#pragma once
#include <string>
#include "ReadBackupNV.h"

//////////////////////////////////////////////////////////////////////////
class CReadBackupNvV2 : public CReadBackupNV
{
    DECLARE_RUNTIME_CLASS(CReadBackupNvV2)
public:
    CReadBackupNvV2(void);
    virtual ~CReadBackupNvV2(void);

protected:
    virtual SPRESULT __PollAction (void);
    virtual BOOL     LoadXMLConfig(void);
	SPRESULT ReadCustNvFile(uint8 *u8pNvData, uint32 u32Size, uint32 *u32offset);

private:
	std::wstring m_strFileName;
	uint32	m_u32Magic;

	BACKUP_FILE_HEAD m_fileHead;
	std::wstring m_strFileFolder;
};
