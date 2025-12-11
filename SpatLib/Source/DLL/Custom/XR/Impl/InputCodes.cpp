#include "StdAfx.h"
#include "InputCodes.h"
#include "CLocks.h"
#include "ActionApp.h"
#include <sstream>
#include <time.h>

IMPLEMENT_RUNTIME_CLASS(CInputCodes)
#define STR_GETIMEI2FROMIMEI1 "FromImei1"

LPCSTR GEN_CODE_TYPE_STR[E_GENCODE_MAX] = {"ManualInput", "AutoGenerate", "Section", "SameImei1", "FromImei1","SameSN1"};
    //////////////////////////////////////////////////////////////////////////
CInputCodes::CInputCodes(void)
{
	m_bCheckXmode = FALSE;
	m_bCheckInputKuCodes = FALSE;
	m_nKudaSnLength = 0;
}

CInputCodes::~CInputCodes(void)
{
}

BOOL CInputCodes::LoadXMLConfig(void)
{
    if (!CImpBase::LoadXMLConfig())
    {
        return FALSE;
    }

    /* 
    @JXP 20180517: 因软件ro.serialno会取用这个SN，将用于Google SERIAL
    Google SERAIL要求: ^([a-zA-Z0-9]{6,20})$
    若不符合上述要求，会导致Google Play无法使用
    */
    INT nMaxSN1Length = GetConfigValue(L"Option:SN1:MaxLength", 14);
    if (nMaxSN1Length < 6 || nMaxSN1Length > 20)
    {
        return FALSE;
    }

    m_bCheckXmode = (BOOL)GetConfigValue(L"Option:ChecXMode", FALSE);

    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        GetXMLConfig(&m_InputSN[i], CBarCodeUtility::m_BarCodeInfo[i].nameW, IS_HEX_CODE(i));
    }

	//manualInputCode
	GetXMLConfig(&m_InputManualCode, L"ManualInput");

	//XunRui by binglin.wen 
	GetXMLConfig(&m_InputAppid,L"Appid");
	m_bCheckInputKuCodes = (BOOL)GetConfigValue(L"Option:KudaCodes", FALSE);
	m_nKudaSnLength = (int)GetConfigValue(L"Option:KudaSnMaxLength", 20);
	if (m_nKudaSnLength < 6 || m_nKudaSnLength > 23)
	{
		return FALSE;
	}
	SetKudaInputCodes();

    // 处理Special [6/22/2017 jian.zhong]
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (SP_OK != InitGenCode((BC_INDEX)i))
		{
			LogFmtStrA(SPLOGLV_ERROR, "InitGenCode Fail , code Section param error!");
			return FALSE;
		}
    }

	if (m_InputSN[BC_IMEI2].bEnable && !m_InputSN[BC_IMEI1].bEnable)
	{
		if (E_GENCODE_FROMIMEI1 == m_InputSN[BC_IMEI2].eGenCodeType || E_GENCODE_SAMEASIMEI1 == m_InputSN[BC_IMEI2].eGenCodeType)
		{
			LogFmtStrA(SPLOGLV_ERROR, "IMEI2 GenerateType Error, IMEI1 should be enable!");
			return FALSE;
		}
	}
	//
	if (m_InputSN[BC_SN2].bEnable && !m_InputSN[BC_SN1].bEnable)
	{
		if(E_GENCODE_SAMEASSN1 == m_InputSN[BC_SN2].eGenCodeType)
		{
			LogFmtStrA(SPLOGLV_ERROR, "SN2 GenerateType Error, SN1 should be enable!");
			return FALSE;
		}
	}

    BOOL bOK = FALSE;
    SetShareMemory(ShareMemory_My_UserInputSN, (void *)&bOK, sizeof(bOK), IContainer::System);
    return TRUE;
}

