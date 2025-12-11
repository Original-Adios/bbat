#include "StdAfx.h"
#include "ImportGoldenSampleV5.h"
#include "gsmUtility.h"
#include "TDUtility.h"
#include "wcdmaUtility.h"
#include "CdmaUtility.h"
#include "LteUtility.h"
#include "wcnUtility.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

IMPLEMENT_RUNTIME_CLASS(CImportGoldenSampleV5)
CImportGoldenSampleV5::CImportGoldenSampleV5(void)
{
}

CImportGoldenSampleV5::~CImportGoldenSampleV5(void)
{
}

SPRESULT CImportGoldenSampleV5::ImportLTE(LPCWSTR lpPath)
{
    WCHAR szTmp[1024] = { 0 };
    WCHAR szkey[64] = { 0 };
    std::wstring strFmtTx, strFmtRx;
    int nCount = GetPrivateProfileIntW(L"LTE", L"Count", 0, lpPath);
    if (nCount == 0)
    {
        return SP_OK;
    }
    m_vecLteData.clear();
    GetLossFmtStrLTE(strFmtTx, FALSE, TRUE);
    GetLossFmtStrLTE(strFmtRx, FALSE, FALSE);

    int nBand = 0;
    int nBandIdent = 0;
    int nArfcn = 0;
    double dFreq = 0.0;
    int nPath = 0;
    int nAnt = 0;
    double dTxPower = 0.0;
    int nTxFac = 0;
    double dCellPower = 0.0;
    double dRssi = 0.0;
    int nRxIndex = 0;
    uint32 nChannelId = 0;
    LTE_Loss_Data_V5 stData;
    for (int i = 0; i < nCount; i++)
    {
        stData.Init();
        swprintf_s(szkey, L"Data%d", i);
        GetPrivateProfileStringW(L"LTE", szkey, L"", szTmp, _countof(szTmp), lpPath);
        if (NULL != wcsstr(szTmp, L"TxPower"))
        {
            //strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%lf,ChannelId:%d,Path=%d,Data:Ant%d{TxPower:%lf,TxFactor:%d}";
            swscanf_s(szTmp, strFmtTx.c_str(), 
                &nBand, &nBandIdent, &nArfcn, &dFreq, &nChannelId, &nPath, &nAnt, &dTxPower, &nTxFac);
            stData.ANT = (LTE_RF_ANTENNA_E)(nAnt - 1);
            stData.nPath = 1;
            stData.TxData.Head.byBand = (uint8)nBand;
            stData.TxData.Head.byPath = (uint8)nPath;
            stData.TxData.Head.uChannel = (uint32)nArfcn;
            stData.TxData.Head.usFreq = (uint16)(dFreq * 10.0);
            stData.TxData.usPower = (uint16)(dTxPower * 100.0);
            stData.TxData.usWord = (uint16)nTxFac;
            stData.TxData.Head.PathId = nChannelId;
        }
        else
        {
            // strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%.2lf,ChannelId:%d,Path=%d,Data:Ant%d{RxRssi:%.2lf,RxCellPower:%.2lf,RxIndex:%d}";
            swscanf_s(szTmp, strFmtRx.c_str(), &nBand, &nBandIdent, &nArfcn, &dFreq, &nChannelId, &nPath, &nAnt, &dRssi, &dCellPower, &nRxIndex);
            stData.ANT = (LTE_RF_ANTENNA_E)(nAnt - 1);
            stData.nPath = 2;
            stData.RxData.Head.byBand = (uint8)nBand;
            stData.RxData.Head.byPath = (uint8)nPath;
            stData.RxData.Head.uChannel = (uint32)nArfcn;
            stData.RxData.Head.usFreq = (uint16)(dFreq * 10.0);
            stData.RxData.usPower = (uint16)(dCellPower * 100.0);
            stData.RxData.usRssi = (uint16)(dRssi * 100.0);
            stData.RxData.byIndex = (uint8)nRxIndex;
            stData.RxData.Head.PathId = nChannelId;
        }
        m_vecLteData.push_back(stData);
    }
    if (SP_OK != LteSerializationV4(m_arrLteInfo))
    {
        NOTIFY("LteSerialization", LEVEL_ITEM, 1, 0, 1);
        return SP_E_FAIL;
    }
    NOTIFY("ImportLTE", LEVEL_ITEM, 1, 1, 1);
    return SP_OK;
}

SPRESULT CImportGoldenSampleV5::LteSerializationV4(std::vector<uint8>& arrInfo)
{    //m_gs.lte to arrInfo
    arrInfo.resize(MAX_GOLDEN_SAMPLE_SIZE);

    uint16 usMainCount = 0;
    uint16 usDivCount = 0;
    uint16 usThirdCount = 0;
    uint16 usFourthCount = 0;

    uint8* pData = &arrInfo[8];

    for (int i = 0; i < (int)m_vecLteData.size(); i++)   //m_vecLteData从文件里读出的数据
    {
        int nTempLen = 0;
        switch (m_vecLteData[i].nPath)
        {
        case 1://TX
            nTempLen = sizeof(LTE_Loss_Data_TX_V5);
            CopyMemory(pData, &m_vecLteData[i].TxData, nTempLen);
            pData += nTempLen;
            break;
        case 2://RX
            nTempLen = sizeof(LTE_Loss_Data_RX_V5);
            CopyMemory(pData, &m_vecLteData[i].RxData, nTempLen);
            pData += nTempLen;
            break;
        }

        switch (m_vecLteData[i].ANT)
        {
        case LTE_RF_ANT_MAIN:
            usMainCount += (uint16)nTempLen;
            break;
        case LTE_RF_ANT_DIVERSITY:
            usDivCount += (uint16)nTempLen;
            break;
        case LTE_RF_ANT_THIRD:
            usThirdCount += (uint16)nTempLen;
            break;
        case LTE_RF_ANT_FOURTH:
            usFourthCount += (uint16)nTempLen;
            break;
        }
    }

    *(uint16*)(&arrInfo[0]) = usMainCount;
    *(uint16*)(&arrInfo[2]) = usDivCount;
    *(uint16*)(&arrInfo[4]) = usThirdCount;
    *(uint16*)(&arrInfo[6]) = usFourthCount;

    arrInfo.resize(usMainCount + usDivCount + usThirdCount + usFourthCount + 8);

    return SP_OK;
}

void CImportGoldenSampleV5::GetLossFmtStrLTE(std::wstring& strFmt, BOOL bExp, BOOL bTX)
{
    if (bTX)
    {
        if (bExp)
        {
            strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%.2lf,ChannelId:%d,Path=%d,Data:Ant%d{TxPower:%.2lf,TxFactor:%d}";
        }
        else
        {
            strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%lf,ChannelId:%d,Path=%d,Data:Ant%d{TxPower:%lf,TxFactor:%d}";
        }
    }
    else
    {
        if (bExp)
        {
            strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%.2lf,ChannelId:%d,Path=%d,Data:Ant%d{RxRssi:%.2lf,RxCellPower:%.2lf,RxIndex:%d}";
        }
        else
        {
            strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%lf,ChannelId:%d,Path=%d,Data:Ant%d{RxRssi:%lf,RxCellPower:%lf,RxIndex:%d}";
        }
    }
}

