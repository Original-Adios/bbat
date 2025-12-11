#include "StdAfx.h"
#include "ImpBase.h"


//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
    : m_pSLDrv(NULL)
{

}

CImpBase::~CImpBase(void)
{
}

SPRESULT CImpBase::GetSLDriverFromShareMemory(void)
{
    SPRESULT res = GetShareMemory(ShareMemory_SLDriver, (void* )&m_pSLDrv, sizeof(m_pSLDrv));
    if (SP_OK == res)
    {
        if (NULL == m_pSLDrv)
        {
            res = SP_E_POINTER;
        }
    }

    return res;
}

SPRESULT CImpBase::SetSLDriverIntoShareMemory(void)
{
    return SetShareMemory(ShareMemory_SLDriver, (const void* )&m_pSLDrv, sizeof(m_pSLDrv),IContainer::System);
}
