#include "StdAfx.h"
#include "AlgoClcRx.h"
#include "LteUtility.h"

CAlgoClcRx::CAlgoClcRx( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CAlgoBase( lpName, pFuncCenter )
    , m_uMaxRetryTime( 30 )
{
	m_pNal = NULL;
	m_pFileConfig = NULL;

	m_Ant = RF_ANT_1st;
	m_Ca = LTE_CA_PCC;

	m_pApi = NULL;
	m_parrConfig = NULL;
	m_parrResult = NULL;
	m_pRFTester = NULL;

	m_uRetryTime = 0;

	ZeroMemory(&m_Loss, sizeof(RF_CABLE_LOSS)); 
}

CAlgoClcRx::~CAlgoClcRx( void )
{


}

void CAlgoClcRx::__SetAntCa()
{
	__super::__SetAntCa();

	m_parrConfig = &m_pNal->m_arrConfig[m_Ant];
	m_parrResult = &m_pNal->m_arrResult[m_Ant];

}

SPRESULT CAlgoClcRx::PreInit()
{
    CHKRESULT( __super::PreInit() );
    
    m_pNal = ( CNalClc* )GetFunc( FUNC_INAL );
    m_pFileConfig = ( CFileConfig* )GetFunc( FUNC_FILE_CONFIG );
    m_pRFTester = m_pSpatBase->m_pRFTester;
    m_pApi = ( IApiRxV3* )GetFunc( API_AGC );
    
    return SP_OK;
}

SPRESULT CAlgoClcRx::Init()
{
	CHKRESULT(__super::Init());
    if ( m_parrConfig->size() == 0 )
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant);
    LogFmtStrA( SPLOGLV_INFO, "%s: ANT = %s; CA = %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca] );
    
    CHKRESULT( InitData() );
    
    LogFmtStrA( SPLOGLV_INFO, "%s End", __FUNCTION__ );
    return SP_OK;
}

SPRESULT CAlgoClcRx::Run()
{
    if ( m_arrarrBandData.size() == 0 )
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant)
    LogFmtStrA( SPLOGLV_INFO, "%s, Ant: %s, Ca: %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca] );
    
    SPRESULT Result;
    
    m_pApi->m_Afc = 0;
    m_pApi->m_Ant = m_Ant;
    m_pApi->m_Ca = m_Ca;
    
    m_pApi->m_pCurrentCaller = NULL;
    m_pApi->DealwithData = NULL;
    
    for ( uint32 i = 0; i < m_arrarrBandData.size(); i++ )
    {
        m_pApi->m_parrBandData = &m_arrarrBandData[i];
        m_pApi->m_pIApi->ClearData();
        
		//////////////////////////////////////////////////////////////////////////
		uint32 nIndex = 0;
		uint32 j = 0;
		for ( ; j < m_parrConfig->size(); j++ )
		{
			if ( ( *m_parrConfig )[j].byPath == RX )
			{
				if ( nIndex == i )
				{
					break;
				}
				nIndex++;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		( *m_parrResult )[j].dLoss = m_pFileConfig->m_dPreLoss;
		SetLoss( ( *m_parrResult )[j].dLoss );

        BOOL ret = TRUE;
        m_uRetryTime = 0;
        
        do
        {
            Result = m_pApi->m_pIApi->Run();
            CHKRESULT( Result );
            CHKRESULT( AdjustLoss( ret, i ) );
            m_uRetryTime++;
        } while ( !ret && m_uRetryTime != m_uMaxRetryTime );
        
        if ( m_uRetryTime == m_uMaxRetryTime )
        {
            return SP_E_SPAT_LOSS_LTE_RX_RETRY_FAIL;
        }
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s End", __FUNCTION__ );
    
    return SP_OK;
}

SPRESULT CAlgoClcRx::InitData()
{
    m_arrarrBandData.resize( 0 );
    
    for ( uint32 i = 0; i < m_parrConfig->size(); i++ )
    {
        Config* pConfig = &( *m_parrConfig )[i];
        
        if ( pConfig->byPath == RX )
        {
            BandData data;
            
            data.arrChannel.resize( 1 );
            
            ChannelData* pChannelData = &data.arrChannel[0];
            pChannelData->nIndicator = pConfig->byIndicator;
            pChannelData->usArfcn = pConfig->usChannel;
            pChannelData->arrPoint.resize( 1 );
            
            PointData* pPointData = &pChannelData->arrPoint[0];
            
            pPointData->usGainIndex = pConfig->Rx.byIndex;
            pPointData->dPower = pConfig->Rx.dCellPower;
            
            vector<BandData> arrBandData;
            arrBandData.push_back( data );
            m_arrarrBandData.push_back( arrBandData );
        }
    }
    
    return SP_OK;
}

SPRESULT CAlgoClcRx::CheckLoss( uint32 uIndex )
{
    LogFmtStrA( SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime );
    
    BandData* pBandData = &m_arrarrBandData[uIndex][0];
    ChannelData* pChannelData = &pBandData->arrChannel[0];
    PointData* pPointData = &pChannelData->arrPoint[0];
    
    uint32 i = 0;
    for ( ; i < m_parrConfig->size(); i++ )
    {
        if ( ( *m_parrConfig )[i].byPath == RX )
        {
            if ( 0 == uIndex )
            {
                break;
            }
            else
            {
                uIndex--;
            }
        }
    }
    
    Config* pConfig = &( *m_parrConfig )[i];
    
    double dTarget = pConfig->Rx.dRssi;
    double dSpec = 0.15;
    double dRssi = pPointData->dRssi;
    
    double dMaxTarget = dTarget + dSpec;
    double dMinTarget = dTarget - dSpec;

    IsValidIndex(m_Ant)
    
    if ( !IN_RANGE( dMinTarget, dRssi, dMaxTarget ) )
    {
        UiSendMsg( "LTE RX Loss Check"
                   , LEVEL_ITEM
                   , dMinTarget
                   , dRssi
                   , dMaxTarget
                   , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                   , pChannelData->usArfcn
                   , "dBm"
                   , "ANT = %s; CA = %s; Freq = %.1lf"
                   , g_lpAntNameA[m_Ant]
                   , g_lpCaNameA[m_Ca]
                   , CLteUtility::GetFreq( pChannelData->usArfcn ) );
                   
        return SP_E_SPAT_LOSS_LTE_CHECK_FAIL;
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s: End", __FUNCTION__ );
    
    return SP_OK;
}

SPRESULT CAlgoClcRx::AdjustLoss( BOOL& ret, uint32 uIndex )
{
    LogFmtStrA( SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime );
    
    BandData* pBandData = &m_arrarrBandData[uIndex][0];
    ChannelData* pChannelData = &pBandData->arrChannel[0];
    PointData* pPointData = &pChannelData->arrPoint[0];
    
    ret = TRUE;
    
    uint32 i = 0;
    for ( ; i < m_parrConfig->size(); i++ )
    {
        if ( ( *m_parrConfig )[i].byPath == RX )
        {
            if ( 0 == uIndex )
            {
                break;
            }
            else
            {
                uIndex--;
            }
        }
    }
    
    Result* pResult = &( *m_parrResult )[i];
    Config* pConfig = &( *m_parrConfig )[i];

	pResult->usAnt = m_Ant;
    
    double dTarget = pConfig->Rx.dRssi;
    double dSpec = 0.15;
    double dRssi = pPointData->dRssi;
    
    double dMaxTarget = dTarget + dSpec;
    double dMinTarget = dTarget - dSpec;
    
    if ( IN_RANGE( dMinTarget, dRssi, dMaxTarget ) )
    {
        pResult->dLoss += ( dTarget - dRssi );
        
        UiSendMsg( "LTE RX Loss"
                   , LEVEL_ITEM
                   , m_pFileConfig->m_dLossValLower
                   , pResult->dLoss
                   , m_pFileConfig->m_dLossValUpper
                   , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                   , pChannelData->usArfcn
                   , "dBm"
                   , "ANT = %s; CA = %s; Freq = %.1lf"
                   , g_lpAntNameA[m_Ant]
                   , g_lpCaNameA[m_Ca]
                   , CLteUtility::GetFreq( pChannelData->usArfcn ) );
                   
		if (pResult->dLoss < 0.0)
		{
			if (NULL != m_pFileConfig->m_pstrNegativeLossDetected)
			{
				if ( string::npos == (*m_pFileConfig->m_pstrNegativeLossDetected).find("LTE"))
				{
					(*m_pFileConfig->m_pstrNegativeLossDetected) += "LTE";
					(*m_pFileConfig->m_pstrNegativeLossDetected) += ",";
				}
			}
		}

        if ( pResult->dLoss <= m_pFileConfig->m_dLossValLower || pResult->dLoss > m_pFileConfig->m_dLossValUpper )
        {
            LogFmtStrA( SPLOGLV_ERROR, "Loss value(%.2f) is out of range(%.2f, %.2f)", pResult->dLoss , m_pFileConfig->m_dLossValLower, m_pFileConfig->m_dLossValUpper );
            return SP_E_SPAT_LOSS_OUT_OF_RANGE;
        }
    }
    else
    {
        if ( m_uRetryTime == m_uMaxRetryTime )
        {
            UiSendMsg( "LTE RX Loss"
                       , LEVEL_ITEM
                       , dMinTarget
                       , dRssi
                       , dMaxTarget
                       , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                       , pChannelData->usArfcn
                       , "dBm"
                       , "ANT = %s; CA = %s; Freq = %.1lf"
                       , g_lpAntNameA[m_Ant]
                       , g_lpCaNameA[m_Ca]
                       , CLteUtility::GetFreq( pChannelData->usArfcn ) );
                       
            return SP_E_SPAT_LOSS_LTE_RX_RETRY_FAIL;
        }
        pResult->dLoss += ( dTarget - dRssi );
        SetLoss( pResult->dLoss );
        ret = FALSE;
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s: End", __FUNCTION__ );
    
    return SP_OK;
}

SPRESULT CAlgoClcRx::SetLoss( double dLoss )
{
    m_Loss.lteLoss.nCount = 1;
    m_Loss.lteLoss.arrPoint[0].dFreq = 1800;
    m_Loss.lteLoss.arrPoint[0].dLoss[0][RF_IO_TX] = dLoss;
    m_Loss.lteLoss.arrPoint[0].dLoss[0][RF_IO_RX] = dLoss;
    m_Loss.lteLoss.arrPoint[0].dLoss[1][RF_IO_TX] = dLoss;
    m_Loss.lteLoss.arrPoint[0].dLoss[1][RF_IO_RX] = dLoss;
    
    U_RF_PARAM param;
    param.pLoss = &m_Loss;
    CHKRESULT(m_pRFTester->SetParameter( PT_CABLE_LOSS, param ));
	return SP_OK;
}

SPRESULT CAlgoClcRx::Check()
{
    if ( m_arrarrBandData.size() == 0 )
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant)
    LogFmtStrA( SPLOGLV_INFO, "%s, Ant: %s, Ca: %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca] );
    
    SPRESULT Result;
    
    m_pApi->m_Afc = 0;
    m_pApi->m_Ant = m_Ant;
    m_pApi->m_Ca = m_Ca;
    
    m_pApi->m_pCurrentCaller = NULL;
    m_pApi->DealwithData = NULL;
    
    for ( uint32 i = 0; i < m_arrarrBandData.size(); i++ )
    {
        m_pApi->m_parrBandData = &m_arrarrBandData[i];

		//////////////////////////////////////////////////////////////////////////
		uint32 nIndex = 0;
		uint32 j = 0;
		for ( ; j < m_parrConfig->size(); j++ )
		{
			if ( ( *m_parrConfig )[j].byPath == RX )
			{
				if ( nIndex == i )
				{
					break;
				}
				nIndex++;
			}
		}
		//////////////////////////////////////////////////////////////////////////
        CHKRESULT(SetLoss( ( *m_parrResult )[j].dLoss ));
        
        Result = m_pApi->m_pIApi->Run();
        CHKRESULT( Result );
        CHKRESULT( CheckLoss( i ) );
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s End", __FUNCTION__ );
    
    return SP_OK;
}

