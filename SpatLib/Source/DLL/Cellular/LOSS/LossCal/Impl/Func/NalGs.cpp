#include "StdAfx.h"
#include "NalGs.h"
#include "LteUtility.h"
#include "math.h"

CNalGs::CNalGs( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
	m_pNvHelper = NULL;
	m_pFileConfig = NULL;
	m_ppNvBandInfo = NULL;
	m_plossVal = NULL;
}

CNalGs::~CNalGs(void)
{

}

SPRESULT CNalGs::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pNvHelper = (CNvHelper*)GetFunc(FUNC_INVHelper);
    m_pFileConfig = (CFileConfig*)GetFunc(FUNC_FILE_CONFIG);

	if (dynamic_cast<CImpBase*>(m_pSpatBase) == nullptr)
	{
		return SP_E_POINTER;
	}

    m_plossVal = &dynamic_cast<CImpBase*>(m_pSpatBase)->m_lossVal;

    return SP_OK;
}

SPRESULT CNalGs::GetConfig()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_ppNvBandInfo = &m_pNvHelper->m_arrpDownload_HelperInfo[0];

    InitBandInfo();

    m_arrFreq.clear();

    GetFreqTable(LTE_ANT_MAIN);
    GetFreqTable(LTE_ANT_DIV);

    GetConfig(LTE_ANT_MAIN);
    GetConfig(LTE_ANT_DIV);

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs::GetFreqTable( LTE_ANT_E Ant )
{
    LogFmtStrA(SPLOGLV_INFO, "%s ANT = %s ", __FUNCTION__, g_lpAntNameA[Ant]);

    double dLow;
    double dUp;

    //根据Band获得TX频率arrFreq
    for (int i = 0; i < m_pNvHelper->m_nBandCount; i++)
    {
        if (!m_arrBandInfo[i].Enable[Ant] || !m_arrBandInfo[i].Tx[Ant])
        {
            continue;
        }

		CHKRESULT( GetFreqLowUp(m_arrBandInfo[i].Band, m_pNvHelper->m_bV2?0:m_pNvHelper->m_pNvV3->m_arrNvPos[i], m_pNvHelper->m_bV2, TX, dLow, dUp) );
		if (CLteUtility::m_BandInfo[m_arrBandInfo[i].Band].UlChannel.dFreqOffset == 0)
		{
			continue;
		}
		FreqInfo freqInfoTemp;
		freqInfoTemp.Band = m_arrBandInfo[i].Band;
		freqInfoTemp.nPath = TX;
		freqInfoTemp.m_arrFreq = dLow;
		m_arrFreq.push_back(freqInfoTemp);

		freqInfoTemp.m_arrFreq = (dLow + dUp) / 2;
		m_arrFreq.push_back(freqInfoTemp);

		freqInfoTemp.m_arrFreq = dUp;
		m_arrFreq.push_back(freqInfoTemp);
    }

    //根据Band或者RX频率arrFreq
    for (int i = 0; i < m_pNvHelper->m_nBandCount; i++)
    {
        if (!m_arrBandInfo[i].Enable[Ant])
        {
            continue;
        }

        if (LTE_ANT_DIV == Ant && m_pNvHelper->m_arrpDownload_HelperInfo[i].DivDisable)
        {
            continue;
        }

		CHKRESULT( GetFreqLowUp(m_arrBandInfo[i].Band,   m_pNvHelper->m_bV2?0:m_pNvHelper->m_pNvV3->m_arrNvPos[i], m_pNvHelper->m_bV2, RX, dLow, dUp) );
		if (CLteUtility::m_BandInfo[m_arrBandInfo[i].Band].DlChannel.dFreqOffset == 0)
		{
			continue;
		}
		FreqInfo freqInfoTemp;
		freqInfoTemp.Band = m_arrBandInfo[i].Band;
		freqInfoTemp.nPath = RX;
		freqInfoTemp.m_arrFreq = dLow;
		m_arrFreq.push_back(freqInfoTemp);
		
		freqInfoTemp.m_arrFreq = (dLow + dUp) / 2;
		m_arrFreq.push_back(freqInfoTemp);
		
		freqInfoTemp.m_arrFreq = dUp;
		m_arrFreq.push_back(freqInfoTemp);
		
    }

    //arrFreq排序
    //std::sort(m_arrFreq.begin(),m_arrFreq.end(), cmp());

    //arrFreq去除重叠的部分
   /* BOOL bOK;
    do 
    {
        bOK = TRUE;

        for (std::vector<FreqInfo> ::iterator i0 = m_arrFreq.begin(); i0 != m_arrFreq.end(); i0++)
        {
            for (std::vector <FreqInfo> ::iterator i1 = m_arrFreq.begin(); i1 != m_arrFreq.end(); i1++)
            {
                if (i1 == i0)
                {
                    continue;
                }
                if ( i0->m_arrFreq == i1->m_arrFreq ) 
                {
					if( !m_pNvHelper->m_bV2 )
					{
						if( ( m_pNvHelper->m_pNvV3->m_arrDownload_AntMap[i0->Band].Tx[Ant] == m_pNvHelper->m_pNvV3->m_arrDownload_AntMap[i1->Band].Tx[Ant]) 
							&& ( m_pNvHelper->m_pNvV3->m_arrDownload_AntMap[i0->Band].Rx[Ant] == m_pNvHelper->m_pNvV3->m_arrDownload_AntMap[i1->Band].Rx[Ant]) )
						{
							m_arrFreq.erase(i1);
							bOK = FALSE;
							goto Loop;
						}
					}
					else
					{
						m_arrFreq.erase(i1);
						bOK = FALSE;
						goto Loop;
					}
                }
            }
        }

Loop:
        Sleep(0);
    } while (!bOK);*/

    return SP_OK;
}