SPRESULT CInputCodes::InitGenCode(BC_INDEX index)
{
	GEN_SECTION_CODES_T Data;
//   if(E_GENCODE_SECTION == m_InputSN[index].eGenCodeType)
   {
	   strcpy_s(Data.szPrefix, m_InputSN[index].szPrefix);
	   Data.nSectionStart =  m_InputSN[index].nSectionStart;
	   Data.nSectionEnd =  m_InputSN[index].nSectionEnd;
	   Data.nMaxLength = m_InputSN[index].nMaxLength;
	   
	   //IMEI自动生成不需要最后一位
	   if (IS_GEID_CODE(index) && (E_GENCODE_SECTION == m_InputSN[index].eGenCodeType || E_GENCODE_AUTO == m_InputSN[index].eGenCodeType))
	   {
		   Data.nMaxLength--;
	   }
	   
	   CHKRESULT(m_GenCodes[index].Init(CBarCodeUtility::m_BarCodeInfo[index].nameW, this, &Data));
   }
    return SP_OK;
}

SPRESULT CInputCodes::__PollAction(void)
{   
    // Lock resource while multi-thread 
    extern CActionApp myApp;
    CLocks Lock(myApp.GetLock());

    Value vArray(kArrayType);
	INPUT_CODES_T stImei2;
    Document::AllocatorType &allocator = m_Json.m_document.GetAllocator();
    m_Json.m_document.SetObject();
    BOOL bUpdateSN = FALSE;
    SPRESULT res = GetShareMemory(ShareMemory_My_UpdateSN, (void *)&bUpdateSN, sizeof(bUpdateSN));
    if (SP_OK != res)
    {
        LogFmtStrW(SPLOGLV_ERROR, L"Get share memory < %s > failure!", ShareMemory_My_UserInputSN);
        //return res;
    }

	//XunRui
	if (m_InputManualCode.bEnable)
	{
		SetJsConfig_XR(&m_InputManualCode, &vArray, allocator);
	}

    //BT WIFI 在获取到数字之后，需要转化成16进制
    string Strtmp = "";
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (!m_InputSN[i].bEnable)
        {
            continue;
        }
        if (E_GENCODE_SECTION == m_InputSN[i].eGenCodeType)
        {
			INPUT_CODES_T stCode[2];
			BOOL bHex = FALSE;
			if (IS_HEX_CODE(i))
			{
				bHex = TRUE;
			}
			int nNum = 1;
			if(BC_IMEI1 == i && m_InputSN[BC_IMEI2].bEnable && E_GENCODE_FROMIMEI1 == m_InputSN[BC_IMEI2].eGenCodeType)
			{
				nNum = 2;
			}
			//清除当前Task失败的串号
			m_GenCodes[i].ClearPreAllocForTask();
			CHKRESULT_WITH_NOTIFY(m_GenCodes[i].PreAllocCodes(&stCode[0], nNum, bHex), "PreAlloc");
			strcpy_s(m_InputSN[i].szInitCode, stCode[0].szCode);
			m_InputSN[i].nSectionID = stCode[0].nSectionID;
			if(BC_IMEI1 == i && m_InputSN[BC_IMEI2].bEnable && E_GENCODE_FROMIMEI1 == m_InputSN[BC_IMEI2].eGenCodeType)
			{
				stImei2 = stCode[1];
			}
        }
		else if(E_GENCODE_FROMIMEI1 == m_InputSN[i].eGenCodeType)
		{
			if(i != BC_IMEI2)
			{
				LogFmtStrA(SPLOGLV_ERROR, "IMEI1 should not be the Same with IMEI2, Please Check!");
				return SP_E_SPAT_CODES_FROMIMEI1;
			}

			if(E_GENCODE_SECTION == m_InputSN[BC_IMEI1].eGenCodeType)
			{
				strcpy_s(m_InputSN[i].szInitCode, stImei2.szCode);
				m_InputSN[i].nSectionID = stImei2.nSectionID;
			}
			else
			{
				/*
				CHAR szIMEI1[BARCODEMAXLENGTH] = {0};
				strncpy_s(szIMEI1, m_InputSN[BC_IMEI1].szCode, 14);
				unsigned __int64 nImei1 = _strtoui64(szIMEI1, NULL, 10);
				sprintf_s(m_InputSN[i].szInitCode, "%14I64ud" , nImei1+1);
				*/
			}
		}
		//SN2 the same with SN1
		if (i == BC_SN2 && m_InputSN[BC_SN2].eGenCodeType == E_GENCODE_SAMEASSN1)
        {
            continue;
        }
		
        SetJsConfig((BC_INDEX)i, &vArray, allocator);
    }

	//XunRui Appid by binglin.wen
	if(m_InputAppid.bEnable)
	{
		SetJsConfig_XR(&m_InputAppid, &vArray, allocator);
	}
	//XunRui Kuda by binglin.wen
	if(m_bCheckInputKuCodes)
	{
		SetJsConfig_XR(&m_DeviceId, &vArray, allocator);
		SetJsConfig_XR(&m_KudaSN, &vArray, allocator);
		SetJsConfig_XR(&m_OneNetDevId, &vArray, allocator);
		SetJsConfig_XR(&m_OneNetProductId, &vArray, allocator);
		SetJsConfig_XR(&m_DevSecret, &vArray, allocator);
	}
	//
    m_Json.m_document.AddMember("Barcodes", vArray, allocator);

	//add DUT No. and Port No. to UI
	Value vDut(kStringType);
	char szBuf[64] = "";
	SPAT_INIT_PARAM spat_init_param = GetAdjParam();
	sprintf_s(szBuf, "DUT%d-Port%d", spat_init_param.nTASK_ID, spat_init_param.ca.Com.dwPortNum);
	vDut.SetString(szBuf, allocator);
	m_Json.m_document.AddMember("DutNo", vDut, allocator);
    //

    // Update JSON file: "Bin\sys\InputCodes.Json"
    if (!SaveJsonFile())
    {
        NOTIFY("Save Json", LEVEL_UI, 1, 0, 1);
        return SP_E_SPAT_TEST_FAIL;
    }
    bUpdateSN = FALSE;
    SetShareMemory(ShareMemory_My_UpdateSN, (void *)&bUpdateSN, sizeof(bUpdateSN), IContainer::System);
    // Show HTML
    UINT8 nInputNumber = 0;
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (m_InputSN[i].bEnable)
        {
            nInputNumber++;
        }
    }

    WCHAR szHtmlFile[MAX_PATH] = {0};
    swprintf_s(szHtmlFile, L"%s\\InputCodes\\InputCodes.html", _T2CW(m_szHtmlPath));

    WINDOW_PROPERTY Wnd;
    ZeroMemory((void *)&Wnd, sizeof(Wnd));
    Wnd.lpszTitle = L"请输入条码";
    Wnd.u32WndStyle = POPUP_WINDOW_CENTER;
    Wnd.cx = 530;
    Wnd.cy = 340 + 40 * nInputNumber;
    BOOL bApply = PopupHtmlView(Wnd, szHtmlFile, NULL);
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
        //  AutoStart模式下启动界面刷新和显示
        SendWorkStatus();
    }

    // Get HTML user input by Json file
    if (!LoadJsonFile())
    {
        NOTIFY("Load Json", LEVEL_UI, 1, 0, 1);
        return SP_E_SPAT_TEST_FAIL;
    }

    /*
    {
    "SN1" : "111111111111111111111",
    "SN2" : "222222222222222222222",
    "IMEI1" : "111111111111111111111",
    "IMEI2" : "222222222222222222222",
    "IMEI3" : "111111111111111111111",
    "IMEI4" : "222222222222222222222",
    "WIFI" : "111111111111111111111",
    "BT" : "222222222222222222222"
    }
    */

    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
		//SN2 the same with SN1
		if (i == BC_SN2 && E_GENCODE_SAMEASSN1 == m_InputSN[BC_SN2].eGenCodeType  && m_InputSN[BC_SN2].bEnable)
        {
            continue;
        }
        CHKRESULT(GetInputCode((BC_INDEX)i));
    }
	//XunRui Appid by binglin.wen
	//获取Appid
	CHKRESULT(GetInputCode_XR(&m_InputAppid));
	if(m_bCheckInputKuCodes)
	{
		CHKRESULT(GetInputCode_XR(&m_DeviceId));
		CHKRESULT(GetInputCode_XR(&m_KudaSN));
		CHKRESULT(GetInputCode_XR(&m_OneNetDevId));
		CHKRESULT(GetInputCode_XR(&m_OneNetProductId));
		CHKRESULT(GetInputCode_XR(&m_DevSecret));
	}

	CHKRESULT(GetInputCode_XR(&m_InputManualCode));

	//SN2 the same with SN1
	if (m_InputSN[BC_SN2].bEnable && m_InputSN[BC_SN2].eGenCodeType == E_GENCODE_SAMEASSN1)
    {
		strncpy_s(m_InputSN[BC_SN2].szCode, m_InputSN[BC_SN1].szCode, BARCODEMAXLENGTH);
	}
	
    if (!m_bCheckXmode && m_InputSN[BC_IMEI1].bEnable && m_InputSN[BC_IMEI2].bEnable && E_GENCODE_SAMEASIMEI1 != m_InputSN[BC_IMEI2].eGenCodeType)
    {
        string strBuf = m_InputSN[BC_IMEI1].szCode;
        if (0 == strBuf.compare(0, strBuf.length(), m_InputSN[BC_IMEI2].szCode))
        {
            LogFmtStrA(SPLOGLV_ERROR, "IMEI1 should not be the Same with IMEI2, Please Check!");
            return SP_E_SPAT_IMEI1_IMEI2_NOT_THE_SAME;
        }
    }

    //Print info
    for (INT i=BC_START; i<BC_MAX_NUM; i++)
    {
        if (!m_InputSN[i].bEnable)
        {
            continue;
        }
        if (CBarCodeUtility::m_BarCodeInfo[i].lpDutInfoKey != NULL)
        {
            NOTIFY(CBarCodeUtility::m_BarCodeInfo[i].lpDutInfoKey, m_InputSN[i].szCode);
        }

        LogFmtStrA(SPLOGLV_DATA, "Input %s = %s", m_InputSN[i].szCodeName, m_InputSN[i].szCode);
        std::string strItem = "Input ";
        strItem += m_InputSN[i].szCodeName;
        std::string strInfo = m_InputSN[i].szCode;
        //strInfo = "'" + strInfo ;
        NOTIFY(strItem.c_str(), LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, "-", strInfo.c_str());
        NOTIFY(strItem.c_str(), LEVEL_REPORT, NOLOWLMT, 1, NOUPPLMT, NULL, -1, "-", ("'" + strInfo).c_str());
    }

	//XunRui by binglin.wen
	//appid
	if(m_InputAppid.bEnable)
	{
		//去除APPID32位限制
		//if(strlen(m_InputAppid.szCode) != 32)
		//{
		//	NOTIFY("InputCodes", LEVEL_UI, 1, 1, 1, NULL, -1, "Length of Appid is not 32");
		//	return SP_E_SPAT_TEST_FAIL;
		//}
	    LogFmtStrA(SPLOGLV_DATA, "Input %s = %s", m_InputAppid.szCodeName, m_InputAppid.szCode);
        std::string strItem = "Input ";
        strItem += m_InputAppid.szCodeName;
        std::string strInfo = m_InputAppid.szCode;
        //strInfo = "'" + strInfo ;
        NOTIFY(strItem.c_str(), LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, "-", strInfo.c_str());
        NOTIFY(strItem.c_str(), LEVEL_REPORT, NOLOWLMT, 1, NOUPPLMT, NULL, -1, "-", ("'" + strInfo).c_str());
		//
		CHKRESULT(SetShareMemory(ShareMemory_XunRuiInputAppid, (const void *)&m_InputAppid, sizeof(INPUT_CODES_T)));
	}
	//Kuda
	if(m_bCheckInputKuCodes)
	{
		PrintKudaCodesInfo();
        char KudaCodes[128] = {0};
		sprintf_s(KudaCodes,"%s_%s_%s_%s_%s",m_DeviceId.szCode,m_KudaSN.szCode,m_OneNetDevId.szCode,m_OneNetProductId.szCode,m_DevSecret.szCode);
		CHKRESULT(SetShareMemory(ShareMemory_XunRuiInputKuda, (const void *)KudaCodes, sizeof(KudaCodes)));
	}

	//ManualInputCode
	if (m_InputManualCode.bEnable)
	{
		LogFmtStrA(SPLOGLV_DATA, "Input %s = %s", m_InputManualCode.szCodeName, m_InputManualCode.szCode);
		std::string strItem = "Input ";
		strItem += m_InputManualCode.szCodeName;
		std::string strInfo = m_InputManualCode.szCode;
		//strInfo = "'" + strInfo ;
		NOTIFY(strItem.c_str(), LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, "-", strInfo.c_str());
		NOTIFY(strItem.c_str(), LEVEL_REPORT, NOLOWLMT, 1, NOUPPLMT, NULL, -1, "-", ("'" + strInfo).c_str());
		//
		CHKRESULT(SetShareMemory(ShareMemory_XunRuiInputManualCode2Compare, (const void*)&m_InputManualCode, sizeof(INPUT_CODES_T)));
	}


    // Set share memory
    CHKRESULT(SetShareMemory(ShareMemory_My_UserInputSN, (const void *)&m_InputSN[0], sizeof(INPUT_CODES_T) * BC_MAX_NUM));
    //NOTIFY("InputCodes", LEVEL_UI, 1, 1, 1, NULL, -1, "");
