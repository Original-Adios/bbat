#pragma once
#include "SpatBase.h"
#include "DeviceManagerExport.h"

//////////////////////////////////////////////////////////////////////////
class CDCSFactory sealed
{
public:
    CDCSFactory(CSpatBase* pImp);
   ~CDCSFactory(void);

    void    Release(void) { delete this; };
    IDCS*   CreateDCS(const GPIB_PARAM& io);
    void    DeleteDCS(IDCS*& pDCS);
    
private:
    IDevManager* CreateDevManager(const std::wstring& strDevDir, LPVOID pLogUtil);
    void ReleaseDevManager(IDevManager*& lpDevManager);

private:
    HMODULE   m_hDevManagerLib;
    IDevManager* m_pDevManager;
    CSpatBase*   m_pImp;
};