SPRESULT CNalGs::GetConfig( LTE_ANT_E Ant )
{
	m_arrConfig[Ant].resize(m_arrFreq.size());

	for (uint32 i = 0; i < m_arrFreq.size(); i++)
	{
		double dFreq = m_arrFreq[i].m_arrFreq;
		m_arrConfig[Ant][i].byPath = 0;

		m_arrConfig[Ant][i].dFreq = dFreq;

		for (int k = 0; k < m_pNvHelper->m_nBandCount; k++)
		{
			if (!m_arrBandInfo[k].Enable[Ant] || !m_arrBandInfo[k].Tx[Ant])
			{
				continue;
			}

			LTE_BAND_E Band = m_pNvHelper->m_arrBandInfo[k].nBand;
			if (m_pNvHelper->m_bV2)
			{
				double dTxUpperFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usEnd) + 0.1;
				double dTxLowerFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin);
				if (IN_RANGE(dTxLowerFreq, dFreq, dTxUpperFreq)
					&& (Band == m_arrFreq[i].Band) && (TX == m_arrFreq[i].nPath))
				{
					m_arrConfig[Ant][i].byPath = TX;
					m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].UlChannel.dFreqOffset) * 10);
					m_arrConfig[Ant][i].usChannel = max(CLteUtility::m_BandInfo[Band].UlChannel.usBegin + 25, m_arrConfig[Ant][i].usChannel);
					m_arrConfig[Ant][i].usChannel = min(CLteUtility::m_BandInfo[Band].UlChannel.usEnd + 1 - 25, m_arrConfig[Ant][i].usChannel);
					m_arrConfig[Ant][i].byBand = (uint8)Band;
					break;
				}
			}
			else
			{
				int nNvPos = m_pNvHelper->m_pNvV3->m_arrNvPos[k];
				double dFreqOffset = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin)
					- CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin);
				double dStartChannel = CLteUtility::m_BandInfo[Band].UlChannel.usBegin;
				double dEndChannel = CLteUtility::m_BandInfo[Band].UlChannel.usEnd;
				if ((0 != m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq)
					&& (0 != m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq))
				{
					double dTxLowerFreq = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq / 10.0 - dFreqOffset;
					double dTxUpperFreq = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq / 10.0 - dFreqOffset;

					dStartChannel += 10 * (m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq / 10.0
						- CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin));
					dEndChannel -= 10 * (CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1
						- m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq / 10.0);

					if (IN_RANGE(dTxLowerFreq, dFreq, dTxUpperFreq)
						&& (Band == m_arrFreq[i].Band) && (TX == m_arrFreq[i].nPath))
					{
						m_arrConfig[Ant][i].byPath = TX;
						m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].UlChannel.dFreqOffset) * 10);
						m_arrConfig[Ant][i].usChannel = (uint32)(max(dStartChannel + 25, m_arrConfig[Ant][i].usChannel));
						m_arrConfig[Ant][i].usChannel = (uint32)(min(dEndChannel + 1 - 25, m_arrConfig[Ant][i].usChannel));
						m_arrConfig[Ant][i].byBand = (uint8)Band;
						break;
					}
				}
				else
				{
					double dTxUpperFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usEnd) + 0.1;
					double dTxLowerFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin);
					if (IN_RANGE(dTxLowerFreq, dFreq, dTxUpperFreq)
						&& (Band == m_arrFreq[i].Band) && (TX == m_arrFreq[i].nPath))
					{
						m_arrConfig[Ant][i].byPath = TX;
						m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].UlChannel.dFreqOffset) * 10);
						m_arrConfig[Ant][i].usChannel = (uint32)(max(dStartChannel + 25, m_arrConfig[Ant][i].usChannel));
						m_arrConfig[Ant][i].usChannel = (uint32)(min(dEndChannel + 1 - 25, m_arrConfig[Ant][i].usChannel));
						m_arrConfig[Ant][i].byBand = (uint8)Band;
						break;
					}
				}
			}
		}

		if (m_arrConfig[Ant][i].byPath == 0)
		{
			for (int j = 0; j < m_pNvHelper->m_nBandCount; j++)
			{
				if (!m_arrBandInfo[j].Enable[Ant])
				{
					continue;
				}

				if ((Ant == LTE_ANT_DIV) && (m_pNvHelper->m_arrpDownload_HelperInfo[j].DivDisable))
				{
					continue;
				}

				LTE_BAND_E Band = m_pNvHelper->m_arrBandInfo[j].nBand;
				if (m_pNvHelper->m_bV2)
				{
					double dRxUpperFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1;
					double dRxLowerFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
					if (IN_RANGE(dRxLowerFreq, dFreq, dRxUpperFreq)
						&& (Band == m_arrFreq[i].Band) && (RX == m_arrFreq[i].nPath))
					{
						m_arrConfig[Ant][i].byPath = RX;
						m_arrConfig[Ant][i].byBand = (uint8)Band;
						m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].DlChannel.dFreqOffset) * 10);
						m_arrConfig[Ant][i].usChannel = (uint32)(max(CLteUtility::m_BandInfo[Band].DlChannel.usBegin + 25, m_arrConfig[Ant][i].usChannel));
						m_arrConfig[Ant][i].usChannel = (uint32)(min(CLteUtility::m_BandInfo[Band].DlChannel.usEnd + 1 - 25, m_arrConfig[Ant][i].usChannel));
						m_arrConfig[Ant][i].usTriggerArfcn = 0;
						
						break;
					}
				}
				else
				{
					int nNvPos = m_pNvHelper->m_pNvV3->m_arrNvPos[j];
					double dStartChannel = CLteUtility::m_BandInfo[Band].DlChannel.usBegin;
					double dEndChannel = CLteUtility::m_BandInfo[Band].DlChannel.usEnd;
					if ((0 != m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq)
						&& (0 != m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq))
					{
						double dRxLowerFreq = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq / 10.0;
						double dRxUpperFreq = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq / 10.0;

						dStartChannel += 10 * (m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq / 10.0
							- CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin));
						dEndChannel -= 10 * (CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1
							- m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq / 10.0);

						if (IN_RANGE(dRxLowerFreq, dFreq, dRxUpperFreq)
							&& (Band == m_arrFreq[i].Band) && (RX == m_arrFreq[i].nPath))
						{
							m_arrConfig[Ant][i].byPath = RX;
							m_arrConfig[Ant][i].byBand = (uint8)Band;
							m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].DlChannel.dFreqOffset) * 10);
							m_arrConfig[Ant][i].usChannel = (uint32)(max(dStartChannel + 25, m_arrConfig[Ant][i].usChannel));
							m_arrConfig[Ant][i].usChannel = (uint32)(min(dEndChannel + 1 - 25, m_arrConfig[Ant][i].usChannel));
							if (CLteUtility::m_BandInfo[m_arrFreq[i].Band].UlChannel.dFreqOffset == 0)
							{
								m_arrConfig[Ant][i].usTriggerArfcn = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].ulTriggerArfcn;
							}
							else
							{
								m_arrConfig[Ant][i].usTriggerArfcn = 0;
							}
							break;
						}
					}
					else
					{
						double dRxUpperFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1;
						double dRxLowerFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
						if (IN_RANGE(dRxLowerFreq, dFreq, dRxUpperFreq)
							&& (Band == m_arrFreq[i].Band) && (RX == m_arrFreq[i].nPath))
						{
							m_arrConfig[Ant][i].byPath = RX;
							m_arrConfig[Ant][i].byBand = (uint8)Band;
							m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].DlChannel.dFreqOffset) * 10);
							m_arrConfig[Ant][i].usChannel = (uint32)(max(dStartChannel + 25, m_arrConfig[Ant][i].usChannel));
							m_arrConfig[Ant][i].usChannel = (uint32)(min(dEndChannel + 1 - 25, m_arrConfig[Ant][i].usChannel));
							if (CLteUtility::m_BandInfo[m_arrFreq[i].Band].UlChannel.dFreqOffset == 0)
							{
								m_arrConfig[Ant][i].usTriggerArfcn = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].ulTriggerArfcn;
							}
							else
							{
								m_arrConfig[Ant][i].usTriggerArfcn = 0;
							}
							break;
						}
					}

				}
			}
		}
	}

	LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

	return SP_OK;
}

