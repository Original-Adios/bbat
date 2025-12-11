#include "StdAfx.h"
#include "WCNCLC.h"
#include "ImpBase.h"
#include "wcnUtility.h"


CWCNCLC::CWCNCLC(CImpBase* pSpat)
    :CWCNMeasurePower(pSpat)
{
}


CWCNCLC::~CWCNCLC(void)
{
}

SPRESULT CWCNCLC::Run()
{
    switch (m_eMode)
    {
    case SP_WIFI:
        return RunWlan();
        break;
    case SP_BT:
        return RunBt();
        break;
    case SP_GPS:
        return RunGps();
        break;
    default:
        m_pImp->LogFmtStrA(SPLOGLV_ERROR, "unsupported mode %d", m_eMode);
        return SP_E_SPAT_NOT_SUPPORT;
    }
}

SPRESULT CWCNCLC::Init()
{
    CHKRESULT(__super::Init());
    return SP_OK;
}

SPRESULT CWCNCLC::Release()
{
    CHKRESULT(__super::Release());
    return SP_OK;
}

SPRESULT CWCNCLC::RunWlan()
{
    int nWLanCount = 0;
    if (m_pImp->m_gs.common.nWlanCnt > MAX_WLAN_LOSS_NUMBER)
    {
        m_pImp->LogFmtStrA(SPLOGLV_ERROR, "Wlan的Loss频点过多，最大%d", MAX_WLAN_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    for (int i = 0; i < m_pImp->m_gs.common.nWlanCnt; i++)
    {

        m_WlanPointGroup[nWLanCount].nChannel = (int16)m_pImp->m_gs.wlan[i].nCh;
        m_WlanPointGroup[nWLanCount].nType = (int8)(m_pImp->m_gs.wlan[i].data.nProtocalType);
        for (int j = 0; j < WCN_ANT; j++)
        {
            m_WlanPointGroup[nWLanCount].dOrigPower[j] = m_pImp->m_gs.wlan[i].data.nPower[j] / 100.0;
            m_WlanPointGroup[nWLanCount].dExpPower[j] = m_WlanPointGroup[nWLanCount].dOrigPower[j] + 10.0;
            m_WlanPointGroup[nWLanCount].nPath[j] = (int8)(m_pImp->m_gs.wlan[i].data.nPath[j]);
        }

        nWLanCount++;
    }
    if (nWLanCount > 0)
    {
        int nTotalCount = 0;
        SPRESULT res = WlanMeasurePower(&m_WlanPointGroup[0], nWLanCount);
        if (SP_OK != res)
        {
            m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量Wlan功率失败！ %d", res);
            return res;
        }
        RF_CABLE_LOSS_UNIT_EX* pWlanLoss = &(m_pImp->m_lossVal.wlanLoss);
        ZeroMemory(pWlanLoss, sizeof(*pWlanLoss));
        for (int n = 0; n < nWLanCount; n++)
        {
            //pWlanLoss->arrPoint[nTotalCount].nBand = m_WlanPointGroup[n].nType;
            pWlanLoss->arrPoint[nTotalCount].uArfcn[RF_IO_TX] = m_WlanPointGroup[n].nChannel;
            pWlanLoss->arrPoint[nTotalCount].uArfcn[RF_IO_RX] = pWlanLoss->arrPoint[nTotalCount].uArfcn[RF_IO_TX];
            pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX] = CwcnUtility::WIFI_Ch2MHz(m_WlanPointGroup[n].nChannel);
            pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_RX] = pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX];
            WLAN_BAND_ENUM eBand = WLAN_BAND_24G;//2.4g
            if (pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX] > 5000)
            {
                eBand = WLAN_BAND_50G;		//5g
            }
            pWlanLoss->arrPoint[nTotalCount].nBand = eBand;		//5g

            for (int i = 0; i < WCN_ANT; i++)
            {
                ANTENNA_ENUM ePath = (ANTENNA_ENUM)m_WlanPointGroup[n].nPath[i];
                if (ANT_WLAN_INVALID != ePath)
                {
                    RF_ANT_E eAnt[2] = { RF_ANT_1st, RF_ANT_1st };
                    //CwcnUtility::WlanGetAnt(ePath, eAnt);
                    CwcnUtility::WlanGetAntByBand(ePath, &eAnt[0], eBand);
                    for (int j = 0; j < MAX_RF_IO; j++)
                    {
                        double dLoss = m_WlanPointGroup[n].dOrigPower[i] - m_WlanPointGroup[n].dPower[i] + m_pImp->m_dPreLoss;
                        pWlanLoss->arrPoint[nTotalCount].dLoss[eAnt[0]][j] = std::round(dLoss * 100)/100.00;
                    }
                    m_pImp->LogFmtStrA(SPLOGLV_INFO, "Wlan Loss = %0.2f OrigPower = %0.2f dPower = %0.2f  PreLoss= %0.2f"
                        , pWlanLoss->arrPoint[nTotalCount].dLoss[eAnt[0]][RF_IO_TX]
                        , m_WlanPointGroup[n].dOrigPower[i]
                        , m_WlanPointGroup[n].dPower[i]
                        , m_pImp->m_dPreLoss
                    );
                    m_pImp->NOTIFY("WLAN Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pWlanLoss->arrPoint[nTotalCount].dLoss[eAnt[0]][RF_IO_TX], m_pImp->m_dLossValUpper,
                        "WLAN", m_WlanPointGroup[n].nChannel, "dB", "Freq = %0.2f; %s", pWlanLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX], CwcnUtility::WLAN_ANT_NAME[ePath]);
                    CHKRESULT(m_pImp->CheckLoss(pWlanLoss->arrPoint[nTotalCount].dLoss[eAnt[0]][RF_IO_TX], "WLAN"));
                }
            }
            nTotalCount++;
        }
        pWlanLoss->nCount = nTotalCount;
    }
    return SP_OK;
}

