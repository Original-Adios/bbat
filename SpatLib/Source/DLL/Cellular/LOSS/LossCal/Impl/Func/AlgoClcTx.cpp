#include "StdAfx.h"
#include "AlgoClcTx.h"
#include "LteUtility.h"

CAlgoClcTx::CAlgoClcTx( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CAlgoBase( lpName, pFuncCenter )
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

	m_uIndex = 0;

	ZeroMemory(&m_Loss, sizeof(RF_CABLE_LOSS)); 
}

CAlgoClcTx::~CAlgoClcTx( void )
{

}

void CAlgoClcTx::__SetAntCa()
{
	__super::__SetAntCa();
    
	m_parrConfig = &m_pNal->m_arrConfig[m_Ant];
	m_parrResult = &m_pNal->m_arrResult[m_Ant];

}

SPRESULT CAlgoClcTx::PreInit()
{
    CHKRESULT( __super::PreInit() );
    
    m_pNal = ( CNalClc* )GetFunc( FUNC_INAL );
    m_pFileConfig = ( CFileConfig* )GetFunc( FUNC_FILE_CONFIG );
    m_pRFTester = m_pSpatBase->m_pRFTester;
    m_pApi = ( IApiTxV2* )GetFunc( API_APC );
    
    return SP_OK;
}

SPRESULT CAlgoClcTx::Init()
{
	CHKRESULT(__super::Init());
    if ( m_parrConfig->size() == 0 )
    {
        return SP_OK;
    }
    
    IsValidIndex(m_Ant)
    LogFmtStrA( SPLOGLV_INFO, "%s: ANT = %s; CA = %s", __FUNCTION__, g_lpAntNameA[m_Ant], g_lpCaNameA[m_Ca] );
    
    CHKRESULT( InitData() );
    
    LogFmtStrA( SPLOGLV_INFO, "%s End", __FUNCTION__ );
    return SP_OK;
}