SPRESULT CNalGs::InitResult()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    InitResult(LTE_ANT_MAIN);
    InitResult(LTE_ANT_DIV );

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs::InitResult( LTE_ANT_E Ant )
{
    m_arrResult[Ant].resize(m_arrConfig[Ant].size());

    return SP_OK;
}

SPRESULT CNalGs::ClearResult()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    ClearResult(LTE_ANT_MAIN);
    ClearResult(LTE_ANT_DIV );

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs::ClearResult( LTE_ANT_E Ant )
{
    memset(&m_arrResult[Ant][0], 0, sizeof(Result) * m_arrResult[Ant].size());

    return SP_OK;
}

void CNalGs::InitBandInfo()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_arrBandInfo.resize(m_pNvHelper->m_nBandCount);

    for (int i = 0; i < m_pNvHelper->m_nBandCount; i++)
    {
        m_arrBandInfo[i].Band = m_pNvHelper->m_arrBandInfo[i].nBand;

        if (m_pFileConfig->m_MainAnt)
        {
            m_arrBandInfo[i].Enable[LTE_ANT_MAIN] = TRUE;
        }
        else
        {
            m_arrBandInfo[i].Enable[LTE_ANT_MAIN] = FALSE;
        }

        if (m_pFileConfig->m_DivAnt && !m_ppNvBandInfo[i].DivDisable)
        {
            m_arrBandInfo[i].Enable[LTE_ANT_DIV] = TRUE;
        }
        else
        {
            m_arrBandInfo[i].Enable[LTE_ANT_DIV] = FALSE;
        }

        m_arrBandInfo[i].Tx[LTE_ANT_MAIN] = TRUE;
        m_arrBandInfo[i].TxCa[LTE_ANT_MAIN] = LTE_CA_PCC;

        if (m_pNvHelper->m_nVersion == LTE_NV_VERSION_V5 
            || m_pNvHelper->m_nVersion == LTE_NV_VERSION_V7)
        {
            m_arrBandInfo[i].Tx[LTE_ANT_DIV] = FALSE;
        }
        else
        {
            if (m_ppNvBandInfo[i].TxCa == 2)
            {
                m_arrBandInfo[i].Tx[LTE_ANT_DIV] = TRUE;
                m_arrBandInfo[i].TxCa[LTE_ANT_DIV] = LTE_CA_SCC;
            }
            else
            {
                m_arrBandInfo[i].Tx[LTE_ANT_DIV] = FALSE;
            }
        }
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
}

