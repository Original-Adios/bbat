#pragma once

#include "ISpatLibManager.h"
#include "ISpLogExport.h"

//////////////////////////////////////////////////////////////////////////
class CSpatLib
{
public:
    CSpatLib(LPCWSTR lpFilePath, LPCWSTR lpFileName, ISpLog* pLogUtil);
    virtual ~CSpatLib(void);

    BOOL IsLoaded(void)const;

    BOOL Load(void);
    void Free(void);

    LPCWSTR         GetLibName(void)const;
    LPCSPATLIB_INFO GetLibInfo(void);

private:
    HMODULE      m_hDLL;
    ISpLog*      m_pLogUtil;
    SPATLIB_INFO m_LibInfo;
};

//////////////////////////////////////////////////////////////////////////
inline BOOL CSpatLib::IsLoaded(void)const 
{
    return (NULL != m_hDLL) ? TRUE : FALSE;
}

inline LPCWSTR CSpatLib::GetLibName(void)const 
{
    return m_LibInfo.szLibaryName;
}