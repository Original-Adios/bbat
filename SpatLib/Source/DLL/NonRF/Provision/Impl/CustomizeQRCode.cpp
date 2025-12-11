#include "StdAfx.h"
#include "CustomizeQRCode.h"
#include "CLocks.h"
#include "../../../../../../Common/Json/JsonUtil.h"
#include "ActionApp.h"
#include "ShareMemoryDefine.h"
extern CActionApp myApp;
//
IMPLEMENT_RUNTIME_CLASS(CCustomizeQRCode)
//////////////////////////////////////////////////////////////////////////
CCustomizeQRCode::CCustomizeQRCode(void)
{
}

CCustomizeQRCode::~CCustomizeQRCode(void)
{
}

BOOL CCustomizeQRCode::LoadXMLConfig(void)
{
	m_QRCodeConfig.strFormat = "V4";
	m_QRCodeConfig.strGenType = "ScanCode";
	m_QRCodeConfig.strFixCode = "";

	m_QRCodeConfig.u32MaxLength = GetConfigValue(L"Option:QRCodeLength", 0);
	if (0 == m_QRCodeConfig.u32MaxLength)
	{
		SendCommonCallback(L"QRCode length must be set > 0.");
		return FALSE;
	}

	for (INT i = BC_START; i < BC_MAX_NUM; i++)
	{
		GetXMLConfig(&m_InputSN[i], CBarCodeUtility::m_BarCodeInfo[i].nameW);
		if (m_InputSN[i].bEnable)
		{
			if (0 == m_InputSN[i].nMaxLength)
			{
				std::wstring str = CBarCodeUtility::m_BarCodeInfo[i].nameW;
				str += L"length must be set > 0.";
				SendCommonCallback(str.c_str());
				return FALSE;
			}

			if (m_InputSN[i].nSectionStart > m_QRCodeConfig.u32MaxLength)
			{
				std::wstring str = CBarCodeUtility::m_BarCodeInfo[i].nameW;
				str += L"StartPosition can not be set > QRCodeLength.";
				SendCommonCallback(str.c_str());
				return FALSE;
			}
		}
	}

	return TRUE;
}

