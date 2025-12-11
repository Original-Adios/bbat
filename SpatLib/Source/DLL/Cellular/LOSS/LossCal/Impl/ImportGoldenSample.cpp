#include "StdAfx.h"
#include "ImportGoldenSample.h"
#include "gsmUtility.h"
#include "TDUtility.h"
#include "wcdmaUtility.h"
#include "CdmaUtility.h"
#include "LteUtility.h"
#include "wcnUtility.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

IMPLEMENT_RUNTIME_CLASS(CImportGoldenSample)
    CImportGoldenSample::CImportGoldenSample(void)
{
}

CImportGoldenSample::~CImportGoldenSample(void)
{
}

BOOL CImportGoldenSample::LoadXMLConfig()
{	
    /// Option	
    std::wstring strVal = GetConfigValue(L"Option:FilePath", L"");
    trimW(strVal);
    m_strFilePath = GetAbsoluteFilePathW(strVal.c_str());
    if (0 == m_strFilePath.length())
    {
        return FALSE;
    }
    return TRUE;
}

SPRESULT CImportGoldenSample::__PollAction(void)
{       
    if(!::PathFileExists(m_strFilePath.c_str()))
    {
        NOTIFY("LossFile not exists", LEVEL_ITEM, 1, 0, 1);
        LogFmtStrW(SPLOGLV_ERROR, L"Loss File %s not exists!", m_strFilePath.c_str());
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    WCHAR szTmp[1024] = {0};
    GetPrivateProfileStringW(L"Common", L"SYSTEMTIME", L"", szTmp, _countof(szTmp), m_strFilePath.c_str());
    swscanf_s(szTmp, L"_%d_%d_%d_%d_%d_%d", &m_gs.common.ts.wYear, &m_gs.common.ts.wMonth, &m_gs.common.ts.wDay, &m_gs.common.ts.wHour, &m_gs.common.ts.wMinute, &m_gs.common.ts.wSecond);
    m_gs.common.nMagicNum = GetPrivateProfileIntW(L"Common", L"MagicNum", 0, m_strFilePath.c_str());
    m_gs.common.u32TotalBytes = GetPrivateProfileIntW(L"Common", L"TotalBytes", 0, m_strFilePath.c_str());

    CHKRESULT_WITH_NOTIFY_FUNNAME(ImportGsm(m_strFilePath.c_str()));
    CHKRESULT_WITH_NOTIFY_FUNNAME(ImportTds(m_strFilePath.c_str()));
    CHKRESULT_WITH_NOTIFY_FUNNAME(ImportWcdma(m_strFilePath.c_str()));
    CHKRESULT_WITH_NOTIFY_FUNNAME(ImportC2K(m_strFilePath.c_str()));
    CHKRESULT_WITH_NOTIFY_FUNNAME(ImportLTE(m_strFilePath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ImportNR(m_strFilePath.c_str()));
    CHKRESULT_WITH_NOTIFY_FUNNAME(ImportWlan(m_strFilePath.c_str()));
    CHKRESULT_WITH_NOTIFY_FUNNAME(ImportBT(m_strFilePath.c_str()));
    CHKRESULT_WITH_NOTIFY_FUNNAME(ImportGPS(m_strFilePath.c_str()));

    SPRESULT res = SaveToPhone();
    NOTIFY("SaveToPhone", LEVEL_ITEM, 1,SP_OK==res ? 1:0,1);
    CHKRESULT(res);
    return SP_OK;
}

SPRESULT CImportGoldenSample::ImportGsm( LPCWSTR lpPath )
{
    WCHAR szTmp[1024] = {0};
    WCHAR szkey[64] = {0};
    m_gs.common.nGsmCnt = (uint8)GetPrivateProfileIntW(L"GSM", L"Count", 0, lpPath);

    if(m_gs.common.nGsmCnt == 0)
    {
        return SP_OK;
    }

    if(m_gs.common.nGsmCnt > MAX_GSM_LOSS_NUMBER)
    {
        LogFmtStrA(SPLOGLV_ERROR, "GSM Loss count error %d > %d.", m_gs.common.nGsmCnt, MAX_GSM_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    std::wstring strFmt;
    GetLossFmtStr(SP_GSM, strFmt, FALSE, TRUE);

    for(int i=0; i<m_gs.common.nGsmCnt; i++)
    {
        int nBand = 0;
        int nArfcn = 0;
        double dFreq = 0.0;
        IMPEXP_ITEM_DATA data[DUL_ANT];
        swprintf_s(szkey, L"Data%d", i);
        GetPrivateProfileStringW(L"GSM", szkey, L"", szTmp, _countof(szTmp), lpPath);
        swscanf_s(szTmp, strFmt.c_str(), &nBand, &nArfcn, &dFreq, 
            &data[0].nLoss, &data[0].nIndex, &data[0].dPower, 
            &data[1].nLoss, &data[1].nIndex, &data[1].dPower);
        m_gs.gsm[i].nBand = (uint16)nBand;
        m_gs.gsm[i].nArfcn = (uint16)nArfcn;
        for(int j=0; j<DUL_ANT; j++)
        {
            m_gs.gsm[i].data[j].nLoss = (uint8)data[j].nLoss;
            m_gs.gsm[i].data[j].nPCL = (uint8)data[j].nIndex;
            m_gs.gsm[i].data[j].nExpPwr = (uint16)(data[j].dPower*100);
        }

    }
    NOTIFY("ImportGsm", LEVEL_ITEM, 1,1,1);
    return SP_OK;
}

SPRESULT CImportGoldenSample::ImportTds( LPCWSTR lpPath )
{
    WCHAR szTmp[1024] = {0};
    WCHAR szkey[64] = {0};
    std::wstring strFmt;
    m_gs.common.nTdCnt = (uint8)GetPrivateProfileIntW(L"TD", L"Count", 0, lpPath);
    if(m_gs.common.nTdCnt == 0)
    {
        return SP_OK;
    }

    if(m_gs.common.nTdCnt > MAX_TD_LOSS_NUMBER)
    {
        LogFmtStrA(SPLOGLV_ERROR, "TDSCDMA Loss count error %d > %d.", m_gs.common.nTdCnt, MAX_TD_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    GetLossFmtStr(SP_TDSCDMA, strFmt, FALSE, TRUE);

    for(int i=0; i<m_gs.common.nTdCnt; i++)
    {
        int nBand = 0;
        int nArfcn = 0;
        double dFreq = 0.0;
        IMPEXP_ITEM_DATA data[DUL_ANT];

        swprintf_s(szkey, L"Data%d", i);
        GetPrivateProfileStringW(L"TD", szkey, L"", szTmp, _countof(szTmp), lpPath);
        swscanf_s(szTmp, strFmt.c_str(), &nBand, &nArfcn, &dFreq, 
            &data[0].nLoss, &data[0].nIndex, &data[0].dPower, 
            &data[1].nLoss, &data[1].nIndex, &data[1].dPower);
        m_gs.td[i].nFreq = (uint16)(dFreq*10);
        for(int j=0; j<DUL_ANT; j++)
        {
            m_gs.td[i].data[j].nBand = (uint8)nBand;
            m_gs.td[i].data[j].nLoss = (uint8)data[j].nLoss;
            m_gs.td[i].data[j].nIndex = (uint16)data[j].nIndex;
            m_gs.td[i].data[j].nExpPwr = (uint16)(data[j].dPower*100);
        }
    }
    NOTIFY("ImportTds", LEVEL_ITEM, 1,1,1);
    return SP_OK;
}

SPRESULT CImportGoldenSample::ImportWcdma( LPCWSTR lpPath )
{
    WCHAR szTmp[1024] = {0};
    WCHAR szkey[64] = {0};
    std::wstring strFmt;
    m_gs.common.nWcdmaCnt = (uint8)GetPrivateProfileIntW(L"WCDMA", L"Count", 0, lpPath);
    if(m_gs.common.nWcdmaCnt == 0)
    {
        return SP_OK;
    }

    if(m_gs.common.nWcdmaCnt > MAX_WCDMA_LOSS_NUMBER)
    {
        LogFmtStrA(SPLOGLV_ERROR, "WCDMA Loss count error %d > %d.", m_gs.common.nWcdmaCnt, MAX_WCDMA_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }

    GetLossFmtStr(SP_WCDMA, strFmt, FALSE, TRUE);

    for(int i=0; i<m_gs.common.nWcdmaCnt; i++)
    {
        int nBand = 0;
        int nArfcn = 0;
        double dFreq = 0.0;
        IMPEXP_ITEM_DATA data[DUL_ANT];

        swprintf_s(szkey, L"Data%d", i);
        GetPrivateProfileStringW(L"WCDMA", szkey, L"", szTmp, _countof(szTmp), lpPath);
        swscanf_s(szTmp, strFmt.c_str(), &nBand, &nArfcn, &dFreq, 
            &data[0].nLoss, &data[0].nIndex, &data[0].dPower, 
            &data[1].nLoss, &data[1].nIndex, &data[1].dPower);
        m_gs.wcdma[i].nFreq = (uint16)(dFreq*10);
        for(int j=0; j<DUL_ANT; j++)
        {
            m_gs.wcdma[i].data[j].nBand = (uint8)nBand-1;
            m_gs.wcdma[i].data[j].nLoss = (uint8)data[j].nLoss;
            m_gs.wcdma[i].data[j].nIndex = (uint16)data[j].nIndex;
            m_gs.wcdma[i].data[j].nExpPwr = (uint16)(data[j].dPower*100);
        }
    }
    NOTIFY("ImportWcdma", LEVEL_ITEM, 1,1,1);
    return SP_OK;
}

SPRESULT CImportGoldenSample::ImportC2K( LPCWSTR lpPath )
{
    WCHAR szTmp[1024] = {0};
    WCHAR szkey[64] = {0};
    std::wstring strFmt;
    m_gs.common.nC2KCnt = (uint8)GetPrivateProfileIntW(L"C2K", L"Count", 0, lpPath);
    if(m_gs.common.nC2KCnt == 0)
    {
        return SP_OK;
    }

    if(m_gs.common.nC2KCnt > MAX_C2K_LOSS_NUMBER)
    {
        LogFmtStrA(SPLOGLV_ERROR, "C2K Loss count error %d > %d.", m_gs.common.nC2KCnt, MAX_C2K_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }

    GetLossFmtStr(SP_C2K, strFmt, FALSE, TRUE);

    for(int i=0; i<m_gs.common.nC2KCnt; i++)
    {
        int nBand = 0;
        int nArfcn = 0;
        double dFreq = 0.0;
        IMPEXP_ITEM_DATA data[DUL_ANT];

        swprintf_s(szkey, L"Data%d", i);
        GetPrivateProfileStringW(L"C2K", szkey, L"", szTmp, _countof(szTmp), lpPath);
        swscanf_s(szTmp, strFmt.c_str(), &nBand, &nArfcn, &dFreq, 
            &data[0].nLoss, &data[0].nIndex, &data[0].dPower, 
            &data[1].nLoss, &data[1].nIndex, &data[1].dPower);
        m_gs.c2k[i].nFreq = (uint16)(nArfcn);
        for(int j=0; j<DUL_ANT; j++)
        {
            m_gs.c2k[i].data[j].nBand = (uint8)nBand;
            m_gs.c2k[i].data[j].nLoss = (uint8)data[j].nLoss;
            m_gs.c2k[i].data[j].nIndex = (uint16)data[j].nIndex;
            m_gs.c2k[i].data[j].nExpPwr = (uint16)(data[j].dPower*100);
        }
    }
    NOTIFY("ImportC2K", LEVEL_ITEM, 1,1,1);
    return SP_OK;
}

SPRESULT CImportGoldenSample::ImportLTE( LPCWSTR lpPath )
{
    WCHAR szTmp[1024] = {0};
    WCHAR szkey[64] = {0};
    std::wstring strFmtTx, strFmtRx;
    int nCount = GetPrivateProfileIntW(L"LTE", L"Count", 0, lpPath);
    if(nCount == 0)
    {
        return SP_OK;
    }
	m_vecLteData.clear();
    GetLossFmtStr(SP_LTE, strFmtTx, FALSE, TRUE);
	GetLossFmtStr(SP_LTE, strFmtRx, FALSE, FALSE);

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
	LTE_Loss_Data stData;
    for(int i=0; i<nCount; i++)
    {
		stData.Init();
    	swprintf_s(szkey, L"Data%d", i);
    	GetPrivateProfileStringW(L"LTE", szkey, L"", szTmp, _countof(szTmp), lpPath);
		if(NULL != wcsstr(szTmp, L"TxPower"))
		{
			swscanf_s(szTmp, strFmtTx.c_str(), &nBand, &nBandIdent, &nArfcn, &dFreq, &nPath, &nAnt, &dTxPower, &nTxFac);
			stData.ANT = (LTE_ANT_E)(nAnt-1);
			stData.nPath = 1;
			stData.TxData.Head.byBand = (uint8)nBand;
			stData.TxData.Head.byPath = (uint8)nPath;
			stData.TxData.Head.uArfcn = (uint32)nArfcn;
			stData.TxData.Head.usFreq = (uint16)(dFreq*10.0);
			stData.TxData.usPower = (uint16)(dTxPower*100.0);
			stData.TxData.usWord = (uint16)nTxFac;
		}
		else
		{
			swscanf_s(szTmp, strFmtRx.c_str(), &nBand, &nBandIdent, &nArfcn, &dFreq, &nPath, &nAnt, &dRssi, &dCellPower, &nRxIndex);
			stData.ANT = (LTE_ANT_E)(nAnt-1);
			stData.nPath = 2;
			stData.RxData.Head.byBand = (uint8)nBand;
			stData.RxData.Head.byPath = (uint8)nPath;
			stData.RxData.Head.uArfcn = (uint32)nArfcn;
			stData.RxData.Head.usFreq = (uint16)(dFreq*10.0);
			stData.RxData.usPower = (uint16)(dCellPower*100.0);
			stData.RxData.usRssi = (uint16)(dRssi*100.0);
			stData.RxData.byIndex = (uint8)nRxIndex;
		}
		m_vecLteData.push_back(stData);
	}
	if(SP_OK != LteSerialization(m_arrLteInfo))
	{
		 NOTIFY("LteSerialization", LEVEL_ITEM, 1,0,1);
		 return SP_E_FAIL;
	}
    NOTIFY("ImportLTE", LEVEL_ITEM, 1,1,1);
    return SP_OK;
}

SPRESULT CImportGoldenSample::ImportNR( LPCWSTR lpPath )
{
	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmtTx, strFmtRx;
	int nCount = GetPrivateProfileIntW(L"NR", L"Count", 0, lpPath);
	if(nCount == 0)
	{
		return SP_OK;
	}
	m_vecNrData.clear();
	GetLossFmtStr(SP_NR, strFmtTx, FALSE, TRUE);
	GetLossFmtStr(SP_NR, strFmtRx, FALSE, FALSE);

	int nBand = 0;
	int nBandIdent = 0;
	int nArfcn = 0;
	double dFreq = 0.0;
	int nPath = 0;
	int RfAnt = 0;
	int nAnt = 0;
	int nParam = 0;
	double dPower = 0.0;
	double dResult = 0.0;
    GsData stData;
	for(int i=0; i<nCount; i++)
	{
		swprintf_s(szkey, L"Data%d", i);
		GetPrivateProfileStringW(L"NR", szkey, L"", szTmp, _countof(szTmp), lpPath);
		swscanf_s(szTmp, strFmtTx.c_str(), &nBand, &nBandIdent, &nArfcn, &dFreq, &nPath, &RfAnt, &nAnt, &nParam, &dPower, &dResult);
		stData.nBand = nBand;
		stData.uArfcn = nArfcn;
		stData.dFreq = dFreq;
		stData.bTx = nPath;
		stData.RfAnt = (RF_ANT_E)RfAnt;
		stData.nAnt = nAnt;
		stData.nParam = nParam;
		stData.dPower = dPower;
		stData.dResult = dResult;

		m_vecNrData.push_back(stData);
	}
	if(SP_OK != NrSerialization(&m_arrNrInfo))
	{
		NOTIFY("NrSerialization", LEVEL_ITEM, 1,0,1);
		return SP_E_FAIL;
	}
	NOTIFY("ImportNR", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CImportGoldenSample::ImportWlan( LPCWSTR lpPath )
{
    WCHAR szTmp[1024] = {0};
    WCHAR szkey[64] = {0};
    std::wstring strFmt;
    m_gs.common.nWlanCnt = (uint8)GetPrivateProfileIntW(L"WLAN", L"Count", 0, lpPath);
    if(m_gs.common.nWlanCnt == 0)
    {
        return SP_OK;
    }

    if(m_gs.common.nWlanCnt > MAX_WLAN_LOSS_NUMBER)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Wlan Loss count error %d > %d.", m_gs.common.nWlanCnt, MAX_WLAN_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    GetLossFmtStr(SP_WIFI, strFmt, FALSE, TRUE);

    for(int i=0; i<m_gs.common.nWlanCnt; i++)
    {
        int nChan = 0;
        int nProtocol = 0;
        IMPEXP_ITEM_DATA data[MUL_ANT];
        swprintf_s(szkey, L"Data%d", i);
        GetPrivateProfileStringW(L"WLAN", szkey, L"", szTmp, _countof(szTmp), lpPath);

        swscanf_s(szTmp, strFmt.c_str(), &nChan, &nProtocol,
            &data[0].nPath, &data[0].dPower,  
            &data[1].nPath, &data[1].dPower,  
            &data[2].nPath, &data[2].dPower,  
            &data[3].nPath, &data[3].dPower);
        m_gs.wlan[i].nCh = (int16)nChan;
        m_gs.wlan[i].data.nProtocalType = (int8)nProtocol;
        for(int j=0; j<MUL_ANT; j++)
        {
            m_gs.wlan[i].data.nPower[j] = int16(data[j].dPower*100.0);
            m_gs.wlan[i].data.nPath[j] = (int8)data[j].nPath;
        }
    }
    NOTIFY("ImportWlan", LEVEL_ITEM, 1,1,1);
    return SP_OK;
}

SPRESULT CImportGoldenSample::ImportBT( LPCWSTR lpPath )
{
    WCHAR szTmp[1024] = {0};
    WCHAR szkey[64] = {0};
    std::wstring strFmt;
    m_gs.common.nBtCnt = (uint8)GetPrivateProfileIntW(L"BT", L"Count", 0, lpPath);
    if(m_gs.common.nBtCnt == 0)
    {
        return SP_OK;
    }

    if(m_gs.common.nBtCnt > MAX_BT_LOSS_NUMBER)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Bluetooth Loss count error %d > %d.", m_gs.common.nBtCnt, MAX_BT_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    GetLossFmtStr(SP_BT, strFmt, FALSE, TRUE);
    for(int i=0; i<m_gs.common.nBtCnt; i++)
    {
        int nChan = 0;
        int nProtocol = 0;
        IMPEXP_ITEM_DATA data[MUL_ANT];
        swprintf_s(szkey, L"Data%d", i);
        GetPrivateProfileStringW(L"BT", szkey, L"", szTmp, _countof(szTmp), lpPath);

        swscanf_s(szTmp, strFmt.c_str(), &nChan, &nProtocol,
            &data[0].nPath, &data[0].dPower,  
            &data[1].nPath, &data[1].dPower,  
            &data[2].nPath, &data[2].dPower,  
            &data[3].nPath, &data[3].dPower);
        m_gs.bt[i].nCh = (int16)nChan;
        m_gs.bt[i].data.nProtocalType = (int8)nProtocol;
        for(int j=0; j<MUL_ANT; j++)
        {
            m_gs.bt[i].data.nPower[j] = int16(data[j].dPower*100.0);
            m_gs.bt[i].data.nPath[j] = (int8)data[j].nPath;
        }
    }
    NOTIFY("ImportBT", LEVEL_ITEM, 1,1,1);
    return SP_OK;
}

SPRESULT CImportGoldenSample::ImportGPS( LPCWSTR lpPath )
{
    WCHAR szTmp[1024] = {0};
    WCHAR szkey[64] = {0};
    std::wstring strFmt;
    m_gs.common.nGpsCnt = (uint8)GetPrivateProfileIntW(L"GPS", L"Count", 0, lpPath);
    if(m_gs.common.nGpsCnt == 0)
    {
        return SP_OK;
    }

    if(m_gs.common.nGpsCnt > MAX_GPS_LOSS_NUMBER)
    {
        LogFmtStrA(SPLOGLV_ERROR, "GPS Loss count error %d > %d.", m_gs.common.nGpsCnt, MAX_GPS_LOSS_NUMBER);
        return SP_E_SPAT_INVALID_PARAMETER;
    }

    GetLossFmtStr(SP_GPS, strFmt, FALSE, TRUE);

    for(int i=0; i<m_gs.common.nGpsCnt; i++)
    {
        int nChan = 0;
        int nProtocol = 0;
        IMPEXP_ITEM_DATA data[MUL_ANT];
        swprintf_s(szkey, L"Data%d", i);
        GetPrivateProfileStringW(L"GPS", szkey, L"", szTmp, _countof(szTmp), lpPath);

        swscanf_s(szTmp, strFmt.c_str(), &nChan, &nProtocol,
            &data[0].nPath, &data[0].dPower,  
            &data[1].nPath, &data[1].dPower,  
            &data[2].nPath, &data[2].dPower,  
            &data[3].nPath, &data[3].dPower);
        m_gs.gps[i].nCh = (int16)nChan;
        m_gs.gps[i].data.nProtocalType = (int8)nProtocol;
        for(int j=0; j<MUL_ANT; j++)
        {
            m_gs.gps[i].data.nPower[j] = int16(data[j].dPower*100.0);
            m_gs.gps[i].data.nPath[j] = (int8)data[j].nPath;
        }
    }
    NOTIFY("ImportGPS", LEVEL_ITEM, 1,1,1);
    return SP_OK;
}

SPRESULT CImportGoldenSample::LteSerialization( std::vector<uint8> &arrInfo )
{
    //m_gs.lte to arrInfo
    arrInfo.resize(MAX_GOLDEN_SAMPLE_SIZE);

    uint16 usMainCount = 0;
    uint16 usDivCount = 0;

    uint8 *pData = &arrInfo[4];

    for (int i = 0; i < (int)m_vecLteData.size(); i++)   //m_vecLteData从文件里读出的数据
    {
		int nTempLen = 0;
        switch(m_vecLteData[i].nPath)
        {
        case 1://TX
			nTempLen = sizeof(LTE_Loss_Data_TX);
            CopyMemory(pData, &m_vecLteData[i].TxData, nTempLen);
            pData += nTempLen;
            break;
        case 2://RX
			nTempLen = sizeof(LTE_Loss_Data_RX);
            CopyMemory(pData, &m_vecLteData[i].RxData, sizeof LTE_Loss_Data_RX);
            pData += nTempLen;
            break;
        }

        switch(m_vecLteData[i].ANT)
        {
        case LTE_ANT_MAIN:
            usMainCount += (uint16)nTempLen;
            break;
        case LTE_ANT_DIV:
            usDivCount += (uint16)nTempLen;
            break;
        }
    }

    *(uint16*)(&arrInfo[0]) = usMainCount;
    *(uint16*)(&arrInfo[2]) = usDivCount;

    arrInfo.resize(usMainCount+usDivCount+4);

    return SP_OK;
}

SPRESULT CImportGoldenSample::NrSerialization( std::vector<uint8> *parrData )
{
	uint32 uSize = m_vecNrData.size() * sizeof(GsData);
	parrData->resize(uSize);
	memcpy(&(*parrData)[0], &m_vecNrData[0], uSize);

	return SP_OK;
}