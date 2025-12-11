#include "StdAfx.h"
#include "NalGs_UIS8910.h"
#include "LteUtility.h"
#include "math.h"

CNalGs_UIS8910::CNalGs_UIS8910( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
	m_pNv = NULL;
	m_pFileConfig = NULL;
	m_ppNvBandInfo = NULL;
	m_plossVal = NULL;
}

CNalGs_UIS8910::~CNalGs_UIS8910(void)
{

}

SPRESULT CNalGs_UIS8910::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pNv = (CNv_UIS8910*)GetFunc(FUNC_INV);
    m_pFileConfig = (CFileConfig_UIS8910*)GetFunc(FUNC_FILE_CONFIG);

    m_plossVal = &dynamic_cast<CImpBaseUIS8910*>(m_pSpatBase)->m_lossVal;

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::GetConfig()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_ppNvBandInfo = &m_pNv->m_arrpDownload_BandInfo[0];

    InitBandInfo();

    m_arrFreq.clear();

    GetFreqTable(LTE_ANT_MAIN);
    GetFreqTable(LTE_ANT_DIV);

    if ((TRUE == m_pFileConfig->m_OnlyRxFreqMeasLoss) && (TRUE == m_pFileConfig->m_OnlyTxFreqMeasLoss))
    {
        LogFmtStrA(SPLOGLV_ERROR, "OnlyRxFreqMeasLoss = %d,OnlyTxFreqMeasLoss = %d ", m_pFileConfig->m_OnlyRxFreqMeasLoss, m_pFileConfig->m_OnlyTxFreqMeasLoss);
        return SP_E_FAIL;
    }

    if (TRUE == m_pFileConfig->m_PriorTxFreqMeasLoss)
    {
        GetConfigTx(LTE_ANT_MAIN);
        GetConfigTx(LTE_ANT_DIV);
    }
    else
    {
        GetConfigRx(LTE_ANT_MAIN);
        GetConfigRx(LTE_ANT_DIV);
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::GetFreqTable( LTE_ANT_E Ant )
{
    LogFmtStrA(SPLOGLV_INFO, "%s ANT = %s ", __FUNCTION__, g_lpAntNameA[Ant]);

    double dLow;
    double dUp;

    //根据Band获得TX频率arrFreq
    for (int i = 0; i < m_pNv->m_nBandCount; i++)
    {
        if (!m_arrBandInfo[i].Enable[Ant] || !m_arrBandInfo[i].Tx[Ant])
        {
            continue;
        }
        IsValidIndex(m_arrBandInfo[i].Band);
        dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[m_arrBandInfo[i].Band].UlChannel.usBegin);
        dUp = CLteUtility::GetFreq(CLteUtility::m_BandInfo[m_arrBandInfo[i].Band].UlChannel.usEnd) + 0.1;

        m_arrFreq.push_back(dLow);
        m_arrFreq.push_back((dLow + dUp) / 2);
        m_arrFreq.push_back(dUp);
    }

    //根据Band或者RX频率arrFreq
    for (int i = 0; i < m_pNv->m_nBandCount; i++)
    {
        if (!m_arrBandInfo[i].Enable[Ant])
        {
            continue;
        }

        if (LTE_ANT_DIV == Ant && m_pNv->m_arrpDownload_BandInfo[i]->DivDisable)
        {
            continue;
        }
        IsValidIndex(m_arrBandInfo[i].Band);
        dLow = CLteUtility::GetFreq(CLteUtility::m_BandInfo[m_arrBandInfo[i].Band].DlChannel.usBegin);
        dUp = CLteUtility::GetFreq(CLteUtility::m_BandInfo[m_arrBandInfo[i].Band].DlChannel.usEnd) + 0.1;

        m_arrFreq.push_back(dLow);
        m_arrFreq.push_back((dLow + dUp) / 2);
        m_arrFreq.push_back(dUp);    
    }

    //arrFreq排序
    std::sort(m_arrFreq.begin(),m_arrFreq.end(), cmp());

    //arrFreq去除重叠的部分
    BOOL bOK;
    do 
    {
        bOK = TRUE;

        for (std::vector<double> ::iterator i0 = m_arrFreq.begin(); i0 != m_arrFreq.end(); i0++)
        {
            for (std::vector <double> ::iterator i1 = m_arrFreq.begin(); i1 != m_arrFreq.end(); i1++)
            {
                if (i1 == i0)
                {
                    continue;
                }
                if (*i0 == *i1)
                {
                    m_arrFreq.erase(i1);
                    bOK = FALSE;
                    goto Loop;
                }
            }
        }

Loop:
        Sleep(0);
    } while (!bOK);

    return SP_OK;
}


