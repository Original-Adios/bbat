#include "StdAfx.h"
#include "InputCustBarCodes.h"
#include "CLocks.h"
#include "ActionApp.h"
#include <sstream>
#include <time.h>
#include "atlconv.h"

IMPLEMENT_RUNTIME_CLASS(CInputCustBarCodes)
#define STR_GETIMEI2FROMIMEI1 "FromImei1"

LPCSTR GEN_CODE_TYPE_STR[E_GENCODE_MAX] = {"ManualInput", "AutoGenerate", "Section", "SameImei1", "FromImei1","SameSN1"};
    //////////////////////////////////////////////////////////////////////////
CInputCustBarCodes::CInputCustBarCodes(void)
{
	m_bCheckXmode = FALSE;
}

CInputCustBarCodes::~CInputCustBarCodes(void)
{
}

BOOL CInputCustBarCodes::LoadXMLConfig(void)
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

	BOOL bCustmizingInput = (BOOL)GetConfigValue(L"Option:CustomerInput:Enable", FALSE);
	if (bCustmizingInput)
	{
		USES_CONVERSION;
		std::wstring strCustInput = GetConfigValue(L"Option:CustomerInput:LabelList", L"");
		if (!SetCustomerXMLConfig(W2A(strCustInput.c_str())))
		{
			LogFmtStrA(SPLOGLV_ERROR, "SetCustomerXMLConfig Fail , <CustomerInput>.LabelList param error!");
			return FALSE;
		}
	}

    BOOL bOK = FALSE;
    SetShareMemory(ShareMemory_My_UserInputSN, (void *)&bOK, sizeof(bOK), IContainer::System);
    return TRUE;
}

SPRESULT CInputCustBarCodes::InitGenCode(BC_INDEX index)
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

SPRESULT CInputCustBarCodes::__PollAction(void)
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


	for (INT i=BC_CUST_1; i<BC_MAX_CUSTMIZENUM; i++)
	{
		if (!m_InputSN[i].bEnable)
		{
			continue;
		}
		SetJsConfig((BC_INDEX)i, &vArray, allocator);
	}



    m_Json.m_document.AddMember("Barcodes", vArray, allocator);

	//add DUT No. and Port No. to UI
	Value vDut(kStringType);
	char szBuf[64] = "";
	SPAT_INIT_PARAM spat_init_param = GetAdjParam();
	sprintf_s(szBuf, "DUT%d-Port%d", spat_init_param.nTASK_ID, spat_init_param.ca.Com.dwPortNum);
	vDut.SetString(szBuf, allocator);
	m_Json.m_document.AddMember("DutNo", vDut, allocator);
    //

    // Update JSON file: "Bin\sys\InputCustBarCodes.Json"
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
    swprintf_s(szHtmlFile, L"%s\\InputCodesByOne\\InputCodes.html", _T2CW(m_szHtmlPath));

    WINDOW_PROPERTY Wnd;
    ZeroMemory((void *)&Wnd, sizeof(Wnd));
    Wnd.lpszTitle = L"请输入条码";
    Wnd.u32WndStyle = POPUP_WINDOW_CENTER;
    Wnd.cx = 530;
    Wnd.cy = 340 + 50 * 1/*48*nInputNumber*/;
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

    for (INT i=BC_START; i<BC_MAX_CUSTMIZENUM; i++)
    {
		//SN2 the same with SN1
		if (i == BC_SN2 && E_GENCODE_SAMEASSN1 == m_InputSN[BC_SN2].eGenCodeType  && m_InputSN[BC_SN2].bEnable)
        {
            continue;
        }
        CHKRESULT(GetInputCode((BC_INDEX)i));
    }
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
    for (INT i=BC_START; i<BC_MAX_CUSTMIZENUM; i++)
    {
        if (!m_InputSN[i].bEnable)
        {
            continue;
        }

        if (IN_RANGE(BC_START, i, BC_MAX_NUM - 1) && CBarCodeUtility::m_BarCodeInfo[i].lpDutInfoKey != NULL)
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

    // Set share memory
    CHKRESULT(SetShareMemory(ShareMemory_My_UserInputSN, (const void *)&m_InputSN[0], sizeof(INPUT_CODES_T) * BC_MAX_NUM));

    CHKRESULT(SetShareMemory(ShareMemory_My_UserInputCustSN, (const void *)&m_InputSN[BC_CUST_1], sizeof(INPUT_CODES_T) * (BC_MAX_CUSTMIZENUM - BC_CUST_1)));
	
    return SP_OK;
}

void CInputCustBarCodes::GetXMLConfig(INPUT_CODES_T *pInput, std::wstring strCodeName, BOOL bHex)
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

void CInputCustBarCodes::SetJsConfig(BC_INDEX eIndex, Value *vArray, Document::AllocatorType &allocator)
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

SPRESULT CInputCustBarCodes::GetInputCode(BC_INDEX eIndex)
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

BOOL CInputCustBarCodes::SetCustomerXMLConfig(char *pInLabel)
{
	std::string strKey = "";
	std::string strVal = "";

	INT i = BC_CUST_1;
	CONST CHAR sep[] = (";");
	char *pToken = NULL, *pNext_token = NULL;
	char *pLabel = pInLabel;//输入的格式要配对：LABEL名1，长度1；LABEL名2，长度2；...
	pToken = strtok_s(pLabel, sep, &pNext_token);
	while(NULL != pToken)
	{
		strKey = pToken;
		std::string::size_type pos = strKey.find((","));
		if (std::string::npos == pos)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Label Length Isn't Set, Please Check It!");
			return FALSE;
		}

		strVal = strKey.substr(0, pos);
		sprintf_s(m_InputSN[i].szCodeName, sizeof(m_InputSN[i].szCodeName), "%s", strVal.c_str());

		strKey.erase(0, pos+1);
		m_InputSN[i].nMaxLength = (UINT8)_strtoui64(strKey.c_str(), NULL, 10);
		if (0 == m_InputSN[i].nMaxLength)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Label Length Is 0, Please Check It!");
			return FALSE;
		}

		m_InputSN[i].bEnable = TRUE;
		m_InputSN[i].eGenCodeType = E_GENCODE_MAN;

		i++;
		pToken = strtok_s(NULL, sep, &pNext_token);
	}

	return TRUE;
}