SPRESULT CWCNCLC::RunBt()
{
    int nBtCount = 0;
    if (m_pImp->m_gs.common.nBtCnt > MAX_BT_LOSS_NUMBER)
    {
        m_pImp->LogFmtStrA(SPLOGLV_ERROR, "BT的Loss频点过多，最大%d", MAX_BT_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    for (int i = 0; i < m_pImp->m_gs.common.nBtCnt; i++)
    {
        m_BtPointGroup[nBtCount].nChannel = m_pImp->m_gs.bt[i].nCh;
        m_BtPointGroup[nBtCount].nType = (int8)(m_pImp->m_gs.bt[i].data.nProtocalType);
        for (int j = 0; j < WCN_ANT; j++)
        {
            m_BtPointGroup[nBtCount].nPath[j] = (int8)(m_pImp->m_gs.bt[i].data.nPath[j]);
            m_BtPointGroup[nBtCount].dOrigPower[j] = m_pImp->m_gs.bt[i].data.nPower[j] / 100.0;
            m_BtPointGroup[nBtCount].dExpPower[j] = m_BtPointGroup[nBtCount].dOrigPower[j] + 10.0;
        }
        nBtCount++;
    }
    if (nBtCount > 0)
    {
        int nTotalCount = 0;
        SPRESULT res = BTMeasurePower(&m_BtPointGroup[0], nBtCount);
        if (SP_OK != res)
        {
            m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量BT功率失败！ %d", res);
            return res;
        }
        RF_CABLE_LOSS_UNIT_EX* pBtLoss = &(m_pImp->m_lossVal.btLoss);
        ZeroMemory(pBtLoss, sizeof(*pBtLoss));
        for (int n = 0; n < nBtCount; n++)
        {
            pBtLoss->arrPoint[nTotalCount].nBand = m_BtPointGroup[n].nType;
            pBtLoss->arrPoint[nTotalCount].uArfcn[RF_IO_TX] = m_BtPointGroup[n].nChannel;
            pBtLoss->arrPoint[nTotalCount].uArfcn[RF_IO_RX] = m_BtPointGroup[n].nChannel;
            pBtLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX] = CwcnUtility::BT_Ch2MHz((BT_TYPE)m_BtPointGroup[n].nType, m_BtPointGroup[n].nChannel);
            pBtLoss->arrPoint[nTotalCount].dFreq[RF_IO_RX] = pBtLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX];
            for (int i = 0; i < WCN_ANT; i++)
            {
                BT_RFPATH_ENUM ePath = (BT_RFPATH_ENUM)m_BtPointGroup[n].nPath[i];
                if (ANT_BT_INVALID != ePath)
                {
                    RF_ANT_E eAnt = CwcnUtility::BTGetAnt(ePath);
                    for (int j = 0; j < MAX_RF_IO; j++)
                    {
                        double dLoss = m_BtPointGroup[n].dOrigPower[i] - m_BtPointGroup[n].dPower[i] + m_pImp->m_dPreLoss;
                        pBtLoss->arrPoint[nTotalCount].dLoss[eAnt][j] = std::round(dLoss * 100) / 100.00;
                    }
                    m_pImp->NOTIFY("BT Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pBtLoss->arrPoint[nTotalCount].dLoss[eAnt][RF_IO_TX], m_pImp->m_dLossValUpper,
                        "BT", m_BtPointGroup[n].nChannel, "dB", "Freq = %0.2f; %s", pBtLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX], CwcnUtility::BT_ANT_NAME[ePath]);
                    CHKRESULT(m_pImp->CheckLoss(pBtLoss->arrPoint[nTotalCount].dLoss[eAnt][RF_IO_TX], "BT"));
                }
            }
            nTotalCount++;
        }
        pBtLoss->nCount = nTotalCount;
    }
    return SP_OK;
}

