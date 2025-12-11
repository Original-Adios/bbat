#pragma once
#include "SpatLib.h"
#include "Tr.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
class CSpatLibManager : public ISpatLibManager
                      , public CTr
{
public:
    CSpatLibManager(void);
    virtual ~CSpatLibManager(void);
    virtual void Release(void);
    virtual void InitLog(LPVOID pLogUtil);
    
    virtual BOOL Load(LPCWSTR lpszLibDirPath);
    virtual void Free(void);

    virtual UINT32          GetLibCount(void)const;
    virtual LPCSPATLIB_INFO GetLibInfo(LPCWSTR lpszLibName)const;
    virtual LPCSPATLIB_INFO GetLibInfo(UINT32 u32Index)const;

private:
    BOOL SearchLoad(LPCWSTR lpszDirPath);
    CSpatLib* Validate(LPCWSTR lpszLibPath, LPCWSTR lpszLibName);

private:
    std::vector<CSpatLib* > m_vecLibs;
    ISpLog* m_pLogUtil;
};

//////////////////////////////////////////////////////////////////////////
inline UINT32 CSpatLibManager::GetLibCount(void)const 
{
    return m_vecLibs.size();
}