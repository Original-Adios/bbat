#include "StdAfx.h"
#include "WlanApiAT.h"
#include "WlanEnterMode.h"

CWlanEnterMode::CWlanEnterMode( CImpBase* pImpBase )
    : m_pWlanApi( NULL )
{
    m_pWlanApi = new CWlanApiAT( pImpBase->m_hDUT );
    ZeroMemory( ( void* )m_szRecvBuf, sizeof( m_szRecvBuf ) );
    ZeroMemory( ( void* )m_szCmd, sizeof( m_szCmd ) );
}

CWlanEnterMode::~CWlanEnterMode( void )
{
    if ( NULL != m_pWlanApi )
    {
        delete m_pWlanApi;
        m_pWlanApi = NULL;
    }
}

SPRESULT CWlanEnterMode::Wlan_SetBand(WLAN_BAND_ENUM eBand)
{
	return m_pWlanApi->DUT_SetBand(eBand);
}

SPRESULT CWlanEnterMode::Wlan_ModeEnable( BOOL bEnterOrLeave )
{
    return m_pWlanApi->DUT_EnterEUTMode( bEnterOrLeave ? true : false );
}

SPRESULT CWlanEnterMode::BDWlan_ModeEnable(BOOL bEnterOrLeave)
{
	return m_pWlanApi->BDDUT_EnterMode(bEnterOrLeave ? true : false);
}