//	CHKRESULT(Debug())
    return SP_OK;
}

void CInputCodes::GetXMLConfig(INPUT_CODES_T *pInput, std::wstring strCodeName, BOOL bHex)
{
    std::wstring strKeyName;
    std::wstring strBuf;

    strKeyName = L"Option:" + strCodeName + L":Enable";
    pInput->bEnable = (UINT8)GetConfigValue(strKeyName.c_str(), 0);

    WideCharToMultiByte(CP_ACP, 0, strCodeName.c_str(), -1, pInput->szCodeName, 30, 0, 0);

    strKeyName = L"Option:" + strCodeName + L":MaxLength";
    pInput->nMaxLength = (UINT8)GetConfigValue(strKeyName.c_str(), 14);

    //GenerateType
    strKeyName = L"Option:" + strCodeName + L":GenerateType";
    std::string sGenerateType = _W2CA(GetConfigValue(strKeyName.c_str(), L"ManualInput"));
    for(int i=E_GENCODE_MAN; i<E_GENCODE_MAX; i++)
	{
		if(0 == strcmp(sGenerateType.c_str(), GEN_CODE_TYPE_STR[i]))
		{
			pInput->eGenCodeType = (E_GEN_CODE_TYPE)i;
		}
	}

    //Prefix
    strKeyName = L"Option:" + strCodeName + L":Prefix";
    std::string sPrefix = _W2CA(GetConfigValue(strKeyName.c_str(), L""));
	strcpy_s(pInput->szPrefix, sPrefix.c_str());
    // Section  [7/7/2017 jian.zhong]
    strKeyName = L"Option:" + strCodeName + L":SectionStart";
	std::string strStart = _W2CA(GetConfigValue(strKeyName.c_str(), L"0"));
	if(bHex)
	{
		pInput->nSectionStart = _strtoui64(strStart.c_str(), NULL, 16);
	}
	else
	{
		pInput->nSectionStart = _strtoui64(strStart.c_str(), NULL, 10);
	}
    
	strKeyName = L"Option:" + strCodeName + L":SectionEnd";
	std::string strEnd = _W2CA(GetConfigValue(strKeyName.c_str(), L"0"));
    if(bHex)
	{
		
		pInput->nSectionEnd = _strtoui64(strEnd.c_str(), NULL, 16);
	}
	else
	{
		 pInput->nSectionEnd = _strtoui64(strEnd.c_str(), NULL, 10);
	}
	   
    //Validation
    strKeyName = L"Param:" + strCodeName + L":Validation";
    std::string sVaildation = _W2CA(GetConfigValue(strKeyName.c_str(), L""));
    strcpy_s(pInput->szValidation, sVaildation.c_str());
}