void CNalGs::Serialization( std::vector<uint8>* parrData )
{
    parrData->clear();
    parrData->push_back(0);
    parrData->push_back(0);
    parrData->push_back(0);
    parrData->push_back(0);

    int nLength;
    nLength = Serialization(parrData, LTE_ANT_MAIN);
    (*parrData)[0] = (uint8)(nLength     );
    (*parrData)[1] = (uint8)(nLength >> 8);

    nLength = Serialization(parrData, LTE_ANT_DIV);
    (*parrData)[2] = (uint8)(nLength     );
    (*parrData)[3] = (uint8)(nLength >> 8);
}

int CNalGs::Serialization( std::vector<uint8>* parrData, LTE_ANT_E Ant )
{
    int nLength = 0;

    for (uint32 i = 0; i < m_arrConfig[Ant].size(); i++)
    {
        if (m_arrConfig[Ant][i].byPath != 0)
        {        
            uint16 usFreq = (uint16)(m_arrConfig[Ant][i].dFreq * 10);
            parrData->push_back((uint8)(usFreq     ));
            parrData->push_back((uint8)(usFreq >> 8));
            parrData->push_back((uint8)(m_arrConfig[Ant][i].usChannel     ));
            parrData->push_back((uint8)(m_arrConfig[Ant][i].usChannel >> 8));
            parrData->push_back((uint8)(m_arrConfig[Ant][i].usChannel >> 16));
            parrData->push_back((uint8)(m_arrConfig[Ant][i].usChannel >> 24));
            parrData->push_back(m_arrConfig[Ant][i].byBand);
            parrData->push_back(m_arrConfig[Ant][i].byPath);
            nLength += 8;

            if (m_arrConfig[Ant][i].byPath == TX)
            {
                parrData->push_back((uint8)(m_arrResult[Ant][i].TxResult.usWord     ));
                parrData->push_back((uint8)(m_arrResult[Ant][i].TxResult.usWord >> 8));
                uint16 usPower = (uint16)(m_arrResult[Ant][i].TxResult.dPower * 100);
                parrData->push_back((uint8)(usPower     ));
                parrData->push_back((uint8)(usPower >> 8));
                nLength += 4;
            }
            else//RX
            {
                uint16 usRssi = (uint16)(m_arrResult[Ant][i].RxResult.dRssi * 100);
                parrData->push_back((uint8)(usRssi     ));
                parrData->push_back((uint8)(usRssi >> 8));
                uint16 usPower = (uint16)(m_arrResult[Ant][i].RxResult.dCellPower * 100);
                parrData->push_back((uint8)(usPower     ));
                parrData->push_back((uint8)(usPower >> 8));
                parrData->push_back((uint8)(m_arrResult[Ant][i].RxResult.byIndex));

                nLength += 5;
            }
        }
    }

    return nLength;
}

