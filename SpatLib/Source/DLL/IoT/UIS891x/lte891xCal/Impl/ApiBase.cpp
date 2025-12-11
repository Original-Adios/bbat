#include "StdAfx.h"
#include "ApiBase.h"

CApiBase::CApiBase( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
	m_hDUT = NULL;
	m_pRFTester = NULL; 
}

CApiBase::~CApiBase(void)
{

}

SPRESULT CApiBase::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_hDUT = m_pSpatBase->m_hDUT;
    m_pRFTester = m_pSpatBase->m_pRFTester;

    return SP_OK;
}
