#include "StdAfx.h"
#include "ExportGoldenSample.h"
#include "gsmUtility.h"
#include "TDUtility.h"
#include "wcdmaUtility.h"
#include "CdmaUtility.h"
#include "LteUtility.h"
#include "NrUtility.h"
#include "wcnUtility.h"
#include <Shlwapi.h>
#include"CommonDefine.h"
#pragma comment(lib, "shlwapi.lib")



IMPLEMENT_RUNTIME_CLASS(CExportGoldenSample)
CExportGoldenSample::CExportGoldenSample(void)
{
}

CExportGoldenSample::~CExportGoldenSample(void)
{
}

BOOL CExportGoldenSample::LoadXMLConfig()
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

SPRESULT CExportGoldenSample::__PollAction(void)
{  

	SPRESULT res = LoadFromPhone();
	NOTIFY("LoadFromPhone", LEVEL_ITEM, 1,SP_OK==res ? 1:0,1);
	CHKRESULT(res);
	
	// TODO: Get directory, name and extension from path
	std::wstring strPath = L"";
	std::wstring strName = L"";
	std::wstring strExt  = L"";
	if (PathIsDirectoryW(m_strFilePath.c_str()))
	{
		// Path is directory
		strPath = m_strFilePath;
		strName = L"gs";
		strExt  = L".ini";
	}
	else
	{
		// Get directory
		strPath = m_strFilePath;
		std::wstring::size_type nPos = strPath.rfind(L'\\');
		if (nPos != std::wstring::npos)
		{
			strPath = strPath.substr(0, nPos + 1);
		}

		// Get file name
		strName = PathFindFileNameW(m_strFilePath.c_str());
		WCHAR szName[_MAX_PATH] = {0};
		wcsncpy_s(szName, strName.c_str(), strName.length());
		PathRemoveExtensionW(szName);
		strName = szName;

		// Get file extension
		strExt  = PathFindExtensionW(m_strFilePath.c_str());
	}

	if (!CreateMultiDirectoryW(strPath.c_str(), NULL))
	{
		NOTIFY("Create Directory", 1, 0, 1);
		LogFmtStrW(SPLOGLV_ERROR, L"Create Directory %s failed", strPath.c_str());
		return SP_E_FAIL;
	}
	WCHAR szTmp[1024] = {0};
	swprintf_s(szTmp, L"_%02d_%02d_%02d_%02d_%02d_%02d", m_gs.common.ts.wYear, m_gs.common.ts.wMonth, m_gs.common.ts.wDay, m_gs.common.ts.wHour, m_gs.common.ts.wMinute, m_gs.common.ts.wSecond);
	strName += szTmp;
	strPath = strPath + strName + strExt;

	WritePrivateProfileStringW(L"Common", L"SYSTEMTIME", szTmp, strPath.c_str());
	swprintf_s(szTmp, L"%d", m_gs.common.nMagicNum);
	WritePrivateProfileStringW(L"Common", L"MagicNum", szTmp, strPath.c_str());

	swprintf_s(szTmp, L"%d", m_gs.common.u32TotalBytes);
	WritePrivateProfileStringW(L"Common", L"TotalBytes", szTmp, strPath.c_str());

	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportGsm(strPath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportTds(strPath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportWcdma(strPath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportC2K(strPath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportLTE(strPath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportNR(strPath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportWlan(strPath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportBT(strPath.c_str()));
	CHKRESULT_WITH_NOTIFY_FUNNAME(ExportGPS(strPath.c_str()));
    return SP_OK;
}

SPRESULT CExportGoldenSample::ExportGsm(LPCWSTR lpPath)
{
	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmt;
	if(m_gs.common.nGsmCnt == 0)
	{
		return SP_OK;
	}

	if(m_gs.common.nGsmCnt > MAX_GSM_LOSS_NUMBER)
	{
		LogFmtStrA(SPLOGLV_ERROR, "GSM Loss count error %d > %d.", m_gs.common.nGsmCnt, MAX_GSM_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	GetLossFmtStr(SP_GSM, strFmt, TRUE, TRUE);
	swprintf_s(szTmp, L"%d", m_gs.common.nGsmCnt);
	WritePrivateProfileStringW(L"GSM", L"Count", szTmp, lpPath);

	WritePrivateProfileStringW(L"GSM", L"Description", L"Band0:GSM850,Band1:GSM900,Band2:DCS,Band3:PCS", lpPath);
	
	for(int i=0; i<m_gs.common.nGsmCnt; i++)
	{
		int nBand = m_gs.gsm[i].nBand;
		int nArfcn = m_gs.gsm[i].nArfcn;
		double dFreq = CgsmUtility::Arfcn2MHz((SP_BAND_INFO)nBand, TRUE, nArfcn)+0.001;
		IMPEXP_ITEM_DATA data[DUL_ANT];
		for(int j=0; j<DUL_ANT; j++)
		{
			data[j].dPower = (double)((int16)m_gs.gsm[i].data[j].nExpPwr)/100.0+0.001;
			data[j].nLoss = m_gs.gsm[i].data[j].nLoss;
			data[j].nIndex = m_gs.gsm[i].data[j].nPCL;
		}
		swprintf_s(szTmp, strFmt.c_str(), nBand, nArfcn, dFreq
			, data[0].nLoss, data[0].nIndex, data[0].dPower
			, data[1].nLoss, data[1].nIndex, data[1].dPower);

		swprintf_s(szkey, L"Data%d", i);
		WritePrivateProfileStringW(L"GSM", szkey, szTmp, lpPath);
	}
	NOTIFY("ExportGsm", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CExportGoldenSample::ExportTds( LPCWSTR lpPath )
{
	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmt;
	if(m_gs.common.nTdCnt == 0)
	{
		return SP_OK;
	}

	if(m_gs.common.nTdCnt > MAX_TD_LOSS_NUMBER)
	{
		LogFmtStrA(SPLOGLV_ERROR, "TDSCDMA Loss count error %d > %d.", m_gs.common.nTdCnt, MAX_TD_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	swprintf_s(szTmp, L"%d", m_gs.common.nTdCnt);
	WritePrivateProfileStringW(L"TD", L"Count", szTmp, lpPath);
	GetLossFmtStr(SP_TDSCDMA, strFmt, TRUE, TRUE);

	for(int i=0; i<m_gs.common.nTdCnt; i++)
	{
		int nBand = m_gs.td[i].data[0].nBand;
		double dFreq = (double)m_gs.td[i].nFreq/10.0+0.001;
		int nArfcn = CTDUtility::TDMHz2Chan(dFreq);
		IMPEXP_ITEM_DATA data[DUL_ANT];
		for(int j=0; j<DUL_ANT; j++)
		{
			data[j].dPower = (double)((int16)m_gs.td[i].data[j].nExpPwr)/100.0+0.001;
			data[j].nLoss = m_gs.td[i].data[j].nLoss;
			data[j].nIndex = m_gs.td[i].data[j].nIndex;
		}
		swprintf_s(szTmp, strFmt.c_str(), nBand, nArfcn, dFreq
			, data[0].nLoss, data[0].nIndex, data[0].dPower
			, data[1].nLoss, data[1].nIndex, data[1].dPower);

		swprintf_s(szkey, L"Data%d", i);
		WritePrivateProfileStringW(L"TD", szkey, szTmp, lpPath);
	}
	NOTIFY("ExportTds", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CExportGoldenSample::ExportWcdma( LPCWSTR lpPath )
{
	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmt;
	if(m_gs.common.nWcdmaCnt == 0)
	{
		return SP_OK;
	}

	if(m_gs.common.nWcdmaCnt > MAX_WCDMA_LOSS_NUMBER)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WCDMA Loss count error %d > %d.", m_gs.common.nWcdmaCnt, MAX_WCDMA_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	swprintf_s(szTmp, L"%d", m_gs.common.nWcdmaCnt);
	WritePrivateProfileStringW(L"WCDMA", L"Count", szTmp, lpPath);
	GetLossFmtStr(SP_WCDMA, strFmt, TRUE, TRUE);

	for(int i=0; i<m_gs.common.nWcdmaCnt; i++)
	{
		int nBand = m_gs.wcdma[i].data[0].nBand;
		double dFreq = (double)m_gs.wcdma[i].nFreq/10.0+0.001;
		int nArfcn = CwcdmaUtility::WUlMHz2Chan((SP_BAND_INFO)nBand, dFreq);
		IMPEXP_ITEM_DATA data[DUL_ANT];
		for(int j=0; j<DUL_ANT; j++)
		{
			data[j].dPower = (double)((int16)m_gs.wcdma[i].data[j].nExpPwr)/100.0+0.001;
			data[j].nLoss = m_gs.wcdma[i].data[j].nLoss;
			data[j].nIndex = m_gs.wcdma[i].data[j].nIndex;
		}
		swprintf_s(szTmp, strFmt.c_str(), nBand+1, nArfcn, dFreq
			, data[0].nLoss, data[0].nIndex, data[0].dPower
			, data[1].nLoss, data[1].nIndex, data[1].dPower);

		swprintf_s(szkey, L"Data%d", i);
		WritePrivateProfileStringW(L"WCDMA", szkey, szTmp, lpPath);
	}
	NOTIFY("ExportWcdma", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CExportGoldenSample::ExportC2K( LPCWSTR lpPath )
{
	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmt;
	if(m_gs.common.nC2KCnt == 0)
	{
		return SP_OK;
	}

	if(m_gs.common.nC2KCnt > MAX_C2K_LOSS_NUMBER)
	{
		LogFmtStrA(SPLOGLV_ERROR, "C2K Loss count error %d > %d.", m_gs.common.nC2KCnt, MAX_C2K_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	swprintf_s(szTmp, L"%d", m_gs.common.nC2KCnt);
	WritePrivateProfileStringW(L"C2K", L"Count", szTmp, lpPath);
	GetLossFmtStr(SP_C2K, strFmt, TRUE, TRUE);

	for(int i=0; i<m_gs.common.nC2KCnt; i++)
	{
		int nBand = m_gs.c2k[i].data[0].nBand;
		int nArfcn = m_gs.c2k[i].nFreq;
		double dFreq = CCdmaUtility::Chan2MHz((SP_BAND_INFO)nBand, nArfcn, TRUE)+0.001;
		IMPEXP_ITEM_DATA data[DUL_ANT];
		for(int j=0; j<DUL_ANT; j++)
		{
			data[j].dPower = (double)((int16)m_gs.c2k[i].data[j].nExpPwr)/100.0+0.001;
			data[j].nLoss = m_gs.c2k[i].data[j].nLoss;
			data[j].nIndex = m_gs.c2k[i].data[j].nIndex;
		}
		swprintf_s(szTmp, strFmt.c_str(), nBand, nArfcn, dFreq
			, data[0].nLoss, data[0].nIndex, data[0].dPower
			, data[1].nLoss, data[1].nIndex, data[1].dPower);

		swprintf_s(szkey, L"Data%d", i);
		WritePrivateProfileStringW(L"C2K", szkey, szTmp, lpPath);
	}
	NOTIFY("ExportC2K", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CExportGoldenSample::ExportLTE( LPCWSTR lpPath )
{
	if(m_gs.common.usLteCnt == 0)
	{
		LogFmtStrA(SPLOGLV_INFO, "%s Len=0", __FUNCTION__);
		return SP_OK;
	}

	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmtTx, strFmtRx;
	if(SP_OK != LteDeserialization(m_arrLteInfo))
	{
		NOTIFY("LteDeserialization", LEVEL_ITEM, 1,0,1);
		return SP_E_FAIL;
	}
	if(m_vecLteData.size() == 0)
	{
		return SP_OK;
	}
	swprintf_s(szTmp, L"%d", m_vecLteData.size());
	WritePrivateProfileStringW(L"LTE", L"Count", szTmp, lpPath);
	GetLossFmtStr(SP_LTE, strFmtTx, TRUE, TRUE);
	GetLossFmtStr(SP_LTE, strFmtRx, TRUE, FALSE);
	for(int i=0; i<(int)m_vecLteData.size(); i++)
	{
		swprintf_s(szkey, L"Data%d", i);
		if(1 == m_vecLteData[i].nPath )
		{
			swprintf_s(szTmp, strFmtTx.c_str(), 
				m_vecLteData[i].TxData.Head.byBand, CLteUtility::m_BandInfo[m_vecLteData[i].TxData.Head.byBand].BandIdent, m_vecLteData[i].TxData.Head.uArfcn,
				(double)m_vecLteData[i].TxData.Head.usFreq/10.0+0.001, m_vecLteData[i].TxData.Head.byPath, m_vecLteData[i].ANT+1,
				(double)((int16)m_vecLteData[i].TxData.usPower)/100.0+0.001, m_vecLteData[i].TxData.usWord);
		}
		else
		{
			swprintf_s(szTmp, strFmtRx.c_str(), 
				m_vecLteData[i].RxData.Head.byBand, CLteUtility::m_BandInfo[m_vecLteData[i].RxData.Head.byBand].BandIdent, m_vecLteData[i].RxData.Head.uArfcn,
				(double)m_vecLteData[i].RxData.Head.usFreq/10.0+0.001, m_vecLteData[i].RxData.Head.byPath, m_vecLteData[i].ANT+1,
				(double)((int16)m_vecLteData[i].RxData.usRssi)/100.0+0.001, (double)((int16)m_vecLteData[i].RxData.usPower)/100.0+0.001, m_vecLteData[i].RxData.byIndex);
		}
		WritePrivateProfileStringW(L"LTE", szkey, szTmp, lpPath);
	}

	NOTIFY("ExportLTE", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CExportGoldenSample::ExportNR( LPCWSTR lpPath )
{
    LogFmtStrA(SPLOGLV_INFO, "%s Len=%d", __FUNCTION__, m_gs.common.usNrCnt);
    if (m_gs.common.usNrCnt == 0)
    {
        LogFmtStrA(SPLOGLV_INFO, "%s Len=0", __FUNCTION__);
        return SP_OK;
    }

    WCHAR szTmp[1024] = { 0 };
    WCHAR szkey[64] = { 0 };
    std::wstring strFmtTx, strFmtRx;
    if (SP_OK != NrDeserialization(&m_arrNrInfo))
    {
        NOTIFY("NrDeserialization", LEVEL_ITEM, 1, 0, 1);
        return SP_E_FAIL;
    }
    if (m_vecNrData.size() == 0)
    {
        return SP_OK;
    }
    swprintf_s(szTmp, L"%d", m_vecNrData.size());
    WritePrivateProfileStringW(L"NR", L"Count", szTmp, lpPath);
    GetLossFmtStr(SP_NR, strFmtTx, TRUE, TRUE);
    GetLossFmtStr(SP_NR, strFmtRx, TRUE, FALSE);
    for (int i = 0; i < (int)m_vecNrData.size(); i++)
    {
        LogFmtStrA(SPLOGLV_INFO, "m_vecNrData.size=%d, Data%d", i, m_vecNrData.size());
        swprintf_s(szkey, L"Data%d", i);
        if (m_vecNrData[i].bTx)
        {
            swprintf_s(szTmp, strFmtTx.c_str(),
                m_vecNrData[i].nBand, CNrUtility::m_BandInfo[m_vecNrData[i].nBand].BandIdent, m_vecNrData[i].uArfcn,
                m_vecNrData[i].dFreq, m_vecNrData[i].bTx, m_vecNrData[i].RfAnt, m_vecNrData[i].nAnt, m_vecNrData[i].nParam,
                m_vecNrData[i].dPower, m_vecNrData[i].dResult);
        }
        else
        {
            swprintf_s(szTmp, strFmtRx.c_str(),
                m_vecNrData[i].nBand, CNrUtility::m_BandInfo[m_vecNrData[i].nBand].BandIdent, m_vecNrData[i].uArfcn,
                m_vecNrData[i].dFreq, m_vecNrData[i].bTx, m_vecNrData[i].RfAnt, m_vecNrData[i].nAnt, m_vecNrData[i].nParam,
                m_vecNrData[i].dPower, m_vecNrData[i].dResult);
        }


        WritePrivateProfileStringW(L"NR", szkey, szTmp, lpPath);
        LogFmtStrA(SPLOGLV_INFO, "Data%d end", i, m_vecNrData.size());
    }

    NOTIFY("ExportNR", LEVEL_ITEM, 1, 1, 1);
    return SP_OK;
}

SPRESULT CExportGoldenSample::ExportWlan( LPCWSTR lpPath )
{
	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmt;
	if(m_gs.common.nWlanCnt == 0)
	{
		return SP_OK;
	}

	if(m_gs.common.nWlanCnt > MAX_WLAN_LOSS_NUMBER)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Wlan Loss count error %d > %d.", m_gs.common.nWlanCnt, MAX_WLAN_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	swprintf_s(szTmp, L"%d", m_gs.common.nWlanCnt);
	WritePrivateProfileStringW(L"WLAN", L"Count", szTmp, lpPath);
	GetLossFmtStr(SP_WIFI, strFmt, TRUE, TRUE);

	for(int i=0; i<m_gs.common.nWlanCnt; i++)
	{
		int nChan = m_gs.wlan[i].nCh;
		int nProtocol = (int)m_gs.wlan[i].data.nProtocalType;
		IMPEXP_ITEM_DATA data[MUL_ANT];
		for(int j=0; j<MUL_ANT; j++)
		{
			data[j].dPower = (double)m_gs.wlan[i].data.nPower[j]/100.0+0.001;
			data[j].nPath = (int8)m_gs.wlan[i].data.nPath[j];
		}
		swprintf_s(szkey, L"Data%d", i);
		swprintf_s(szTmp, strFmt.c_str(), nChan, nProtocol,
			data[0].nPath, data[0].dPower,  
			data[1].nPath, data[1].dPower,  
			data[2].nPath, data[2].dPower,  
			data[3].nPath, data[3].dPower);
		WritePrivateProfileStringW(L"WLAN", szkey, szTmp, lpPath);
	}
	NOTIFY("ExportWlan", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CExportGoldenSample::ExportBT( LPCWSTR lpPath )
{
	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmt;
	if(m_gs.common.nBtCnt == 0)
	{
		return SP_OK;
	}

	if(m_gs.common.nBtCnt > MAX_BT_LOSS_NUMBER)
	{
		LogFmtStrA(SPLOGLV_ERROR, "Bluetooth Loss count error %d > %d.", m_gs.common.nBtCnt, MAX_BT_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	swprintf_s(szTmp, L"%d", m_gs.common.nBtCnt);
	WritePrivateProfileStringW(L"BT", L"Count", szTmp, lpPath);
	GetLossFmtStr(SP_BT, strFmt, TRUE, TRUE);

	for(int i=0; i<m_gs.common.nBtCnt; i++)
	{
		int nChan = m_gs.bt[i].nCh;
		int nProtocol = (int)m_gs.bt[i].data.nProtocalType;
		IMPEXP_ITEM_DATA data[MUL_ANT];
		for(int j=0; j<MUL_ANT; j++)
		{
			data[j].dPower = (double)m_gs.bt[i].data.nPower[j]/100.0+0.001;
			data[j].nPath = (int8)m_gs.bt[i].data.nPath[j];
		}
		swprintf_s(szkey, L"Data%d", i);
		swprintf_s(szTmp, strFmt.c_str(), nChan, nProtocol,
			data[0].nPath, data[0].dPower,  
			data[1].nPath, data[1].dPower,  
			data[2].nPath, data[2].dPower,  
			data[3].nPath, data[3].dPower);
		WritePrivateProfileStringW(L"BT", szkey, szTmp, lpPath);
	}
	NOTIFY("ExportBT", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CExportGoldenSample::ExportGPS( LPCWSTR lpPath )
{
	WCHAR szTmp[1024] = {0};
	WCHAR szkey[64] = {0};
	std::wstring strFmt;
	if(m_gs.common.nGpsCnt == 0)
	{
		return SP_OK;
	}

	if(m_gs.common.nGpsCnt > MAX_GPS_LOSS_NUMBER)
	{
		LogFmtStrA(SPLOGLV_ERROR, "GPS Loss count error %d > %d.", m_gs.common.nGpsCnt, MAX_GPS_LOSS_NUMBER);
		return SP_E_SPAT_INVALID_PARAMETER;
	}

	swprintf_s(szTmp, L"%d", m_gs.common.nGpsCnt);
	WritePrivateProfileStringW(L"GPS", L"Count", szTmp, lpPath);
	GetLossFmtStr(SP_GPS, strFmt, TRUE, TRUE);

	for(int i=0; i<m_gs.common.nGpsCnt; i++)
	{
		int nChan = m_gs.gps[i].nCh;
		int nProtocol = (int)m_gs.gps[i].data.nProtocalType;
		IMPEXP_ITEM_DATA data[MUL_ANT];
		for(int j=0; j<MUL_ANT; j++)
		{
			data[j].dPower = (double)m_gs.gps[i].data.nPower[j]/100.0+0.001;
			data[j].nPath = (int8)m_gs.gps[i].data.nPath[j];
		}
		swprintf_s(szkey, L"Data%d", i);
		swprintf_s(szTmp, strFmt.c_str(), nChan, nProtocol,
			data[0].nPath, data[0].dPower,  
			data[1].nPath, data[1].dPower,  
			data[2].nPath, data[2].dPower,  
			data[3].nPath, data[3].dPower);
		WritePrivateProfileStringW(L"GPS", szkey, szTmp, lpPath);
	}
	NOTIFY("ExportGPS", LEVEL_ITEM, 1,1,1);
	return SP_OK;
}

SPRESULT CExportGoldenSample::LteDeserialization( std::vector<uint8> &arrInfo )
{
	LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
	LTE_Loss_Data stData;
	m_vecLteData.clear();
    uint16 usMainLength = *(uint16*)&arrInfo[0];
    uint16 usDivLength = *(uint16*)&arrInfo[2];
	LogFmtStrA(SPLOGLV_INFO, "MainLen=%d,DivLen=%d", usMainLength, usDivLength);

    uint16 usLength = usMainLength + usDivLength;
    uint16 usPosition = 0;

	if(usLength+4 != (uint16)arrInfo.size())
	{
		LogFmtStrA(SPLOGLV_ERROR, "LTE Loss data length Error0!");
		return SP_E_FAIL;
	}

    BOOL bEnd = FALSE;
    while (!bEnd)
    {
        if ((usLength - usPosition) < min(sizeof LTE_Loss_Data_TX, sizeof LTE_Loss_Data_RX))
        {
			LogFmtStrA(SPLOGLV_ERROR, "LTE Loss data length Error1!");
            return SP_E_FAIL;
        }
		stData.Init();
        if (usPosition < usMainLength)
        {
            stData.ANT = LTE_ANT_MAIN;
        }
        else
        {
            stData.ANT = LTE_ANT_DIV;
        }

        uint8* pData = &arrInfo[4] + usPosition;
        switch(*(pData + 7))
        {
        case 1:
            {
                stData.nPath = 1;
                stData.TxData = *((LTE_Loss_Data_TX*)pData);

                pData += sizeof LTE_Loss_Data_TX;
                usPosition += sizeof LTE_Loss_Data_TX;
            }
            break;
        case 2:
            {
                stData.nPath = 2;
                stData.RxData = *((LTE_Loss_Data_RX*)pData);

                pData += sizeof LTE_Loss_Data_RX;
                usPosition += sizeof LTE_Loss_Data_RX;
            }
            break;
        default:
            LogFmtStrA(SPLOGLV_ERROR, "Loss data Error0!");
            return SP_E_FAIL;
        }
		m_vecLteData.push_back(stData);
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

SPRESULT CExportGoldenSample::NrDeserialization( std::vector<uint8>* parrData)
{
	LogFmtStrA(SPLOGLV_INFO, "%s, size=%d", __FUNCTION__, parrData->size());
	uint32 uSize = parrData->size() / sizeof(GsData);
	m_vecNrData.resize(uSize);
	memcpy(&m_vecNrData[0], &(*parrData)[0], parrData->size());
	LogFmtStrA(SPLOGLV_INFO, "%s end", __FUNCTION__);
	return SP_OK;
}

