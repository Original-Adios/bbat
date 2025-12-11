#include "StdAfx.h"
#include "ExportGoldenSample_V5.h"
#include "gsmUtility.h"
#include "TDUtility.h"
#include "wcdmaUtility.h"
#include "CdmaUtility.h"
#include "LteUtility.h"
#include "NrUtility.h"
#include "wcnUtility.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")



IMPLEMENT_RUNTIME_CLASS(CExportGoldenSample_V5)
CExportGoldenSample_V5::CExportGoldenSample_V5(void)
{
}

CExportGoldenSample_V5::~CExportGoldenSample_V5(void)
{
}

SPRESULT CExportGoldenSample_V5::ExportLTE(LPCWSTR lpPath)
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
    if (m_gs.common.usLteCnt == 0)
    {
        LogFmtStrA(SPLOGLV_INFO, "%s Len=0", __FUNCTION__);
        return SP_OK;
    }

    WCHAR szTmp[1024] = { 0 };
    WCHAR szkey[128] = { 0 };
    std::wstring strFmtTx, strFmtRx;
    if (SP_OK != LteDeserialization(m_arrLteInfo))
    {
        NOTIFY("LteDeserialization", LEVEL_ITEM, 1, 0, 1);
        return SP_E_FAIL;
    }
    if (m_vecLteData_V5.size() == 0)
    {
        return SP_OK;
    }
    swprintf_s(szTmp, L"%d", m_vecLteData_V5.size());
    WritePrivateProfileStringW(L"LTE", L"Count", szTmp, lpPath);
    GetLossFmtStrLTE(strFmtTx, TRUE, TRUE);
    GetLossFmtStrLTE(strFmtRx, TRUE, FALSE);
    for (int i = 0; i < (int)m_vecLteData_V5.size(); i++)
    {
        swprintf_s(szkey, L"Data%d", i);
        if (1 == m_vecLteData_V5[i].nPath)
        {
            swprintf_s(szTmp, strFmtTx.c_str(),
                m_vecLteData_V5[i].TxData.Head.byBand, CLteUtility::m_BandInfo[m_vecLteData_V5[i].TxData.Head.byBand].BandIdent, m_vecLteData_V5[i].TxData.Head.uChannel,
                (double)m_vecLteData_V5[i].TxData.Head.usFreq / 10.0 + 0.001, m_vecLteData_V5[i].TxData.Head.PathId, m_vecLteData_V5[i].nPath, m_vecLteData_V5[i].ANT + 1,
                (double)((int16)m_vecLteData_V5[i].TxData.usPower) / 100.0 + 0.001, m_vecLteData_V5[i].TxData.usWord, m_vecLteData_V5[i].TxData.usVoltage);
        }
        else
        {
        //Index:% d, Band : % d, Channel : % d, Freq : % .2lf, ChannelId : % d, Path = % d, Data : Ant % d{ RxRssi: % .2lf,RxCellPower : % .2lf,RxIndex : % d
            swprintf_s(szTmp, strFmtRx.c_str(),
                m_vecLteData_V5[i].RxData.Head.byBand, CLteUtility::m_BandInfo[m_vecLteData_V5[i].RxData.Head.byBand].BandIdent, m_vecLteData_V5[i].RxData.Head.uChannel,
                (double)m_vecLteData_V5[i].RxData.Head.usFreq / 10.0 + 0.001, m_vecLteData_V5[i].RxData.Head.PathId, m_vecLteData_V5[i].nPath, m_vecLteData_V5[i].ANT + 1,
                (double)((int16)m_vecLteData_V5[i].RxData.usRssi) / 100.0 + 0.001, (double)((int16)m_vecLteData_V5[i].RxData.usPower) / 100.0 + 0.001, m_vecLteData_V5[i].nPath, m_vecLteData_V5[i].RxData.byIndex);
        }
        WritePrivateProfileStringW(L"LTE", szkey, szTmp, lpPath);
    }

    NOTIFY("ExportLTE", LEVEL_ITEM, 1, 1, 1);
    return SP_OK;
}


