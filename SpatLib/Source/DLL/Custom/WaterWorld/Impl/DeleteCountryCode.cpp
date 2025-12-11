#include "StdAfx.h"
#include "DeleteCountryCode.h"

IMPLEMENT_RUNTIME_CLASS(CDeleteCountryCode)

CDeleteCountryCode::CDeleteCountryCode(void)
{
	m_strSn = "";
	m_strMo = "";
	m_strCode = "";
	m_strSoftwarePN = "";
}

CDeleteCountryCode::~CDeleteCountryCode(void)
{
}

BOOL CDeleteCountryCode::LoadXMLConfig(void)
{
	m_strMo = _W2CA(GetConfigValue(L"Option:MO", L""));
	m_strSoftwarePN = _W2CA(GetConfigValue(L"Option:SoftwarePN", L""));

    return TRUE;
}

SPRESULT CDeleteCountryCode::__PollAction(void)
{

	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "SQLDriver");
	
	SPRESULT ret = CheckSnAllow();
	if (SP_OK != ret)
	{
		return ret;
	}
	
	ret = CheckCountryCode();
	if (SP_OK != ret)
	{
		return ret;
	}

	ret = ClearMiscData();
	if (SP_OK != ret)
	{
		return ret;
	}

	ret = SaveInfo2CCDeleteRecord();
	if (SP_OK != ret)
	{
		return ret;
	}
	return SP_OK;
}

SPRESULT CDeleteCountryCode::CheckSnAllow(void)
{
	CONST CHAR * ACTION = "CheckSnAllow";
	std::string strErrMsg;
	CHAR  szValue[3000] = {0};
	SPRESULT ret = SP_LoadSN(m_hDUT, SN1, szValue, sizeof(szValue));
	if (SP_OK != ret)
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "LoadSN fails");
		return SP_E_FAIL;
	}
	else
	{
		m_strSn = szValue;
	}


	if (TRUE != m_pSql_Imp->CheckTpControlBySn(m_strSn, strErrMsg))
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of CheckTpControlBySn, \'%s\', \'%s\'!", m_strSn.c_str(), strErrMsg.c_str());		
		return SP_E_MES_ERROR;
	}

	if (TRUE != m_pSql_Imp->CheckTpInputBySn(m_strSn, strErrMsg))
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of CheckTpInputBySn, \'%s\', \'%s\'!", m_strSn.c_str(), strErrMsg.c_str());		
		return SP_E_MES_ERROR;
	}

	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "Sn:\'%s\'",m_strSn.c_str());

	return SP_OK;
}

SPRESULT CDeleteCountryCode::ClearMiscData(void)
{
	CONST CHAR * ACTION = "ClearMiscData";
	//MISDATA_MAX_DETAILMODEL_LENGTH;// 最大不能超过 256KB,256*1024
	std::string::size_type nLen = MISDATA_MAX_DETAILMODEL_LENGTH;

	uint8 wBuff[MISDATA_MAX_DETAILMODEL_LENGTH+1] = {0};

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


SPRESULT CDeleteCountryCode::SaveInfo2CCDeleteRecord(void)
{
	CONST CHAR * ACTION = "SaveInfo2CCDeleteRecord";
	std::string strErrMsg;
	if (TRUE != m_pSql_Imp->AddInfo2CCDeleteRecord(m_strSn, m_strSoftwarePN, m_strMo, 1, strErrMsg))
	{
		NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of SaveInfo2CCDeleteRecord, \'%s\'!", strErrMsg.c_str());		
		return SP_E_MES_ERROR;
	}

	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "\'%s\'", m_strCode.c_str());

	return SP_OK;
}



SPRESULT CDeleteCountryCode::CheckCountryCode(void)
{
	CONST CHAR * ACTION = "CheckCountryCode";
	LPCSTR VER_CMD = "AT+PROP=0,[persist.radio.countrycode]";
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
			NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "CountryCode: \'%s\'", m_strRevBuf.c_str());
			return SP_E_INVALID_PARAMETER;
		}

		m_strRevBuf.erase(0, 1);
		m_strRevBuf.erase(m_strRevBuf.size() - 1, 1);
		if (2 != m_strRevBuf.size())
		{
			NOTIFY(ACTION, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "CountryCode: \'%s\'", m_strRevBuf.c_str());
			return SP_E_INVALID_PARAMETER;
		}
	}
	m_strCode = m_strRevBuf;
	NOTIFY(ACTION, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, m_strRevBuf.c_str());

	return SP_OK;
}