SPRESULT CCustomizeQRCode::__PollAction(void)
{
	// Lock resource while multi-thread 
	CLocks Lock(myApp.GetLock());

	// JSON file is used to transfer config parameter of html and code scanned.
	WCHAR szFile[MAX_PATH] = { 0 };
	swprintf_s(szFile, L"QRCode%d.Json", GetAdjParam().nTASK_ID);
	std::wstring strJsonFile = (std::wstring)_T2CW(myApp.GetSysPath()) + L"\\" + szFile;
	SetJsonConfig(strJsonFile.c_str());

	string_t strHtmlFile = (string_t)myApp.GetAppPath() + _T("\\Html\\InputQRCode\\InputCodes.html");

	WINDOW_PROPERTY Wnd;
	ZeroMemory((void*)&Wnd, sizeof(Wnd));
	Wnd.lpszTitle = L"ScanQRCode";
	Wnd.u32WndStyle = POPUP_WINDOW_CENTER;
	Wnd.cx = 600;
	Wnd.cy = 400;
	BOOL bApply = PopupHtmlView(Wnd, strHtmlFile.c_str(), strJsonFile.c_str());
	if (!bApply)
	{
		if (m_bAutoStart)
		{
			return SP_E_USER_ABORT;
		}
		else
		{
			LogRawStrA(SPLOGLV_ERROR, "User not input!");
			return SP_E_SPAT_USER_NOT_INPUT;
		}
	}
	else
	{
		SendWorkStatus();
	}

	QRCODE QRCode;
	if (!GetQRCode(QRCode, strJsonFile.c_str()))
	{
		NOTIFY("GetQRCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "no QRCode");
		return SP_E_UNKNOWN_BARCODE;
	}

	std::string strUsedCode;
	if (!ParseCode(QRCode, strUsedCode))
	{
		NOTIFY("ParseQRCode", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL);
		return SP_E_UNKNOWN_BARCODE;
	}

	CHKRESULT(SetShareMemory(ShareMemory_My_UserInputSN, (const void*)&m_InputSN[0], sizeof(m_InputSN)));
	NOTIFY("CustomizeQRCode", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "QRCode: %s", strUsedCode.c_str());
	return SP_OK;
}

void CCustomizeQRCode::SetJsonConfig(LPCTSTR lpszJsonFile)
{
	/* JSON format of configuration
	{
		"Codes": [
			{
				"CodeType":"QRCode",        // QRCode or BarCode
				"Label":"QR",               // Label, if this changed, do not forget change CImpBase::GetQRCode()
				"Enable":1                  // 1: Show
				"Format":"V1",              // QRCode format to analyse which is used in CImpBase::ParseIDFromQRCode()
				"Length":0,                 // 0: INFINITE, others means inputted code length must be equal to this setting
				"GenerateType":"ScanCode",  // ScanCode or FixCode, FixCode is usually for debugging
				"FixCode":""                // FixCode string
			}
		],
		"DUT":"DUT1-Port1"
	}
	*/

	CJsonUtil Json;
	Document::AllocatorType& allocator = Json.m_document.GetAllocator();
	Json.m_document.SetObject();

	Value vArray(kArrayType);
	vArray.Clear();

	Value vObject(kObjectType);
	Value vString(kStringType);

	// QR fixed type
	vString.SetString("QRCode", allocator);
	vObject.AddMember("CodeType", vString, allocator);

	vString.SetString("QR", allocator);
	vObject.AddMember("Label", vString, allocator);

	// Fixed enable
	vObject.AddMember("Enable", 1, allocator);

	vString.SetString(m_QRCodeConfig.strFormat.c_str(), allocator);
	vObject.AddMember("Format", vString, allocator);

	vObject.AddMember("Length", m_QRCodeConfig.u32MaxLength, allocator);

	vString.SetString(m_QRCodeConfig.strGenType.c_str(), allocator);
	vObject.AddMember("GenerateType", vString, allocator);

	vString.SetString(m_QRCodeConfig.strFixCode.c_str(), allocator);
	vObject.AddMember("FixCode", vString, allocator);

	vArray.PushBack(vObject, allocator);

	Json.m_document.AddMember("Codes", vArray, allocator);

	// Add DUT No. and Port No. to UI
	CHAR szBuf[64] = "";
	sprintf_s(szBuf, "DUT%d-PORT%d", GetAdjParam().nTASK_ID, GetAdjParam().ca.Com.dwPortNum);
	vString.SetString(szBuf, allocator);
	Json.m_document.AddMember("DUT", vString, allocator);

	Json.SaveJson(lpszJsonFile);
}

BOOL CCustomizeQRCode::GetQRCode(QRCODE& Code, LPCWSTR lpszJsonFile)
{
	if (NULL == lpszJsonFile)
	{
		assert(0);
		return FALSE;
	}


	CJsonUtil Json;
	Json.m_document.SetObject();
	if (Json.LoadJson(_W2CT(lpszJsonFile)))
	{
		LPCSTR QRLabel = "QR"; // This is the label of QR code scan text box which is shown on HTML.
		if (NULL == Json.m_document.HasMember(QRLabel))
		{
			return FALSE;
		}
		Value& QR = Json.m_document[QRLabel];

		LPCSTR CODE = "Code"; // Fix tag saved by HTML javascript file (Submit.js)
		if (NULL == QR.HasMember(CODE) || !QR[CODE].IsString())
		{
			return FALSE;
		}
		else
		{
			Code.strQRCode = QR[CODE].GetString();
		}

		LPCSTR FORMAT = "Format"; // Fix tag saved by HTML javascript file (Submit.js)
		if (NULL == QR.HasMember(FORMAT) || !QR[FORMAT].IsString())
		{
			return FALSE;
		}
		else
		{
			Code.strFormat = QR[FORMAT].GetString();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CCustomizeQRCode::ParseCode(const QRCODE& QRCode, std::string &strUsedCode)
{
	BOOL bRet = TRUE;
	CHAR* pszQRCode = NULL;
	int nQRCodeSize = sizeof(CHAR) * QRCode.strQRCode.length();

	try
	{
		pszQRCode = (CHAR*)new CHAR[nQRCodeSize + 1];
	}
	catch (const std::bad_alloc& /*e*/)
	{
		pszQRCode = NULL;
	}

	if (NULL == pszQRCode)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"malloc Buffer fail");
		return FALSE;
	}

	memset((void*)pszQRCode, 0, nQRCodeSize);
	memcpy_s((CHAR*)pszQRCode, nQRCodeSize, (CHAR*)QRCode.strQRCode.c_str(), nQRCodeSize);
	pszQRCode[nQRCodeSize] = '\0';


	for (INT iCode = 0; iCode < BC_MAX_NUM; iCode++)
	{
		if (m_InputSN[iCode].bEnable)
		{
			int nStartPosition = m_InputSN[iCode].nSectionStart;
			memcpy_s(m_InputSN[iCode].szCode, sizeof(m_InputSN[iCode].szCode), (char*)&pszQRCode[nStartPosition], m_InputSN[iCode].nMaxLength);

			if (m_InputSN[iCode].nMaxLength != strlen(m_InputSN[iCode].szCode))
			{
				LogFmtStrA(SPLOGLV_ERROR, "%s input length error %d", m_InputSN[iCode].szCodeName, strlen(m_InputSN[iCode].szCode));
				bRet = FALSE;
				break;
			}

			if (IS_IMEI_CODE(iCode))
			{					
				if (!CheckStringIsDigit(m_InputSN[iCode].szCode, m_InputSN[iCode].nMaxLength))
				{
					LogFmtStrA(SPLOGLV_ERROR, "%s input is not digit %s", m_InputSN[iCode].szCodeName, m_InputSN[iCode].szCode);
					bRet = FALSE;
					break;
				}

				int64 nImei = _atoi64(m_InputSN[iCode].szCode);
				int64 nCD = ComputeImeiCD(nImei / 10);
				int64 nImeiCD = nImei % 10;
				if (nImeiCD != nCD)
				{
					LogFmtStrA(SPLOGLV_ERROR, "%s Invalid Imei: %s", m_InputSN[iCode].szCodeName, m_InputSN[iCode].szCode);
					bRet = FALSE;
					break;
				}
			}
			if (IS_MEID_CODE(iCode))
			{
				char cCheckCode = '\0';
				bRet = ComputeMeidCD(m_InputSN[iCode].szCode, cCheckCode);
				char cTailCode = m_InputSN[iCode].szCode[m_InputSN[iCode].nMaxLength - 1];
				if (!(bRet && cCheckCode == cTailCode))
				{
					LogFmtStrA(SPLOGLV_ERROR, "%s Invalid meid: %s", m_InputSN[iCode].szCodeName, m_InputSN[iCode].szCode);
					bRet = FALSE;
					break;
				}
			}

			if (IS_HEX_CODE(iCode))
			{
				if (!CheckHexString(m_InputSN[iCode].szCode, m_InputSN[iCode].nMaxLength))
				{
					LogFmtStrA(SPLOGLV_ERROR, "%s Invalid hex fromat data: %s", m_InputSN[iCode].szCodeName, m_InputSN[iCode].szCode);
					bRet = FALSE;
					break;
				}
			}

			strUsedCode += m_InputSN[iCode].szCodeName;
			strUsedCode += "=";
			strUsedCode += m_InputSN[iCode].szCode;
			strUsedCode += ";";
		}
	}

	delete[]pszQRCode;
	pszQRCode = NULL;

	return bRet;
}

void CCustomizeQRCode::GetXMLConfig(INPUT_CODES_T* pInput, std::wstring strCodeName)
{
	std::wstring strKeyName;
	std::wstring strBuf;

	strKeyName = L"Option:" + strCodeName + L":Enable";
	pInput->bEnable = (UINT8)GetConfigValue(strKeyName.c_str(), 0);

	strcpy_s(pInput->szCodeName, _W2CA(strCodeName.c_str()));

	strKeyName = L"Option:" + strCodeName + L":Length";
	pInput->nMaxLength = (UINT8)GetConfigValue(strKeyName.c_str(), 14);


	//Prefix
	strKeyName = L"Option:" + strCodeName + L":StartPosition";
	strBuf = GetConfigValue(strKeyName.c_str(), L"0");
	pInput->nSectionStart = _wcstoui64(strBuf.c_str(), 0, 10);
}