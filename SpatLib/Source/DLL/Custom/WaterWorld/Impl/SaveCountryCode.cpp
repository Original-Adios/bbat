#include "StdAfx.h"
#include "SaveCountryCode.h"



IMPLEMENT_RUNTIME_CLASS(CSaveCountryCode)

	CSaveCountryCode::CSaveCountryCode(void)
{
	m_strSn = "";
	m_strSmo = "";
	m_strCode = "";
	m_strDetailModel = "";
	m_strSoftwarePN = "";
	m_ckBatery = 1;
	m_dUpVoltage = 4.20;
	m_dDownVoltage = 3.40;
}

CSaveCountryCode::~CSaveCountryCode(void)
{
}

BOOL CSaveCountryCode::LoadXMLConfig(void)
{
	m_strSoftwarePN = _W2CA(GetConfigValue(L"Option:SoftwarePN", L""));
	m_ckBatery = (int)(GetConfigValue(L"Option:BatteryCheck:Active", 1));
	m_dUpVoltage = (double)(GetConfigValue(L"Option:BatteryCheck:Uplimit", 4.20));
	m_dDownVoltage = (double)(GetConfigValue(L"Option:BatteryCheck:Downlimit", 3.40));

	return TRUE;
}

SPRESULT CSaveCountryCode::__PollAction(void)
{

	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "SQLDriver");

	BarCode_T barcodes[MAX_BARCODE_NUM];
	SPRESULT ret = GetShareMemory(ShareMemory_PLinkInputCodes, (void*)&barcodes, sizeof(barcodes)*MAX_BARCODE_NUM);
	if (SP_OK != ret)
	{
		NOTIFY("No Barcodes", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
		return ret;
	}

	ret = CheckSnAllow(barcodes);
	if (SP_OK != ret)
	{
		return ret;
	}

	ret = CheckPhoneModel();
	if (SP_OK != ret)
	{
		return ret;
	}

	ret = CheckVoltage();
	if (SP_OK != ret)
	{
		return ret;
	}

	ret = CheckResetFlag();
	if (SP_OK != ret)
	{
		return ret;
	}

//	return SP_OK;
	ret = SaveMiscData();
	if (SP_OK != ret)
	{
		return ret;
	}

	ret = SaveInfo2TpCountryCode();
	if (SP_OK != ret)
	{
		return ret;
	}
	return SP_OK;
}


SPRESULT CSaveCountryCode::CheckSnAllow(BarCode_T* pBarcodes)
{
	CONST CHAR * ACTION = "CheckSnAllow";
	std::string strErrMsg;
	for (UINT i = 0; i < MAX_BARCODE_NUM; i++)
	{
		if (!pBarcodes[i].bActived) continue;
		if (0 == _stricmp(pBarcodes[i].szLabel, "SN1"))
		{
			m_strSn = pBarcodes[i].szCode;
			if (TRUE != m_pSql_Imp->CheckTpControlBySn(m_strSn, strErrMsg))
			{
				NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of CheckTpControlBySn, \'%s\',\'%s\'!", m_strSn.c_str(), strErrMsg.c_str());
				return SP_E_MES_ERROR;
			}

			if (TRUE != m_pSql_Imp->CheckTpInputBySn(m_strSn, strErrMsg))
			{
				NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of CheckTpInputBySn, \'%s\',\'%s\'!", m_strSn.c_str(), strErrMsg.c_str());
				return SP_E_MES_ERROR;
			}
		}

		if (0 == _stricmp(pBarcodes[i].szLabel, "CustFixed"))
		{
			m_strSmo = pBarcodes[i].szCode;
			if (TRUE != m_pSql_Imp->QueryCCFlagFrTpPlanByBatchName(m_strSmo, m_strCode, m_strDetailModel, strErrMsg))
			{
				NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of QueryCCFlagFrTpPlanByBatchName, \'%s\',\'%s\'!", m_strSmo.c_str(), strErrMsg.c_str());
				return SP_E_MES_ERROR;
			}
		}
	}


	CHAR  szValue[3000] = {0};
	SPRESULT ret = SP_LoadSN(m_hDUT, SN1, szValue, sizeof(szValue));
	if (SP_OK != ret || 0 != m_strSn.compare(szValue))
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Compare:\'%s\':\'%s\'", m_strSn.c_str(), szValue);
		return SP_E_FAIL;
	}

	if (m_strSn.empty() || m_strSmo.empty() || m_strCode.empty() || m_strDetailModel.empty())
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Empty of Sn:\'%s\',Smo:\'%s\',Code:\'%s\',DetailModel:\'%s\'",m_strSn.c_str(), m_strSmo.c_str(), m_strCode.c_str(), m_strDetailModel.c_str());
		return SP_E_MES_ERROR;
	}

	if (0 != m_strDetailModel.compare(m_strDetailModel.size() - 2, 2, m_strCode.c_str()))
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Dismatch CountryCode: \'%s\',\'%s\'", m_strDetailModel.c_str(), m_strCode.c_str());
		return SP_E_INVALID_PARAMETER;
	}
	
	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strCode.c_str());

	return SP_OK;
}