SPRESULT CExportGoldenSample_V5::LteDeserialization( std::vector<uint8> &arrInfo )
{
    LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
	LTE_Loss_Data_V5 stData;
    m_vecLteData_V5.clear();
	uint16 usMainLength = *(uint16*)(&arrInfo[0]);
	uint16 usDivLength = *(uint16*)(&arrInfo[2]);
	uint16 usthirdLength = *(uint16*)(&arrInfo[4]);
	uint16 usFourthLength = *(uint16*)(&arrInfo[6]);
    LogFmtStrA(SPLOGLV_INFO, "MainLen=%d, DivLen=%d, ThirdLen=%d, Fourth=%d",
        usMainLength, usDivLength, usthirdLength, usFourthLength);
    LogFmtStrA(SPLOGLV_INFO, "Total Size=%d", arrInfo.size());


	if (arrInfo.size() != (uint32)(8 + usMainLength + usDivLength + usthirdLength + usFourthLength))
	{
		LogFmtStrA(SPLOGLV_ERROR, "LTE Loss data length Error0!");
		return SP_E_FAIL;
	}

    uint16 usLength = usMainLength + usDivLength + usthirdLength + usFourthLength;
    uint16 usPosition = 0;

    BOOL bEnd = FALSE;
    while (!bEnd)
    {
        if ((usLength - usPosition) < min(sizeof LTE_Loss_Data_TX_V5, sizeof LTE_Loss_Data_RX_V5))
        {
			LogFmtStrA(SPLOGLV_ERROR, "LTE Loss data length Error1!");
            return SP_E_FAIL;
        }
		stData.Init();
        if (usPosition < usMainLength)
        {
            stData.ANT = LTE_RF_ANT_MAIN;
        }
        else if (usPosition < usMainLength + usDivLength)
        {
            stData.ANT = LTE_RF_ANT_DIVERSITY;
        }
        else if (usPosition < usMainLength + usDivLength + usthirdLength)
        {
            stData.ANT = LTE_RF_ANT_THIRD;
        }
        else
        {
            stData.ANT = LTE_RF_ANT_FOURTH;
        }

        uint8* pData = &arrInfo[8] + usPosition;
        switch(*(pData + 13))
        {
        case 1:
            {
            LogFmtStrA(SPLOGLV_INFO, "tx");
                stData.nPath = 1;
                stData.TxData = *((LTE_Loss_Data_TX_V5*)pData);

                pData += sizeof LTE_Loss_Data_TX_V5;
                usPosition += sizeof LTE_Loss_Data_TX_V5;
            }
            break;
        case 2:
            {
            LogFmtStrA(SPLOGLV_INFO, "rx");
                stData.nPath = 2;
                stData.RxData = *((LTE_Loss_Data_RX_V5*)pData);

                pData += sizeof LTE_Loss_Data_RX_V5;
                usPosition += sizeof LTE_Loss_Data_RX_V5;
            }
            break;
        default:
            LogFmtStrA(SPLOGLV_ERROR, "Loss data Error0!");
            return SP_E_FAIL;
        }
        m_vecLteData_V5.push_back(stData);
        if (usPosition > usLength)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Loss data Error1!");
            return SP_E_FAIL;
        }

        if (usPosition == usLength)
        {
            bEnd = TRUE;
        }
    }
    return SP_OK;
}

void CExportGoldenSample_V5::GetLossFmtStrLTE(std::wstring& strFmt, BOOL bExp, BOOL bTX)
{
    if (bTX)
    {
        if (bExp)
        {
            strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%.2lf,PathId:%d,PathGroup=%d,Data:Ant%d{TxPower:%.2lf,TxFactor:%d,TxVoltage=%d}";
        }
        else
        {
            strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%lf,PathId:%d,PathGroup=%d,Data:Ant%d{TxPower:%lf,TxFactor:%d,TxVoltage=%d}";
        }
    }
    else
    {
        if (bExp)
        {
            strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%.2lf,PathId:%d,PathGroup=%d,Stream=%d,Data:Ant%d{RxRssi:%.2lf,RxCellPower:%.2lf,RxIndex:%d}";
        }
        else
        {
            strFmt = L"Index:%d,Band:%d,Channel:%d,Freq:%lf,PathId:%d,PathGroup=%d,Stream=%d,Data:Ant%d{RxRssi:%lf,RxCellPower:%lf,RxIndex:%d}";
        }
    }
}

