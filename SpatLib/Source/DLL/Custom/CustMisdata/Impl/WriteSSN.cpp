#include "StdAfx.h"
#include "WriteSSN.h"
#include "timo_crypt.h"
#include <fstream>

#define MISCDATA_FOR_SSN_LENGTH 47
#define MAX_KEY_NUMBER 5
#define MAX_KEY_ITEM_LENGTH 64

LPCSTR TIMO_ENCRYPT_MODE_STR[TIMO_ENCRYPT_MODE_MAX] = {"EncryptSSNToS1", "EncryptSSN", "EncryptSSNToJoy2S", "EncryptSSNToJoy3", "EncryptSSNToM4", "EncryptSSNToT10", "EncryptSSNToQ1", "EncryptSSNToK1PF", "EncryptSSNToTeemo"};

IMPLEMENT_RUNTIME_CLASS(CWriteSSN)
//////////////////////////////////////////////////////////////////////////
CWriteSSN::CWriteSSN(void)
{
	m_nEncryptMode = Mode_EncryptSSNToM4;
	m_strConfigPath = "";
	m_strProName = "";
	m_pCryptKey = NULL;
	m_bLookupKey = false;
	
	m_pCryptKey = new LPSTR [MAX_KEY_NUMBER];
	for (int k = 0; k < MAX_KEY_NUMBER; k++)
	{
		m_pCryptKey[k] = new char[MAX_KEY_ITEM_LENGTH];
	}
}

CWriteSSN::~CWriteSSN(void)
{
	for (int j = 0; j < MAX_KEY_NUMBER; j++)
	{
		delete []m_pCryptKey[j];
	}
	delete []m_pCryptKey;

}

BOOL CWriteSSN::LoadXMLConfig(void)
{
	std::string strMode = _W2CA(GetConfigValue(L"Option:EncryptMode", L""));
	if (0 == strMode.length())
	{
		return FALSE;
	}

	for (int i = 0; i < TIMO_ENCRYPT_MODE_MAX; i++)
	{
		if(0 == strcmp(strMode.c_str(), TIMO_ENCRYPT_MODE_STR[i]))
		{
			m_nEncryptMode = (ENCRYPT_MODE)i;
			break;
		}
	}

	if(Mode_EncryptSSNToTeemo == m_nEncryptMode)
	{
		m_strConfigPath = _W2CA(GetConfigValue(L"Option:TeemoConfig:ConfigFile", L""));
		if (0 == m_strConfigPath.length())
		{
			return FALSE;
		}

		m_strProName = _W2CA(GetConfigValue(L"Option:TeemoConfig:ProName", L""));
		if (0 == m_strProName.length())
		{
			return FALSE;
		}
	}

    return TRUE;
}

SPRESULT CWriteSSN::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "WriteSSN";
    INPUT_CODES_T inputBarCodes[BC_MAX_NUM];
	_UNISOC_MES_CMN_CODES mes_codes;
	CHAR szSN2[SP15_MAX_SN_LEN] = {0};
	SPRESULT sRet = SP_OK;
	MES_RESULT mesResult = MES_SUCCESS;
	if (Mode_EncryptSSNToTeemo == m_nEncryptMode)
	{
		sRet = LookupSSNKey(&m_pCryptKey[0]);
		if (SP_OK != sRet)
		{
			NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "LookupSSNKey");
			return sRet;
		}
	}

	//获取SN1
	do
	{
		//If mesonline 获取串码
		mesResult = UnisocMesActive();		//判断是否mes在线
		if(MES_SUCCESS == mesResult)
		{
			sRet = GetShareMemory(ShareMemory_MES_Assigned_Codes, (void* )&mes_codes, sizeof(mes_codes));
			if (SP_OK == sRet && 0 != strlen(mes_codes.sz_cmn_sn2))
			{
				uint32 uLength = strlen( mes_codes.sz_cmn_sn2);
				memcpy_s(szSN2, sizeof(szSN2),  mes_codes.sz_cmn_sn2, uLength > CopySize(szSN2) ? CopySize(szSN2) : uLength);
				break;
			}
		}

		//用户输入
		if (SP_OK == GetShareMemory(ShareMemory_My_UserInputSN, (void* )&inputBarCodes, sizeof(inputBarCodes)))
		{
			if(0 != strlen(inputBarCodes[BC_SN2].szCode))
			{
				uint32 uLength = strlen(inputBarCodes[BC_SN2].szCode);
				memcpy_s(szSN2, sizeof(szSN2), inputBarCodes[BC_SN2].szCode, uLength > CopySize(szSN2) ? CopySize(szSN2) : uLength);
				break;
			}
		}
		if (SP_OK == GetShareMemory(ShareMemory_SN2, (void* )szSN2, sizeof(szSN2)))
		{
			if(0 != strlen(szSN2))
			{
				break;
			}
		}

		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get SN2 fail");
		return SP_E_FAIL;
#pragma warning(disable:4127)
	}while(false);