SPRESULT CNalGs_UIS8910::GetConfigRx( LTE_ANT_E Ant )
{
    m_arrConfig[Ant].resize(m_arrFreq.size());
    uint32 iArrFreqSize = m_arrFreq.size();
    for (uint32 i = 0; i < iArrFreqSize; i++)
    {
        double dFreq = m_arrFreq[i];
        m_arrConfig[Ant][i].byPath = 0;

        m_arrConfig[Ant][i].dFreq = dFreq;

        if (FALSE == m_pFileConfig->m_OnlyTxFreqMeasLoss)
        {
            for (int k = 0; k < m_pNv->m_nBandCount; k++)
            {
                if (!m_arrBandInfo[k].Enable[Ant])
                {
                    continue;
                }

                if ((Ant == LTE_ANT_DIV) && (m_pNv->m_arrpDownload_BandInfo[k]->DivDisable))
                {
                    continue;
                }

                LTE_BAND_E Band = m_pNv->m_arrBandInfo[k].nBand;
                IsValidIndex(Band);
                m_arrConfig[Ant][i].byBand = (uint8)Band;
                double dRxUpperFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1;
                double dRxLowerFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
                if (IN_RANGE(dRxLowerFreq, dFreq, dRxUpperFreq))
                {
                    m_arrConfig[Ant][i].byPath = RX;
                    m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].DlChannel.dFreqOffset) * 10);
                    m_arrConfig[Ant][i].usChannel = max(CLteUtility::m_BandInfo[Band].DlChannel.usBegin + 25, m_arrConfig[Ant][i].usChannel);
                    m_arrConfig[Ant][i].usChannel = min(CLteUtility::m_BandInfo[Band].DlChannel.usEnd + 1 - 25, m_arrConfig[Ant][i].usChannel);

                    break;
                }
            }
        }

        if ((m_arrConfig[Ant][i].byPath == 0)&&(FALSE == m_pFileConfig->m_OnlyRxFreqMeasLoss))
        {
            for (int j = 0; j < m_pNv->m_nBandCount; j++)
            {
                if (!m_arrBandInfo[j].Enable[Ant] || !m_arrBandInfo[j].Tx[Ant])
                {
                    continue;
                }

                LTE_BAND_E Band = m_pNv->m_arrBandInfo[j].nBand;
                IsValidIndex(Band);
                m_arrConfig[Ant][i].byBand = (uint8)Band;
                double dTxUpperFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usEnd) + 0.1;
                double dTxLowerFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin);
                if (IN_RANGE(dTxLowerFreq, dFreq, dTxUpperFreq))
                {
                    m_arrConfig[Ant][i].byPath = TX;
                    m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].UlChannel.dFreqOffset) * 10);
                    m_arrConfig[Ant][i].usChannel = max(CLteUtility::m_BandInfo[Band].UlChannel.usBegin + 25, m_arrConfig[Ant][i].usChannel);
                    m_arrConfig[Ant][i].usChannel = min(CLteUtility::m_BandInfo[Band].UlChannel.usEnd + 1 - 25, m_arrConfig[Ant][i].usChannel);
                    break;
                }
            }
        }
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::GetConfigTx(LTE_ANT_E Ant)
{
    m_arrConfig[Ant].resize(m_arrFreq.size());
    uint32 iArrFreqSize = m_arrFreq.size();
    for (uint32 i = 0; i < iArrFreqSize; i++)
    {
        double dFreq = m_arrFreq[i];
        m_arrConfig[Ant][i].byPath = 0;

        m_arrConfig[Ant][i].dFreq = dFreq;

        if (FALSE == m_pFileConfig->m_OnlyRxFreqMeasLoss)
        {
            for (int k = 0; k < m_pNv->m_nBandCount; k++)
            {
                if (!m_arrBandInfo[k].Enable[Ant] || !m_arrBandInfo[k].Tx[Ant])
                {
                    continue;
                }

                LTE_BAND_E Band = m_pNv->m_arrBandInfo[k].nBand;
                IsValidIndex(Band);
                m_arrConfig[Ant][i].byBand = (uint8)Band;
                double dTxUpperFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usEnd) + 0.1;
                double dTxLowerFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].UlChannel.usBegin);
                if (IN_RANGE(dTxLowerFreq, dFreq, dTxUpperFreq))
                {
                    m_arrConfig[Ant][i].byPath = TX;
                    m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].UlChannel.dFreqOffset) * 10);
                    m_arrConfig[Ant][i].usChannel = max(CLteUtility::m_BandInfo[Band].UlChannel.usBegin + 25, m_arrConfig[Ant][i].usChannel);
                    m_arrConfig[Ant][i].usChannel = min(CLteUtility::m_BandInfo[Band].UlChannel.usEnd + 1 - 25, m_arrConfig[Ant][i].usChannel);
                    break;
                }
            }
        }

        if ((m_arrConfig[Ant][i].byPath == 0)&&(FALSE == m_pFileConfig->m_OnlyTxFreqMeasLoss))
        {
            for (int j = 0; j < m_pNv->m_nBandCount; j++)
            {
                if (!m_arrBandInfo[j].Enable[Ant])
                {
                    continue;
                }

                if ((Ant == LTE_ANT_DIV) && (m_pNv->m_arrpDownload_BandInfo[j]->DivDisable))
                {
                    continue;
                }

                LTE_BAND_E Band = m_pNv->m_arrBandInfo[j].nBand;
                IsValidIndex(Band);
                m_arrConfig[Ant][i].byBand = (uint8)Band;
                double dRxUpperFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usEnd) + 0.1;
                double dRxLowerFreq = CLteUtility::GetFreq(CLteUtility::m_BandInfo[Band].DlChannel.usBegin);
                if (IN_RANGE(dRxLowerFreq, dFreq, dRxUpperFreq))
                {
                    m_arrConfig[Ant][i].byPath = RX;
                    m_arrConfig[Ant][i].usChannel = (uint32)((dFreq - CLteUtility::m_BandInfo[Band].DlChannel.dFreqOffset) * 10);
                    m_arrConfig[Ant][i].usChannel = max(CLteUtility::m_BandInfo[Band].DlChannel.usBegin + 25, m_arrConfig[Ant][i].usChannel);
                    m_arrConfig[Ant][i].usChannel = min(CLteUtility::m_BandInfo[Band].DlChannel.usEnd + 1 - 25, m_arrConfig[Ant][i].usChannel);
                    break;
                }
            }
        }
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::InitResult()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    InitResult(LTE_ANT_MAIN);
    InitResult(LTE_ANT_DIV );

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::InitResultAcc()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    InitResultAcc(LTE_ANT_MAIN);
    InitResultAcc(LTE_ANT_DIV );

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::InitResult( LTE_ANT_E Ant )
{
    m_arrResult[Ant].resize(m_arrConfig[Ant].size());

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::InitResultAcc( LTE_ANT_E Ant )
{
    m_arrResultAcc[Ant].resize(m_arrConfig[Ant].size());

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::ClearResult()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    ClearResult(LTE_ANT_MAIN);
    ClearResult(LTE_ANT_DIV );

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::ClearResultAcc()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    ClearResultAcc(LTE_ANT_MAIN);
    ClearResultAcc(LTE_ANT_DIV );

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::ClearResult( LTE_ANT_E Ant )
{
    memset(&m_arrResult[Ant][0], 0, sizeof(Result) * m_arrResult[Ant].size());

    return SP_OK;
}

SPRESULT CNalGs_UIS8910::ClearResultAcc( LTE_ANT_E Ant )
{
    memset(&m_arrResultAcc[Ant][0], 0, sizeof(Result) * m_arrResultAcc[Ant].size());

    return SP_OK;
}

void CNalGs_UIS8910::InitBandInfo()
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);

    m_arrBandInfo.resize(m_pNv->m_nBandCount);

    for (int i = 0; i < m_pNv->m_nBandCount; i++)
    {
        m_arrBandInfo[i].Band = m_pNv->m_arrBandInfo[i].nBand;

        if (m_pFileConfig->m_MainAnt)
        {
            m_arrBandInfo[i].Enable[LTE_ANT_MAIN] = TRUE;
        }
        else
        {
            m_arrBandInfo[i].Enable[LTE_ANT_MAIN] = FALSE;
        }

        if (m_pFileConfig->m_DivAnt && !m_ppNvBandInfo[i]->DivDisable)
        {
            m_arrBandInfo[i].Enable[LTE_ANT_DIV] = TRUE;
        }
        else
        {
            m_arrBandInfo[i].Enable[LTE_ANT_DIV] = FALSE;
        }

        if ((m_pNv->m_arrBandInfo[i].nNumeral == 58)
          &&(m_pNv->m_arrBandInfo[i].nIndicator == 65))//B58 use for wifi,no TX
        {
            m_arrBandInfo[i].Tx[LTE_ANT_MAIN] = FALSE;
        }
        else
        {
            m_arrBandInfo[i].Tx[LTE_ANT_MAIN] = TRUE;
        }

        m_arrBandInfo[i].TxCa[LTE_ANT_MAIN] = LTE_CA_PCC;
		m_arrBandInfo[i].Tx[LTE_ANT_DIV] = FALSE;
#if 0
        if (m_pNv->m_pCalibration_Version->Version == LTE_NV_VERSION_V5 
            || m_pNv->m_pCalibration_Version->Version == LTE_NV_VERSION_V7)
        {
            m_arrBandInfo[i].Tx[LTE_ANT_DIV] = FALSE;
        }
        else
        {
            if (m_ppNvBandInfo[i]->TxCa == 2)
            {
                m_arrBandInfo[i].Tx[LTE_ANT_DIV] = TRUE;
                m_arrBandInfo[i].TxCa[LTE_ANT_DIV] = LTE_CA_SCC;
            }
            else
            {
                m_arrBandInfo[i].Tx[LTE_ANT_DIV] = FALSE;
            }
        }
#endif
    }

    LogFmtStrA(SPLOGLV_INFO, "%s End", __FUNCTION__);
}