void CInputCodes::SetJsConfig(BC_INDEX eIndex, Value *vArray, Document::AllocatorType &allocator)
{
    Value vObject(kObjectType);
    Value vString(kStringType);
    //Document::AllocatorType& allocator = m_Json.m_document.GetAllocator();
    if (m_bCheckXmode && strlen(m_InputSN[eIndex].szPrefix) > 0)
    {
        //CheckX 模式下，输入前缀之后，不用弹出输入框，只要在后面检测时，检测手机中的条码是否符合前缀
        return;
    }
	E_GEN_CODE_TYPE eGenType = m_InputSN[eIndex].eGenCodeType;
	if( BC_IMEI2 == eIndex)
	{
		if(E_GENCODE_SAMEASIMEI1 == eGenType || E_GENCODE_FROMIMEI1 == eGenType)
		{
			return;
		}

		/*
		if(E_GENCODE_FROMIMEI1 == eGenType)
		{
			eGenType = E_GENCODE_SECTION;
		}
		*/
	}
    
    vString.SetString(m_InputSN[eIndex].szCodeName, allocator);
    vObject.AddMember("Name", vString, allocator);

    vObject.AddMember("Enable", m_InputSN[eIndex].bEnable, allocator);

    vObject.AddMember("MaxLength", m_InputSN[eIndex].nMaxLength, allocator);

    vString.SetString(GEN_CODE_TYPE_STR[eGenType], allocator);
    vObject.AddMember("GenerateType", vString, allocator);

    vString.SetString(m_InputSN[eIndex].szPrefix, allocator);
    vObject.AddMember("Prefix", vString, allocator);

    vString.SetString(m_InputSN[eIndex].szValidation, allocator);
    vObject.AddMember("Validation", vString, allocator);

    vString.SetString(m_InputSN[eIndex].szInitCode, allocator);
    vObject.AddMember("InitBarCode", vString, allocator);

    vObject.AddMember("EnableInputCheck", 1, allocator);

    vArray->PushBack(vObject, allocator);
}

