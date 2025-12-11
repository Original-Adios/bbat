#include "StdAfx.h"
#include "AlgoClcRxModV4.h"
#include "LteUtility.h"

CAlgoClcRxModV4::CAlgoClcRxModV4( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CAlgoBase( lpName, pFuncCenter )
    , m_uMaxRetryTime( 30 )
{
	m_pNal = NULL;
	m_pFileConfig = NULL;

	m_Ant = RF_ANT_1st;

	m_pApi = NULL;
	m_parrConfig = NULL;
	m_parrResult = NULL;
	m_pRFTester = NULL;

	m_uRetryTime = 0;

	ZeroMemory(&m_Loss, sizeof(RF_CABLE_LOSS)); 

	m_pNv = NULL;
}

CAlgoClcRxModV4::~CAlgoClcRxModV4( void )
{


}

void CAlgoClcRxModV4::__SetAntCa()
{
	__super::__SetAntCa();

	m_parrConfig = &m_pNal->m_arrConfig[m_Ant];
	m_parrResult = &m_pNal->m_arrResult[m_Ant];
}

SPRESULT CAlgoClcRxModV4::PreInit()
{
    CHKRESULT( __super::PreInit() );
    
	m_pNv = (CNvHelperV4*)GetFunc(FUNC_INVHelper);
    m_pNal = ( CNalClcV4* )GetFunc( FUNC_INAL );
    m_pFileConfig = ( CFileConfig* )GetFunc( FUNC_FILE_CONFIG );
    m_pRFTester = m_pSpatBase->m_pRFTester;
    m_pApi = ( IApiRxModV3* )GetFunc( API_AGC );
    
    return SP_OK;
}

SPRESULT CAlgoClcRxModV4::Init()
{
	CHKRESULT(__super::Init());

    m_arrarrBandData.resize(0);

    if ( m_parrConfig->size() == 0 )
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant)
    LogFmtStrA(SPLOGLV_INFO, "%s: ANT = %s", __FUNCTION__, g_lpAntNameAV4[m_Ant]);
    
    CHKRESULT( InitData() );
    
    LogFmtStrA( SPLOGLV_INFO, "%s End", __FUNCTION__ );
    return SP_OK;
}

SPRESULT CAlgoClcRxModV4::Run()
{
    if ( m_arrarrBandData.size() == 0 )
    {
        return SP_OK;
    }

	if( (uint32)m_ePortComp >= m_pNv->m_pNvV4->m_arrGS_PortComp.size())
	{
		LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
		return SP_OK;
	}
    IsValidIndex(m_Ant);
    LogFmtStrA(SPLOGLV_INFO, "%s: ANT = %s", __FUNCTION__, g_lpAntNameAV4[m_Ant]);
    
    SPRESULT Result;
    
	m_pApi->m_Afc = 0;
	m_pApi->m_Ant = m_UiAnt;
	m_pApi->m_Bw = LTE_RF_BW0;
	m_pApi->m_bVsBW_Enable = FALSE;
	m_pApi->m_RfChain = RF_CHAIN_CC0;

	m_pApi->m_TxIns = m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0];
	m_pApi->m_RxIns = m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[m_UiAnt];
	m_pApi->m_bBandAdaptation = m_pNv->m_pNvV4->m_bBandAdaptation;
    
    m_pApi->m_pCurrentCaller = NULL;
    m_pApi->DealwithData = NULL;
    
    for ( uint32 i = 0; i < m_arrarrBandData.size(); i++ )
    { 

        m_pApi->m_parrBandData = &m_arrarrBandData[i];
        if ((m_arrarrBandData[i][0].nChannelId < 16) &&(m_Ant>1))
            m_pApi->m_Ant = (RF_ANT_E)(m_UiAnt % 2);
        
        m_pApi->m_pIApi->ClearData();
		//////////////////////////////////////////////////////////////////////////
		uint32 nIndex = 0;
		uint32 j = 0;
		for ( ; j < m_parrConfig->size(); j++ )
		{
			uint8 nBand = ( *m_parrConfig )[j].byBand; 
			if( (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Tx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0])
				||(m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[0])
				||(m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[1] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[1])
                || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[2] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[2])
                || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[3] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[3]))
			{
				continue;
			}

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
            CHKRESULT(Result);
            CHKRESULT(AdjustLoss(ret, i));
            m_uRetryTime++;
        } while (!ret && m_uRetryTime != m_uMaxRetryTime);

        if (m_uRetryTime == m_uMaxRetryTime)
        {
            return SP_E_SPAT_LOSS_LTE_RX_RETRY_FAIL;
        }
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s End", __FUNCTION__ );
    
    return SP_OK;
}