void CNalGs_UIS8910::Serialization( std::vector<uint8>* parrData )
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

int CNalGs_UIS8910::Serialization( std::vector<uint8>* parrData, LTE_ANT_E Ant )
{
    int nLength = 0;
    uint32 iArrConfigSize = m_arrConfig[Ant].size();
    for (uint32 i = 0; i < iArrConfigSize; i++)
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

SPRESULT CNalGs_UIS8910::AccResultPro(int ave_calc)
{
    for(uint32 Ant = 0; Ant <MAX_LTE_ANT; Ant++)
    {
        uint32 iArrResultSize = m_arrResult[Ant].size();
        for (uint32 i = 0; i < iArrResultSize; i++)
        {
            m_arrResultAcc[Ant][i].TxResult.usWord += m_arrResult[Ant][i].TxResult.usWord;
            m_arrResultAcc[Ant][i].TxResult.dPower += m_arrResult[Ant][i].TxResult.dPower;
            m_arrResultAcc[Ant][i].TxResult.dTargetPower += m_arrResult[Ant][i].TxResult.dTargetPower;
            m_arrResultAcc[Ant][i].RxResult.byIndex += m_arrResult[Ant][i].RxResult.byIndex;
            m_arrResultAcc[Ant][i].RxResult.dRssi += m_arrResult[Ant][i].RxResult.dRssi;
            m_arrResultAcc[Ant][i].RxResult.dCellPower += m_arrResult[Ant][i].RxResult.dCellPower;
            m_arrResult[Ant][i].bDone = FALSE;
            if(ave_calc)
            {
                m_arrResultAcc[Ant][i].TxResult.usWord /= 2;
                m_arrResultAcc[Ant][i].TxResult.dPower /= 2;
                m_arrResultAcc[Ant][i].TxResult.dTargetPower /= 2;
                m_arrResultAcc[Ant][i].RxResult.byIndex /= 2;
                m_arrResultAcc[Ant][i].RxResult.dRssi /= 2;
                m_arrResultAcc[Ant][i].RxResult.dCellPower /= 2;

                m_arrResult[Ant][i].TxResult.usWord = m_arrResultAcc[Ant][i].TxResult.usWord;
                m_arrResult[Ant][i].TxResult.dPower = m_arrResultAcc[Ant][i].TxResult.dPower;
                m_arrResult[Ant][i].TxResult.dTargetPower = m_arrResultAcc[Ant][i].TxResult.dTargetPower;
                m_arrResult[Ant][i].RxResult.byIndex = m_arrResultAcc[Ant][i].RxResult.byIndex;
                m_arrResult[Ant][i].RxResult.dRssi = m_arrResultAcc[Ant][i].RxResult.dRssi;
                m_arrResult[Ant][i].RxResult.dCellPower = m_arrResultAcc[Ant][i].RxResult.dCellPower;
            }
        }
    }
    
    return SP_OK;
}