SPRESULT CAlgoClcTx::Run()
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
    
    m_pApi->m_pCurrentCaller = this;
    m_pApi->DealwithData = this->GetResultStatic;
    
    for ( uint32 i = 0; i < m_arrarrBandData.size(); i++ )
    {
		//////////////////////////////////////////////////////////////////////////
		uint32 nIndex = 0;
		uint32 j = 0;
		for ( ; j < m_parrConfig->size(); j++ )
		{
			if ( ( *m_parrConfig )[j].byPath == TX )
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
        
        m_uIndex = i;
        
        m_pApi->m_parrBandData = &m_arrarrBandData[i];
        m_pApi->m_pIApi->ClearData();
        
        Result = m_pApi->m_pIApi->Run();
        CHKRESULT( Result );
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s End", __FUNCTION__ );
    return SP_OK;
}

SPRESULT CAlgoClcTx::InitData()
{
    m_arrarrBandData.resize( 0 );
    
    for ( uint32 i = 0; i < m_parrConfig->size(); i++ )
    {
        Config* pConfig = &( *m_parrConfig )[i];
        
        if ( pConfig->byPath == TX )
        {
            BandData data;
            
            data.arrChannel.resize( 1 );
            data.nIndicator = pConfig->byIndicator;
            
            ChannelData* pChannelData = &data.arrChannel[0];
            pChannelData->usArfcn = pConfig->usChannel;
            pChannelData->arrSection.resize( 1 );
            
            SectionData* pSectionData = &pChannelData->arrSection[0];
            pSectionData->arrPower.resize( 1 );
            pSectionData->dEnp = 0;
            pSectionData->usApt = 24;
            pSectionData->usPaMode = 1;
            pSectionData->usWordStep = 8;
            pSectionData->usBeginWord = pConfig->Tx.usWord;
            
            vector<BandData> arrBandData;
            arrBandData.push_back( data );
            m_arrarrBandData.push_back( arrBandData );
        }
    }
    
    return SP_OK;
}

SPRESULT CAlgoClcTx::GetResultStatic( void* pCaller
                                      , BOOL* ret
                                      , uint32 uStartBand
                                      , uint32 uStartChannel
                                      , uint32 uStartSection
                                      , uint32 uStopBand
                                      , uint32 uStopChannel
                                      , uint32 uStopSection
                                      , uint32 uRetryTime )
{
    CAlgoClcTx* pObject = ( CAlgoClcTx* )pCaller;
    
    return pObject->GetResult( ret
                               , uStartBand
                               , uStartChannel
                               , uStartSection
                               , uStopBand
                               , uStopChannel
                               , uStopSection
                               , uRetryTime );
}

SPRESULT CAlgoClcTx::GetResult( BOOL* ret
                                , uint32 /*uStartBand*/
                                , uint32 /*uStartChannel*/
                                , uint32 /*uStartSection*/
                                , uint32 /*uStopBand*/
                                , uint32 /*uStopChannel*/
                                , uint32 /*uStopSection*/
                                , uint32 uRetryTime )
{
    LogFmtStrA( SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, uRetryTime );
    
    BandData* pBandData = &m_arrarrBandData[m_uIndex][0];
    ChannelData* pChannelData = &pBandData->arrChannel[0];
    SectionData* pSectionData = &pChannelData->arrSection[0];
    
    *ret = TRUE;
    
    uint32 uIndex = m_uIndex;
    uint32 i = 0;
    for ( ; i < m_parrConfig->size(); i++ )
    {
        if ( ( *m_parrConfig )[i].byPath == TX )
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
    
    double dTargetPower = pConfig->Tx.dPower;
    double dPower = pSectionData->arrPower[0];
    
    double dSpec = 0.15;
    
    double dMaxTarget = dTargetPower + dSpec;
    double dMinTarget = dTargetPower - dSpec;
    
    if ( IN_RANGE( dMinTarget, dPower, dMaxTarget ) )
    {
        pResult->dLoss += ( dTargetPower - dPower );
        
        UiSendMsg( "LTE TX Loss"
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
        if ( uRetryTime == m_uMaxRetryTime )
        {
            UiSendMsg( "LTE TX Loss"
                       , LEVEL_ITEM
                       , dMinTarget
                       , dPower
                       , dMaxTarget
                       , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                       , pChannelData->usArfcn
                       , "dBm"
                       , "ANT = %s; CA = %s; Freq = %.1lf"
                       , g_lpAntNameA[m_Ant]
                       , g_lpCaNameA[m_Ca]
                       , CLteUtility::GetFreq( pChannelData->usArfcn ) );
                       
            return SP_E_SPAT_LOSS_LTE_TX_RETRY_FAIL;
        }
        pResult->dLoss += ( dTargetPower - dPower );
        CHKRESULT(SetLoss( pResult->dLoss ));
        *ret = FALSE;
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s: End", __FUNCTION__ );
    
    return SP_OK;
}

SPRESULT CAlgoClcTx::SetLoss( double dLoss )
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

SPRESULT CAlgoClcTx::Check()
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
    
    m_pApi->m_pCurrentCaller = nullptr;
    m_pApi->DealwithData = nullptr;
    
    for ( uint32 i = 0; i < m_arrarrBandData.size(); i++ )
    {
		//////////////////////////////////////////////////////////////////////////
		uint32 nIndex = 0;
		uint32 j = 0;
		for ( ; j < m_parrConfig->size(); j++ )
		{
			if ( ( *m_parrConfig )[j].byPath == TX )
			{
				if ( nIndex == i )
				{
					break;
				}
				nIndex++;
			}
		}
		//////////////////////////////////////////////////////////////////////////
        SetLoss( ( *m_parrResult )[j].dLoss );
        m_arrarrBandData[i][0].arrChannel[0].arrSection[0].usDone = FALSE;
        
        m_pApi->m_parrBandData = &m_arrarrBandData[i];
        
        Result = m_pApi->m_pIApi->Run();
        CHKRESULT( Result );
        
//         uint32 uIndex = i;
//         uint32 j = 0;
//         for ( ; j < m_parrConfig->size(); j++ )
//         {
//             if ( ( *m_parrConfig )[j].byPath == TX )
//             {
//                 if ( 0 == uIndex )
//                 {
//                     break;
//                 }
//                 else
//                 {
//                     uIndex--;
//                 }
//             }
//         }
        
        Config* pConfig = &( *m_parrConfig )[j];
        double dTargetPower = pConfig->Tx.dPower;
        double dSpec = 0.15;
        double dMaxTarget = dTargetPower + dSpec;
        double dMinTarget = dTargetPower - dSpec;
        
        double dPower = m_arrarrBandData[i][0].arrChannel[0].arrSection[0].arrPower[0];
        
        if ( !IN_RANGE( dMinTarget, dPower, dMaxTarget ) )
        {
            UiSendMsg( "LTE TX Loss Check"
                       , LEVEL_ITEM
                       , dMinTarget
                       , dPower
                       , dMaxTarget
                       , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                       , m_arrarrBandData[i][0].arrChannel[0].usArfcn
                       , "dBm"
                       , "ANT = %s; CA = %s; Freq = %.1lf"
                       , g_lpAntNameA[m_Ant]
                       , g_lpCaNameA[m_Ca]
                       , CLteUtility::GetFreq( m_arrarrBandData[i][0].arrChannel[0].usArfcn ) );
                       
            return SP_E_SPAT_LOSS_LTE_CHECK_FAIL;
        }
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s End", __FUNCTION__ );
    return SP_OK;
}