SPRESULT CSaveCountryCode::CheckPhoneModel(void)
{
	CONST CHAR * ACTION = "CheckPhoneModel";
	LPCSTR VER_CMD = "AT+PROP=0,[persist.sys.model.info]";
	SPRESULT res = SendATCommand(VER_CMD);
	if (SP_OK != res)
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, VER_CMD);
		return SP_E_PHONE_AT_EXECUTE_FAIL;
	}
	else
	{
		if (0 == m_strRevBuf.compare("NOT FOUND"))//NOT FOUND
		{
			NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
			return SP_OK;
		}

		m_strRevBuf.erase(0, 1);
		m_strRevBuf.erase(m_strRevBuf.size() - 1, 1);
		if (0 != m_strDetailModel.compare(0, 6, m_strRevBuf.c_str(), 0, 6))
		{
			NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Dismatch Model: \'%s\',\'%s\'", m_strDetailModel.c_str(), m_strRevBuf.c_str());
			return SP_E_INVALID_PARAMETER;
		}
	}
	
	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());
	return SP_OK;
}

SPRESULT CSaveCountryCode::SaveMiscData(void)
{
	CONST CHAR * ACTION = "SaveMiscData";
	//MISDATA_MAX_DETAILMODEL_LENGTH;// 最大不能超过 256KB,256*1024
	std::string::size_type nLen = m_strDetailModel.length();

	if (!IN_RANGE(1, nLen, MISDATA_MAX_DETAILMODEL_LENGTH))
	{
		LogFmtStrA(SPLOGLV_ERROR, "Invalid Country code, length = \'%d\'", nLen);
		return SP_E_INVALID_PARAMETER;
	}

	uint8 wBuff[MISDATA_MAX_DETAILMODEL_LENGTH + 1] = {0};
	memcpy(wBuff, m_strDetailModel.c_str(), nLen);

	//写入miscdata
	CHKRESULT_WITH_NOTIFY(SP_SaveMiscData(m_hDUT, MISCDATA_BASE_OFFSET, wBuff, nLen, TIMEOUT_3S), "SaveCustomerMiscData");

	Sleep(100);
	// 读取出来和写入的数据进行比较
	uint8 rbuff[MISDATA_MAX_DETAILMODEL_LENGTH+1] = {0};
	CHKRESULT_WITH_NOTIFY(SP_LoadMiscData(m_hDUT, MISCDATA_BASE_OFFSET, rbuff, nLen, TIMEOUT_3S), "LoadCustomerMiscData");

	rbuff[nLen] = '\0';
	if (0 != memcmp(wBuff, rbuff, nLen))
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Verify after write fails");
		return SP_E_MISMATCHED_CU;
	}

	return SP_OK;
}

SPRESULT CSaveCountryCode::SaveInfo2TpCountryCode(void)
{
	CONST CHAR * ACTION = "SaveInfo2TpCountryCode";
	std::string strErrMsg;

	if (TRUE != m_pSql_Imp->AddInfo2TpCountryCode(m_strSn, m_strSoftwarePN, m_strCode, m_strSmo, m_strCode, m_strDetailModel, strErrMsg))
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of SaveInfo2TpCountryCode, \'%s\'!", strErrMsg.c_str());
		return SP_E_MES_ERROR;
	}

	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Sn:\'%s\',SwPN:\'%s\',Smo:\'%s\',Code:\'%s\',DetailModel:\'%s\'",m_strSn.c_str(), m_strSoftwarePN.c_str(), m_strSmo.c_str(), m_strCode.c_str(), m_strDetailModel.c_str());

	return SP_OK;
}

SPRESULT CSaveCountryCode::CheckVoltage(void)
{
	if (!m_ckBatery)
	{
		return SP_OK;
	}
	CONST CHAR * ACTION = "CheckVoltage";
	uint32 input[MAX_AP_ADC_DATA_NUM] = {0};
	uint32 ouput[MAX_AP_ADC_DATA_NUM] = {0};
	CHKRESULT_WITH_NOTIFY(SP_apADC(m_hDUT, AP_ADC_FGU_VOLT, input, ouput), "SP_apADC(AP_ADC_FGU_VOLT)");
	float fmV = (float)(ouput[0]/1000.00);
	NOTIFY(ACTION, LEVEL_ITEM, m_dDownVoltage, fmV, m_dUpVoltage, NULL, -1, NULL, "\'%.2fV\'", fmV);

	return IN_RANGE(m_dDownVoltage, fmV, m_dUpVoltage) ? SP_OK : SP_E_SPAT_CHECK_Volt_FGU_Check;
}

SPRESULT CSaveCountryCode::CheckResetFlag(void)
{
	CONST CHAR * ACTION = "CheckResetFlag";
	std::string::size_type nLen = MISDATA_RESET_FLAG_LENGTH;
	uint8 rbuff[MISDATA_RESET_FLAG_LENGTH+1] = {0};
	CHKRESULT_WITH_NOTIFY(SP_LoadMiscData(m_hDUT, MISCDATA_RESET_FLAG_OFFSET, rbuff, nLen, TIMEOUT_3S), "LoadCustomerMiscData");
	if (0 == rbuff[0])
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "\'%s\'", "The Phone Hasn't Been Reset!");
		return SP_E_FAIL;
	}

	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "\'%s\'", "The Phone Has been Reset!");
	return SP_OK;
}