#pragma warning(default:4127)

	//生成SSN
	std::string strSSN = "";
	switch(m_nEncryptMode)
	{
		case Mode_EncryptSSNToS1:
 			strSSN = timo_crypt::EncryptSSNToS1(szSN2);
			break;
		case Mode_EncryptSSN:
 			strSSN = timo_crypt::EncryptSSN(szSN2);
			break;
		case Mode_EncryptSSNToJoy2S:
 			strSSN = timo_crypt::EncryptSSNToJoy2S(szSN2);
			break;
		case Mode_EncryptSSNToJoy3:
 			strSSN = timo_crypt::EncryptSSNToJoy3(szSN2);
			break;
		case Mode_EncryptSSNToM4:
			strSSN = timo_crypt::EncryptSSNToQ1(szSN2);
			break;			
		case Mode_EncryptSSNToT10:
			strSSN = timo_crypt::EncryptSSNToT10(szSN2);
			break;
		case Mode_EncryptSSNToQ1:
			strSSN = timo_crypt::EncryptSSNToQ1(szSN2);
			break;
		case Mode_EncryptSSNToK1PF:
			strSSN = timo_crypt::EncryptSSNToK1PF(szSN2);
			break;
		case Mode_EncryptSSNToTeemo:
			{
				char szKey[MAX_KEY_NUMBER*MAX_KEY_ITEM_LENGTH] = {0};
				sprintf_s(szKey, sizeof(szKey),"%s,%s,%s,%s,%s", m_pCryptKey[0], m_pCryptKey[1], m_pCryptKey[2], m_pCryptKey[3], m_pCryptKey[4]);
				LogFmtStrA(SPLOGLV_INFO, "TimoCryptKey: %s", szKey);
				strSSN = timo_crypt::EncryptSSNToTeemo(szSN2, m_strProName.c_str(), (const char**)m_pCryptKey);
			}
			break;	
		default:
			break;
	}
	

	LogFmtStrA(SPLOGLV_INFO, "Timo Encrypt Mode:%s, Sn2:%s,Ssn:%s!", TIMO_ENCRYPT_MODE_STR[m_nEncryptMode], szSN2, strSSN.c_str());
	uint32 uLength = strSSN.length();
	strSSN.erase(uLength-1);//舍去最后一位
	if(MISCDATA_FOR_SSN_LENGTH != uLength)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Get SSN fail(SSN:%s length:%d isn't set %d)", strSSN.c_str(), uLength - 1, MISCDATA_FOR_SSN_LENGTH - 1);
		return SP_E_FAIL;
	}

	sRet = WriteCustMisdata(MISCDATA_BASE_ANDROID_OFFSET, strSSN.c_str(), strSSN.length(), ESYS_Android);
	if (SP_OK != sRet)
	{
		return sRet;
	}

	//If mesonline 添加到串码中
	if(MES_SUCCESS == mesResult)
	{
		memcpy_s(mes_codes.sz_cmn_code1, sizeof(mes_codes.sz_cmn_code1),  strSSN.c_str(), strSSN.length() > CopySize(mes_codes.sz_cmn_code1) ? CopySize(mes_codes.sz_cmn_code1) : strSSN.length());
		CHKRESULT(SetShareMemory(ShareMemory_MES_Assigned_Codes, (const void*)&mes_codes, sizeof(mes_codes)));
	}

	NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "\"szSSN:%s\"", strSSN.c_str());
    return SP_OK;
}

SPRESULT CWriteSSN::LookupSSNKey(LPSTR *pkeyVal)
{
	if (m_bLookupKey)
	{
		return SP_OK;
	}
	ifstream fsdata;
	int nfileSize = 0;
	char *pfileData = NULL;

	fsdata.open(m_strConfigPath.c_str(), ios::in);
	if (!fsdata)
	{
		return SP_E_OPEN_FILE;
	}

	fsdata.seekg(0, ios::end);
	nfileSize = (int)fsdata.tellg();
	fsdata.seekg(0, ios::beg);
	pfileData = (char*)malloc(nfileSize);
	if (!pfileData)
	{
		fsdata.close();
		return SP_E_ALLOC_MEMORY;
	}

	memset(pfileData, 0, nfileSize);
	fsdata.read(pfileData, nfileSize);
	fsdata.close();

	std::string strTemp = pfileData;
	free(pfileData);
	pfileData = NULL;

	std::string::size_type _nPos = strTemp.find(m_strProName.c_str(), 0);
	if (std::string::npos == _nPos)
	{
		return SP_E_INVALID_PARAMETER;
	}
	_nPos = strTemp.find("Crypt", _nPos + 1);
	if (std::string::npos == _nPos)
	{
		return SP_E_INVALID_PARAMETER;
	}

	_nPos = strTemp.find("=", _nPos + 1);
	if (std::string::npos == _nPos)
	{
		return SP_E_INVALID_PARAMETER;
	}

	std::string strCryptKey = strTemp.substr(_nPos + 1);
	_nPos = strCryptKey.find("\n", 0);
	if (0 == strCryptKey.length())
	{
		return SP_E_INVALID_PARAMETER;
	}

	strCryptKey.erase(_nPos);
	replace_all(strCryptKey, "\x20", "");

	std::string strSeps = ",";
	strCryptKey += strSeps;//方便分离

//	return SP_OK;
	uint8 _nCount = 0;
	_nPos = strCryptKey.find(strSeps, 0);
	while(std::string::npos != _nPos)
	{
		std::string key = strCryptKey.substr(0, _nPos);
		if (key.length() > MAX_KEY_ITEM_LENGTH)//防止过载
		{
			return SP_E_INVALID_PARAMETER;
		}
		memcpy(pkeyVal[_nCount], key.c_str(), key.length());
		pkeyVal[_nCount][key.length()] = '\0';
		_nCount++;
		strCryptKey.erase(0, _nPos + 1);
		_nPos = strCryptKey.find(strSeps, 0);
	}

	if (MAX_KEY_NUMBER != _nCount)
	{
		return SP_E_INVALID_PARAMETER;
	}

	m_bLookupKey = true;

	return SP_OK;
}
