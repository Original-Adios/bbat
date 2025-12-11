#include "StdAfx.h"
#include "BTLoadXMLCW.h"
#include <map>

CBTLoadXMLCW::CBTLoadXMLCW(CSpatBase* pSpatBase)
    : m_pSpatBase(pSpatBase)
{
}

CBTLoadXMLCW::~CBTLoadXMLCW(void)
{
    if (NULL != m_pSpatBase)
    {
        m_pSpatBase = NULL;
    }
}

SPRESULT CBTLoadXMLCW::BTLoadXmlFile(vector<BTMeasParamBand>& VecBTParamBandImp)
{
    if (!VecBTParamBandImp.empty())
    {
        VecBTParamBandImp.clear();
    }

    BOOL bEnable = FALSE;
    int nCount = 0;
    double* parrDouble = NULL;
    int* parrInt = NULL;
    SPRESULT eBtConState = SP_OK;

    wstring strMode[MAX_BT_TYPE] = { L"BDR",L"EDR", L"BLE", L"BLE5.0" };
    wstring strAnt[ANT_BT_MAX] = { L"INVALID", L"StandAlone", L"Shared" };

    int nAvgCount = m_pSpatBase->GetConfigValue(L"Param:Common:AvgCount", 3);

    for (int nAnt = ANT_SINGLE; nAnt < ANT_BT_MAX; nAnt++)
    {
        bEnable = (BOOL)m_pSpatBase->GetConfigValue((L"Option:Ant:" + strAnt[nAnt]).c_str(), FALSE);
        if (!bEnable)
        {
            continue;
        }
        int nRfSwitchAnt1st = 0;
        int nRfSwitchAnt2nd = 0;
        if (ANT_SINGLE == nAnt)
        {
            nRfSwitchAnt1st = (BOOL)m_pSpatBase->GetConfigValue(L"Option:Ant:RfSwitchPrimaryAnt", 0);
        }

        if (ANT_SHARED == nAnt)
        {
            nRfSwitchAnt2nd = (BOOL)m_pSpatBase->GetConfigValue(L"Option:Ant:RfSwitchDiversityAnt", 0);
        }

        for (int nMode = BDR; nMode < MAX_BT_TYPE; nMode++)
        {
            BTMeasParamBand stBTParamBand;
            stBTParamBand.eMode = (BT_TYPE)nMode;
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((L"Option:Mode:" + strMode[nMode]).c_str(), FALSE);

            if (!bEnable)
            {
                continue;
            }
            int nGroupCount = m_pSpatBase->GetConfigValue((L"Param:" + strMode[nMode] + L":GroupCount").c_str(), FALSE);
            for (int nGroupIndex = 0; nGroupIndex < nGroupCount; nGroupIndex++)
            {
                vector<int> ChGroup;
                wchar_t    szGroup[20] = { 0 };
                swprintf_s(szGroup, L":Group%d", nGroupIndex + 1);
                wstring strGroup = L"Param:" + strMode[nMode] + szGroup;
                wstring strVal = m_pSpatBase->GetConfigValue((strGroup + L":TCH").c_str(), L"1,39,78");
                parrInt = m_pSpatBase->GetSimpleTokenIntegerW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
                if (nCount < 1)
                {
                    m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid meas chan config!");
                    return SP_E_WCN_INVALID_XML_CONFIG;
                }

                for (int nChTmp = 0; nChTmp < nCount; nChTmp++)
                {
                    RSLT_RANGE_CHECK(parrInt[nChTmp], 0, 78, eBtConState, SP_E_WCN_INVALID_XML_CONFIG);
                    CHKRESULT(eBtConState);
                    ChGroup.push_back(parrInt[nChTmp]);
                }
                LPCSTR lpPacket = m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strGroup + L":PacketType").c_str(), L"DH5"));
                BT_PACKET ePacket = CwcnUtility::BTGetPacket(stBTParamBand.eMode, lpPacket);
                if (INVALID_PACKET == ePacket)
                {
                    m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT]Invalid packet type!");
                    return SP_E_WCN_INVALID_XML_CONFIG;
                }
                double dVsgLvl = m_pSpatBase->GetConfigValue((strGroup + L":RXLVL").c_str(), -65.0);
                double dRefLvl = m_pSpatBase->GetConfigValue((strGroup + L":RefLVL").c_str(), 18);
                int nPacketCount = m_pSpatBase->GetConfigValue((strGroup + L":RxCount").c_str(), 1000);
                wstring strMask = strGroup + L":TestItem:";
                DWORD dwMask = 0;

                bEnable = (BOOL)m_pSpatBase->GetConfigValue((strMask + L"TXP").c_str(), FALSE);
                if (bEnable)
                {
                    dwMask |= BT_CW;
                }

                for (int nChTmp = 0; nChTmp < (int)ChGroup.size(); nChTmp++)
                {
                    BTMeasParamChan stChanParam;
                    stChanParam.dRefLvl = dRefLvl;
                    stChanParam.dVsgLvl = dVsgLvl;
                    stChanParam.dwMask = dwMask;
                    stChanParam.ePacketType = ePacket;
                    stChanParam.ePath = BT_RFPATH_ENUM(nAnt);
                    stChanParam.nRfSwitchAnt1st = nRfSwitchAnt1st;
                    stChanParam.nRfSwitchAnt2nd = nRfSwitchAnt2nd;
                    stChanParam.nAvgCount = nAvgCount;
                    stChanParam.nCh = ChGroup[nChTmp];
                    stChanParam.nTotalPackets = nPacketCount;
                    stChanParam.nPacketLen = CwcnUtility::BT_MAX_PKTLEN[stBTParamBand.eMode][stChanParam.ePacketType];
                    stBTParamBand.vecBTFileParamChan.push_back(stChanParam);
                }
            }

            wstring strSpec = L"Param:" + strMode[nMode] + L":Specification";
            if (nMode == BDR)
            {
                wstring strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXP").c_str(), L"2,20");
                parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
                if (nCount < 2)
                {
                    m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
                    return SP_E_WCN_INVALID_XML_CONFIG;
                }
                stBTParamBand.stSpec.stBdrSpec.dTxp.low = parrDouble[0];
                stBTParamBand.stSpec.stBdrSpec.dTxp.upp = parrDouble[1];
                //TXPShared
                strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXPShared").c_str(), L"");
                if (strVal.empty())
                {
                    stBTParamBand.stSpec.stBdrSpec.dTxpShared.low = parrDouble[0];
                    stBTParamBand.stSpec.stBdrSpec.dTxpShared.upp = parrDouble[1];
                }
                else
                {
                    parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
                    if (nCount < 2)
                    {
                        m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXPShared limit");
                        return SP_E_WCN_INVALID_XML_CONFIG;
                    }
                    stBTParamBand.stSpec.stBdrSpec.dTxpShared.low = parrDouble[0];
                    stBTParamBand.stSpec.stBdrSpec.dTxpShared.upp = parrDouble[1];
                }

                VecBTParamBandImp.push_back(stBTParamBand);
            }
            else if (nMode == EDR)
            {
                //TXP spec
                wstring strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXP").c_str(), L"4,20");
                parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
                if (nCount < 2)
                {
                    m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXP limit");
                    return SP_E_WCN_INVALID_XML_CONFIG;
                }
                stBTParamBand.stSpec.stEdrSpec.dTxp.low = parrDouble[0];
                stBTParamBand.stSpec.stEdrSpec.dTxp.upp = parrDouble[1];

                //TXPShared
                strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXPShared").c_str(), L"");
                if (strVal.empty())
                {
                    stBTParamBand.stSpec.stEdrSpec.dTxpShared.low = parrDouble[0];
                    stBTParamBand.stSpec.stEdrSpec.dTxpShared.upp = parrDouble[1];
                }
                else
                {
                    parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
                    if (nCount < 2)
                    {
                        m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXPShared limit");
                        return SP_E_WCN_INVALID_XML_CONFIG;
                    }
                    stBTParamBand.stSpec.stEdrSpec.dTxpShared.low = parrDouble[0];
                    stBTParamBand.stSpec.stEdrSpec.dTxpShared.upp = parrDouble[1];
                }
                VecBTParamBandImp.push_back(stBTParamBand);
            }
            else
            {
                //TXP AVG spec
                wstring strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXP").c_str(), L"-20,10");
                parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
                if (nCount < 2)
                {
                    m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid BLE avg power limit");
                    return SP_E_WCN_INVALID_XML_CONFIG;
                }
                stBTParamBand.stSpec.stBleExSpec.dTxpAvg.low = parrDouble[0];
                stBTParamBand.stSpec.stBleExSpec.dTxpAvg.upp = parrDouble[1];

                //TXPShared
                strVal = m_pSpatBase->GetConfigValue((strSpec + L":TXPShared").c_str(), L"");
                if (strVal.empty())
                {
                    stBTParamBand.stSpec.stBleExSpec.dTxpAvgShared.low = parrDouble[0];
                    stBTParamBand.stSpec.stBleExSpec.dTxpAvgShared.upp = parrDouble[1];
                }
                else
                {
                    parrDouble = m_pSpatBase->GetTokenDoubleW(strVal.c_str(), DEFAULT_DELIMITER_W, nCount);
                    if (nCount < 2)
                    {
                        m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[BT FT]Invalid TXPShared limit");
                        return SP_E_WCN_INVALID_XML_CONFIG;
                    }
                    stBTParamBand.stSpec.stBleExSpec.dTxpAvgShared.low = parrDouble[0];
                    stBTParamBand.stSpec.stBleExSpec.dTxpAvgShared.upp = parrDouble[1];
                }

                VecBTParamBandImp.push_back(stBTParamBand);
            }
        }
    }
    return SP_OK;
}