SPRESULT CInputCodes::GetInputCode(BC_INDEX eIndex)
{
    if (m_InputSN[eIndex].bEnable)
    {
        LPCSTR lpCode = m_Json.GetString(m_InputSN[eIndex].szCodeName);
        if (NULL == lpCode)
        {
            if (m_bCheckXmode && strlen(m_InputSN[eIndex].szPrefix) > 0)
			{
				LogFmtStrA(SPLOGLV_INFO, "CheckX mode, when PrefixMode Skip! ");
                return SP_OK;
            }
            //if (!m_bCheckXmode)
            if (E_GENCODE_SAMEASIMEI1 == m_InputSN[eIndex].eGenCodeType)
            {
				strncpy_s(m_InputSN[BC_IMEI2].szCode, m_InputSN[BC_IMEI1].szCode, BARCODEMAXLENGTH);
                return SP_OK;
            }
			else if(E_GENCODE_FROMIMEI1 == m_InputSN[eIndex].eGenCodeType)
			{
				CHAR szIMEI1[BARCODEMAXLENGTH] = {0};
				strncpy_s(szIMEI1, m_InputSN[BC_IMEI1].szCode, 14);
				unsigned __int64 nImei1 = _strtoui64(szIMEI1, NULL, 10);
				sprintf_s(m_InputSN[eIndex].szCode, "%014I64u" , nImei1+1);

				//int DigitLength = 14;
				string strIMEI2 = m_InputSN[eIndex].szCode;
				LONGLONG nIMEI2 = 0;
				char szBuf[64] = {0};
				int Sum = 0;
				int x1 = 0;
				int x2 = 0;
				int temp = 0;
				for (int i = 0; i < 14; i++)
				{
					x1 = atoi(strIMEI2.substr(i, 1).c_str());
					i++;
					temp = atoi(strIMEI2.substr(i, 1).c_str()) * 2;
					x2 = temp < 10 ? temp : temp % 10 + (temp / 10);
					Sum += x1 + x2;
				}
				Sum %= 10;
				Sum = Sum == 0 ? 0 : 10 - Sum;
				nIMEI2 = nIMEI2 * 10 + Sum;
				sprintf_s(szBuf, "%lld", nIMEI2);

				strIMEI2 += szBuf;
				strncpy_s(m_InputSN[BC_IMEI2].szCode, strIMEI2.c_str(), BARCODEMAXLENGTH);
			}
			else
			{
				LogFmtStrA(SPLOGLV_ERROR, "Invalid input. %s is null", m_InputSN[eIndex].szCodeName);
				assert(0);
				return SP_E_SPAT_INVALID_POINTER;
			}
        }
        else
        {
            ZeroMemory(m_InputSN[eIndex].szCode, sizeof(m_InputSN[eIndex].szCode));
            if (strlen(lpCode) > BARCODEMAXLENGTH)
            {
                LogFmtStrA(SPLOGLV_ERROR, "Max SN Length is 64. Input SN length is %d, pls check!", strlen(lpCode));
                return SP_E_CHECK_WRITEX_CHECK_SN_LENGTH_FAIL;
            }
            strncpy_s(m_InputSN[eIndex].szCode, lpCode, strlen(lpCode));
        }
    }
    return SP_OK;
}

