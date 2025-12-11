#pragma once
#include "ImpBase.h"
#include "WriteBackupNvV2.h"

class CWriteBackupNvV3 :public CWriteBackupNvV2
{
    DECLARE_RUNTIME_CLASS(CWriteBackupNvV3)
public:
    CWriteBackupNvV3(void);
    virtual ~CWriteBackupNvV3(void);
    SPRESULT WriteCustNvFileV3(bool bWrite, uint8* pBuff, uint32 u32BytesToWrite);
    SPRESULT WriteBigNvItem(uint16 u16NvID, uint8* pBuff, uint32 u32ActLen, uint32 u32offset);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL     LoadXMLConfig(void);

};