SPRESULT CNalGs::GetFreqLowUp(LTE_BAND_E Band, int nNvPos, BOOL bV2, int nTRX, double &dLow, double &dUp)
{
	IsValidIndex(Band);
	if(bV2)
	{
		if(nTRX == TX)
		{
			dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin);
			dUp = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usEnd) + 0.1;
		}
		else
		{
			dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
			dUp = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1;
		}
	}
	else
	{
		if(( 0 != m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq ) 
			&& ( 0 != m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq ))
		{
			double dMinDLFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
			double dMaxDLFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1;

			double dNvMinDLFreq = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq/10.0;
			double dNvMaxDLFreq = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq/10.0;

			if(( dNvMinDLFreq < (dMinDLFreq - 0.000001) ) || ( dNvMaxDLFreq > (dMaxDLFreq + 0.000001) ))
			{
				LogFmtStrA( SPLOGLV_ERROR, "%s ResetCaliChannel fail! Wrong Freq Nv of Band%d MinDLFreq:%d  MaxDLFreq:%d", __FUNCTION__, nNvPos, dNvMinDLFreq,  dNvMaxDLFreq );
				return SP_E_LTE_CALI_UNEXPECTED;
			}
			else if(( IS_EQUAL( dNvMinDLFreq, dMinDLFreq ) ) && ( IS_EQUAL( dNvMaxDLFreq, dMaxDLFreq) ))
			{
				if(nTRX == TX)
				{
					dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin);
					dUp = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usEnd) + 0.1;
				}
				else
				{
					dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
					dUp = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1;
				}
			}
			else
			{
				if(nTRX == TX)
				{
					double dOffsetLow = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq/10.0 
						-  CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
					double dOffsetUp = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq/10.0 
						-  CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) - 0.1;

					dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin) + dOffsetLow;
					dUp  = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usEnd) + 0.1 + dOffsetUp;
				}
				else
				{
					dLow = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_start_freq/10.0;
					dUp  = m_pNvHelper->m_pNvV3->m_BandSupportInfo[nNvPos].band_end_freq/10.0;
				}
			}
		}
		else
		{
			if(nTRX == TX)
			{
				dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin);
				dUp = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usEnd) + 0.1;
			}
			else
			{
				dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
				dUp = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1;
			}
		}
	}

	return SP_OK;
}