SPRESULT CInputCodes::Debug()
{
	SPRESULT res = SP_OK;
	char szBuf[128] = {0};
	res = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN[0], sizeof(m_InputSN));
	if (SP_OK != res)
	{
		LogFmtStrW(SPLOGLV_ERROR, L"Get share memory < %s > failure!", ShareMemory_My_UserInputSN);
		return res;
	}
	srand((unsigned int)time(NULL));
	int nRand = rand()%9;
	if(nRand > 5)
	{
		return SP_E_SPAT_INVALID_POINTER;
	}
	for (INT i=BC_START; i<BC_MAX_NUM; i++)
	{
		if (m_InputSN[i].bEnable)
		{
			if(E_GENCODE_SECTION == m_InputSN[i].eGenCodeType )
			{
				CGenCodes GenCode;
				GenCode.Init(CBarCodeUtility::m_BarCodeInfo[i].nameW, this);
				CHKRESULT_WITH_NOTIFY(GenCode.CompleteCodes(&m_InputSN[i]), "CompleteCodes");
			}
			else if(BC_IMEI2 == i && E_GENCODE_SECTION == m_InputSN[BC_IMEI1].eGenCodeType 
				&& E_GENCODE_FROMIMEI1 == m_InputSN[i].eGenCodeType)
			{
				CGenCodes GenCode;
				GenCode.Init(CBarCodeUtility::m_BarCodeInfo[BC_IMEI1].nameW, this);
				CHKRESULT_WITH_NOTIFY(GenCode.CompleteCodes(&m_InputSN[i]), "CompleteCodes");
			}
			sprintf_s(szBuf, "write %s", CBarCodeUtility::m_BarCodeInfo[i].nameA);
			NOTIFY(szBuf, LEVEL_UI, NOLOWLMT, (SP_OK == res) ? 1 : 0, NOUPPLMT, NULL, -1, "-", m_InputSN[i].szCode);
			string strinfo = m_InputSN[i].szCode;
			NOTIFY(szBuf, LEVEL_REPORT, NOLOWLMT, (SP_OK == res) ? 1 : 0, NOUPPLMT, NULL, -1, "-", ("'"+strinfo).c_str());
		}
	}
	return SP_OK;
}