SPRESULT CAlgoClcRxModV4::InitData()
{
    if (m_ePortComp >= m_pNv->m_pNvV4->m_arrGS_PortComp.size())
    {
      return SP_OK;
    }
    
    for ( uint32 i = 0; i < m_parrConfig->size(); i++ )
    {
        Config* pConfig = &( *m_parrConfig )[i];

		uint8 nBand = pConfig->byBand;
		if ((m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Tx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[1] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[1])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[2] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[3] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[3]))
		{
			continue;
		}
        
        if ( pConfig->byPath == RX )
        {
            BandData data;
            
			data.nBand = pConfig->byBand;
            data.arrChannel.resize( 1 );

            data.nChannelId = pConfig->channelId;
            data.nTriggerArfcn = pConfig->nTriggerArfcn;

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

SPRESULT CAlgoClcRxModV4::CheckLoss( uint32 uIndex )
{
    LogFmtStrA( SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime );
    
    BandData* pBandData = &m_arrarrBandData[uIndex][0];
    ChannelData* pChannelData = &pBandData->arrChannel[0];
    PointData* pPointData = &pChannelData->arrPoint[0];
    
    uint32 i = 0;
    for ( ; i < m_parrConfig->size(); i++ )
    {
		uint8 nBand = ( *m_parrConfig )[i].byBand;
		if ((m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Tx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[1] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[1])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[2] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[3] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[3]))
		{
			continue;
		}

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
    
    IsValidIndex(m_Ant);

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
                   , "ANT = %s;Freq = %.1lf"
                   , g_lpAntNameAV4[m_Ant]
                   , CLteUtility::GetFreq( pChannelData->usArfcn ) );
                   
        return SP_E_SPAT_LOSS_LTE_CHECK_FAIL;
    }
    
    LogFmtStrA( SPLOGLV_INFO, "%s: End", __FUNCTION__ );
    
    return SP_OK;
}

SPRESULT CAlgoClcRxModV4::AdjustLoss( BOOL& ret, uint32 uIndex )
{
    LogFmtStrA( SPLOGLV_INFO, "%s: Retry Time = %d", __FUNCTION__, m_uRetryTime );
    
    BandData* pBandData = &m_arrarrBandData[uIndex][0];
    ChannelData* pChannelData = &pBandData->arrChannel[0];
    PointData* pPointData = &pChannelData->arrPoint[0];
    
    ret = TRUE;
    
    uint32 i = 0;
    for ( ; i < m_parrConfig->size(); i++ )
    {
		uint8 nBand = ( *m_parrConfig )[i].byBand;
		if ((m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Tx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[0])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[1] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[1])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[2] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[2])
            || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[3] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[3]))
		{
			continue;
		}

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

    IsValidIndex(m_UiAnt)
    IsValidIndex(m_Ant)
	pResult->usAnt = m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[m_UiAnt];
    
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
                   , "ANT = %s;Freq = %.1lf"
                   , g_lpAntNameAV4[m_Ant]
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
            UiSendMsg( "LTE RX Rssi"
                       , LEVEL_ITEM
                       , dMinTarget
                       , dRssi
                       , dMaxTarget
                       , CLteUtility::m_BandInfo[pConfig->byBand].NameA
                       , pChannelData->usArfcn
                       , "dBm"
                       , "ANT = %s;Freq = %.1lf"
                       , g_lpAntNameAV4[m_Ant]
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

SPRESULT CAlgoClcRxModV4::SetLoss( double dLoss )
{
    IsValidIndex(m_UiAnt)
	int iRx0 = m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[m_UiAnt];

    m_Loss.lteLoss.nCount = 1;
    m_Loss.lteLoss.arrPoint[0].dFreq = 1800;
    m_Loss.lteLoss.arrPoint[0].dLoss[iRx0][RF_IO_TX] = dLoss;
    m_Loss.lteLoss.arrPoint[0].dLoss[iRx0][RF_IO_RX] = dLoss;
    
    U_RF_PARAM param;
    param.pLoss = &m_Loss;
    CHKRESULT(m_pRFTester->SetParameter( PT_CABLE_LOSS, param ));
	return SP_OK;
}

SPRESULT CAlgoClcRxModV4::Check()
{
    if ( m_arrarrBandData.size() == 0 )
    {
        return SP_OK;
    }
    IsValidIndex(m_Ant);
    LogFmtStrA(SPLOGLV_INFO, "%s: ANT = %s", __FUNCTION__, g_lpAntNameAV4[m_Ant]);
    
    SPRESULT Result;
    
    m_pApi->m_Afc = 0;
    m_pApi->m_Ant = m_Ant;

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
			uint8 nBand = ( *m_parrConfig )[j].byBand;
			if ((m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Tx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Tx[0])
                || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[0] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[0])
                || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[1] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[1])
                || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[2] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[2])
                || (m_pNv->m_pNvV4->m_arrDownload_AntMap[nBand].Rx[3] != m_pNv->m_pNvV4->m_arrGS_PortComp[m_ePortComp].Rx[3]))
			{
				continue;
			}

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