SPRESULT CNalGs::ReadNv(PC_MODEM_RF_V3_DATA_PARAM_T* pNvRsp, int eNvType, int nNvPos, int nRfChain, int nBwId, int nAnt, int nDataOffset, int nDataSize)
{
	LogFmtStrA(SPLOGLV_INFO, "%s: Read NV", __FUNCTION__);
	if (NULL == pNvRsp)
	{
		return SP_E_ALLOC_MEMORY;
	}

	LogFmtStrA(SPLOGLV_INFO, "%s: Read NV %s NV TYPE = %d, Band = %d, RfChain = %d",
		__FUNCTION__,
		CLteUtility::m_mapNvmLte.find(eNvType)->second,
		eNvType,
		nNvPos,
		nRfChain);

	m_stNvmParam.NVM_Uninitialize();

	m_stNvmParam.stReqNv.eNvType = (uint16)eNvType;
	m_stNvmParam.stReqNv.BandId = (uint8)nNvPos;
	m_stNvmParam.stReqNv.HwChanID = (uint8)nRfChain;
	m_stNvmParam.stReqNv.BwId = (uint8)nBwId;
	m_stNvmParam.stReqNv.AntId = (uint8)nAnt;
	m_stNvmParam.stReqNv.DataOffset = (uint16)nDataOffset;
	m_stNvmParam.stReqNv.DataSize = (uint16)nDataSize;

	CHKRESULT(SP_ModemV3_Nv_Read(m_pSpatBase->m_hDUT, &m_stNvmParam.stReqNv, &m_stNvmParam.stRspNv));
	//pNvRsp = &(m_stNvmParam.stRspNv);
	CopyMemory(pNvRsp, &m_stNvmParam.stRspNv, sizeof(PC_MODEM_RF_V3_DATA_PARAM_T));

	LogFmtStrA(SPLOGLV_INFO, "%s: NvDataSize = %d", __FUNCTION__, pNvRsp->DataSize);

	return SP_OK;
}