//XunRui定制编码
void CInputCodes::SetJsConfig_XR(INPUT_CODES_T *m_Input, Value *vArray, Document::AllocatorType &allocator)
{
    Value vObject(kObjectType);
    Value vString(kStringType);
    //Document::AllocatorType& allocator = m_Json.m_document.GetAllocator();
    if (m_bCheckXmode && strlen(m_Input->szPrefix) > 0)
    {
        //CheckX 模式下，输入前缀之后，不用弹出输入框，只要在后面检测时，检测手机中的条码是否符合前缀
        return;
    }

    vString.SetString(m_Input->szCodeName, allocator);
    vObject.AddMember("Name", vString, allocator);

    vObject.AddMember("Enable", m_Input->bEnable, allocator);

    vObject.AddMember("MaxLength", m_Input->nMaxLength, allocator);

    vString.SetString("ManualInput", allocator);	//手动
    vObject.AddMember("GenerateType", vString, allocator);

    vString.SetString(m_Input->szPrefix, allocator);
    vObject.AddMember("Prefix", vString, allocator);

    vString.SetString(m_Input->szValidation, allocator);
    vObject.AddMember("Validation", vString, allocator);

    vString.SetString(m_Input->szInitCode, allocator);
    vObject.AddMember("InitBarCode", vString, allocator);

    vArray->PushBack(vObject, allocator);
}

