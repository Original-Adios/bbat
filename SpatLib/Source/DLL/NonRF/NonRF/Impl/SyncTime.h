#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CSyncTime : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSyncTime)
public:
    CSyncTime(void);
    virtual ~CSyncTime(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
    int getTimeZoneOffset(void);
    FILETIME systemTimeToFileTime(const SYSTEMTIME& st);
    SYSTEMTIME fileTimeToSystemTime(const FILETIME& ft);
    SYSTEMTIME convertToUTC(const SYSTEMTIME& localTime, int timeZoneOffset);
    std::string formatSystemTime(const SYSTEMTIME& time, int timeZoneOffset);

protected:
    std::string  m_strAT;
    std::string m_strRsp;
    uint32  m_u32TimeOut;
};