SPRESULT CWCNCLC::RunGps()
{
    if (m_pImp->m_gs.common.nGpsCnt > MAX_GPS_LOSS_NUMBER)
    {
        m_pImp->LogFmtStrA(SPLOGLV_ERROR, "GPS的Loss频点过多，最大%d", MAX_GPS_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    if (m_pImp->m_gs.common.nGpsCnt > 0)
    {
        int nTotalCount = 0;

        RF_CABLE_LOSS_UNIT_EX* pGpsLoss = &(m_pImp->m_lossVal.gpsLoss);
        ZeroMemory(pGpsLoss, sizeof(*pGpsLoss));
        for (int k = 0; k < m_pImp->m_gs.common.nGpsCnt; k++)
        {

            double dExpectCellPower = (double)m_pImp->m_gs.gps[nTotalCount].data.nPower[0] / 10.0;

            double dCellPower = 0;
            GPS_BAND eBand = (GPS_BAND)m_pImp->m_gs.gps[nTotalCount].nCh;
            RF_ANT_E eAnt = (RF_ANT_E)m_pImp->m_gs.gps[nTotalCount].data.nPath[0];
            SPRESULT res = GetGPSCNR(eBand, eAnt, dCellPower, dExpectCellPower);
            if (SP_OK != res)
            {
                m_pImp->LogFmtStrA(SPLOGLV_ERROR, "测量GPS SNR 失败！ %d", res);
                return res;
            }
            pGpsLoss->arrPoint[nTotalCount].nBand = eBand;

            pGpsLoss->arrPoint[nTotalCount].uArfcn[RF_IO_TX] = (uint32)g_GpsFreq[eBand];
            pGpsLoss->arrPoint[nTotalCount].uArfcn[RF_IO_RX] = (uint32)g_GpsFreq[eBand];
            pGpsLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX] = g_GpsFreq[eBand];
            pGpsLoss->arrPoint[nTotalCount].dFreq[RF_IO_RX] = g_GpsFreq[eBand];
            for (int i = 0; i < WCN_ANT; i++)
            {
                if (eAnt != i)
                {
                    continue;
                }
                for (int j = 0; j < MAX_RF_IO; j++)
                {
                    double dLoss = dCellPower - dExpectCellPower + m_pImp->m_dPreLoss;
                    pGpsLoss->arrPoint[nTotalCount].dLoss[i][j] = std::round(dLoss * 100) / 100.00;
                }
            }
            m_pImp->NOTIFY("GPS Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pGpsLoss->arrPoint[nTotalCount].dLoss[eAnt][RF_IO_TX], m_pImp->m_dLossValUpper,
                g_GpsBand[eBand], -1, "dB", "Freq = %0.2f", pGpsLoss->arrPoint[nTotalCount].dFreq[RF_IO_TX]);
            SPRESULT sRet = m_pImp->CheckLoss(pGpsLoss->arrPoint[nTotalCount].dLoss[eAnt][RF_IO_TX], "GPS");
            m_pImp->NOTIFY("CheckLoss", LEVEL_ITEM, 1, SP_OK == sRet ? 1:0, 1, NULL, -1, "-", "%s", g_GpsBand[eBand]);
            CHKRESULT(sRet);

            pGpsLoss->nCount++;
            nTotalCount++;

        }
    }
    return SP_OK;
}