SPRESULT CInputCodes::GetInputCode_XR(INPUT_CODES_T *pInputCode)
{
	if (pInputCode->bEnable)
	{
		LPCSTR lpCode = m_Json.GetString(pInputCode->szCodeName);
		if (NULL == lpCode)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Invalid input. %s is null", pInputCode->szCodeName);
			assert(0);
			return SP_E_SPAT_INVALID_POINTER;
		}
		else
		{
			ZeroMemory(pInputCode->szCode, sizeof(pInputCode->szCode));
			if (strlen(lpCode) > BARCODEMAXLENGTH)
			{
				LogFmtStrA(SPLOGLV_ERROR, "Max SN Length is 64. Input SN length is %d, pls check!", strlen(lpCode));
				return SP_E_CHECK_WRITEX_CHECK_SN_LENGTH_FAIL;
			}
			strncpy_s(pInputCode->szCode, lpCode, strlen(lpCode));		
		}
	}
	return SP_OK;
}

void CInputCodes::SetKudaXMLConfig(INPUT_CODES_T *pInputCode, int iMaxLenght,std::wstring strCodeName)
{
    std::wstring strKeyName;
    std::wstring strBuf;
    pInputCode->bEnable = true;
    WideCharToMultiByte(CP_ACP, 0, strCodeName.c_str(), -1, pInputCode->szCodeName, 30, 0, 0);
    pInputCode->nMaxLength = (UINT8)iMaxLenght;
    //GenerateType
	pInputCode->eGenCodeType = E_GENCODE_MAN;
}

void CInputCodes::SetKudaInputCodes()
{
	if(m_bCheckInputKuCodes)
	{
		SetKudaXMLConfig(&m_DeviceId,16,L"deviceId");
		SetKudaXMLConfig(&m_KudaSN,m_nKudaSnLength,L"sn");
		SetKudaXMLConfig(&m_OneNetDevId,9,L"oneNetDevId");
		SetKudaXMLConfig(&m_OneNetProductId,6,L"oneNetProductId");
		SetKudaXMLConfig(&m_DevSecret,16,L"devSecret");
	}
}

void CInputCodes::PrintKudaCodesInfo()
{
		PrintKudaCodeInfo(&m_DeviceId);
		PrintKudaCodeInfo(&m_KudaSN);
		PrintKudaCodeInfo(&m_OneNetDevId);
		PrintKudaCodeInfo(&m_OneNetProductId);
		PrintKudaCodeInfo(&m_DevSecret);
}

void CInputCodes::PrintKudaCodeInfo(INPUT_CODES_T *pInputCode)
{
	LogFmtStrA(SPLOGLV_DATA, "Input %s = %s", pInputCode->szCodeName, pInputCode->szCode);
    std::string strItem = "Input ";
    strItem += pInputCode->szCodeName;
    std::string strInfo = pInputCode->szCode;
    //strInfo = "'" + strInfo ;
    NOTIFY(strItem.c_str(), LEVEL_UI, NOLOWLMT, 1, NOUPPLMT, NULL, -1, "-", strInfo.c_str());
    NOTIFY(strItem.c_str(), LEVEL_REPORT, NOLOWLMT, 1, NOUPPLMT, NULL, -1, "-", ("'" + strInfo).c_str());
}
