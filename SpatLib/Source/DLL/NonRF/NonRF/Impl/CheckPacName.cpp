#include "StdAfx.h"
#include "CheckPacName.h"
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckPacName)

///
CCheckPacName::CCheckPacName(void)
{
}

CCheckPacName::~CCheckPacName(void)
{  
}

BOOL CCheckPacName::LoadXMLConfig(void)
{
    std::wstring strName = GetConfigValue(L"Option:PacName", L"");
    m_strPacName = _W2CA(strName.c_str());
    trimA(m_strPacName);
    if (0 == m_strPacName.length())
    {
        SendCommonCallback(L"Please configure the PAC name to check!");
        return FALSE;
    }

    return TRUE;
}

SPRESULT CCheckPacName::__PollAction()
{
    CHAR   szBuf[8192] = {0};
    uint32 u32recvSize =  0;
    LPCSTR    AT = "AT+GETSWPAC?";
    SPRESULT res = SP_SendATCommand(m_hDUT, AT, TRUE, szBuf, sizeof(szBuf), &u32recvSize, TIMEOUT_3S);
    if (SP_OK == res)
    {
        LPCSTR RSP_HEAD = "+GETSWPAC:";
        if (NULL != strstr(szBuf, "+CME ERROR") || NULL == strstr(szBuf, RSP_HEAD))
        {
            NOTIFY("GetPacName", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid AT+GETSWPAC? response!");
            return res;
        }

        CHAR* pChar = strstr(szBuf, RSP_HEAD); // pChar cannot be NULL
        std::string strName = (CHAR* )(pChar + strlen(RSP_HEAD));

        /*
          AT response maybe has "\n" like: debug_gms\n

          --> 24(0x00000018) Bytes
          00000000h: 7E 00 00 00 00 16 00 68 00 41 54 2B 47 45 54 53 : ~......h.AT+GETS
          00000010h: 57 50 41 43 3F 0D 0A 7E                         : WPAC?..~        
          <<- 58(0x0000003a) Bytes
          00000000h: 00 00 00 00 3A 00 9C 00 2B 47 45 54 53 57 50 41 : ....:...+GETSWPA
          00000010h: 43 3A 55 6E 69 73 6F 63 3A 20 73 70 39 38 33 32 : C:Unisoc: sp9832
          00000020h: 65 5F 31 68 31 30 5F 67 6F 66 75 5F 75 73 65 72 : e_1h10_gofu_user
          00000030h: 64 65 62 75 67 5F 67 6D 73 0A                   : debug_gms.      
          <<- 14(0x0000000e) Bytes
          00000000h: 00 00 00 00 0E 00 9C 00 0D 0A 4F 4B 0D 0A       : ..........OK..

          So we replace all \r or \n or \r\nOK\r\n to \0
        */
        replace_all(strName, "\r\nOK\r\n", "");
        replace_all(strName, "\r", "");
        replace_all(strName, "\n", "");
        
        trimA(strName);

        if (m_strPacName == strName)
        {
            NOTIFY("CheckPacName", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "%s", strName.c_str());
        }
        else
        {
            LogFmtStrA(SPLOGLV_INFO, "expected: %s, but read: %s", m_strPacName.c_str(), strName.c_str());
            res = SP_E_SPAT_VERIFY_PAC_NAME;
            NOTIFY("CheckPacName", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Unmatched pac name");
        }
    }

    return res;
}
