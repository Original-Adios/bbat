#include "StdAfx.h"
#include "CustomMiscData.h"
#include "ExtraLogFile.h"
#include "ModeSwitch.h"

//
IMPLEMENT_RUNTIME_CLASS(CCustomerMiscData)

//////////////////////////////////////////////////////////////////////////
CCustomerMiscData::CCustomerMiscData(void)
: m_u32Base(0)
, m_u32Size(0)
{
}

CCustomerMiscData::~CCustomerMiscData(void)
{
}

BOOL CCustomerMiscData::LoadXMLConfig(void)
{
	m_u32Base = (uint32)GetConfigValue(L"Option:Offset", MISCDATA_CUSTOMER_OFFSET);
	m_u32Size = (uint32)GetConfigValue(L"Option:Size", 256*1024);
	if (m_u32Base + m_u32Size > 1024 * 1024)
	{
		SendCommonCallback(L"Offset + Size should be less than 1024*1024 bytes!");
		return FALSE;
	}

	return TRUE;
}

SPRESULT CCustomerMiscData::__PollAction(void)
{ 
	enum
	{
		ALL_0 = 0,
		ALL_1,
		ALL_INCREASING,
		CUSTOM
	} eType;
	std::wstring strDataType = GetConfigValue(L"Option:Data", L"Increasing");
	if (strDataType == L"All_1")
	{
		eType = ALL_1;
	}
	else if (strDataType == L"All_0")
	{
		eType = ALL_0;
	}
	else if(strDataType == L"Increasing")
	{
		eType = ALL_INCREASING;
	}
	else
	{
		eType = CUSTOM;
	}

	std::wstring strCustom = GetConfigValue(L"Option:Custom", L"on");
	if(eType == CUSTOM)
	{
		if(strCustom.empty())
		{
			NOTIFY("CompareCustomerMiscData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Please input custom string!");
			return SP_E_SPAT_USER_NOT_INPUT;
		}

		uint32 nLen = strCustom.length();
		LogFmtStrA(SPLOGLV_INFO, "Set Size=%d CusString %s length=%d", m_u32Size, _W2CA(strCustom.c_str()), nLen);
		if(nLen != m_u32Size)
		{
			NOTIFY("CompareCustomerMiscData", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "Please input correct size!");
			return SP_E_SPAT_USER_NOT_INPUT;
		}
	}
	
    SPRESULT res = SP_OK;
	std::vector<uint8> vecSend(m_u32Size,0);
	std::vector<uint8> vecRecv(m_u32Size,0);
    for (uint32 i=0; i<m_u32Size; i++)
    {
		switch(eType)
		{
		case ALL_1:
			vecSend[i] = 1;
			break;
		case ALL_0:
			vecSend[i] = 0;
			break;
		case ALL_INCREASING:
			vecSend[i] = i%255;
			break;
		case CUSTOM:
			vecSend[i] = strCustom.at(i);
			break;
		default:
        // 初始化数据, 测试包含7D,7E转义字符
			vecSend[i] = i%255;
			break;
		}
    }

/*	if(eType == CUSTOM)//Bug 1771818,vecSend崩溃，m_u32Size循环增加
	{
		vecSend[m_u32Size] = '\0';
		m_u32Size ++;
	}
*/
	CExtraLog file;
    file.Open(this, _T("miscdata\\Send.bin"));
    file.Write(&vecSend[0], m_u32Size);
    file.Close();

    CHKRESULT_WITH_NOTIFY(SP_SaveMiscData(m_hDUT, m_u32Base, &vecSend[0], m_u32Size, TIMEOUT_3S), "SaveCustomerMiscData");

    NOTIFY("SaveCustomerMiscData", LEVEL_UI, 1, 1, 1);

    // Restart phone to verify whether the data has been saved correctly or not.
    CHKRESULT_WITH_NOTIFY(m_pModeSwitch->Reboot(RM_CALIBRATION_MODE, NULL), "Reboot");


    CHKRESULT_WITH_NOTIFY(SP_LoadMiscData(m_hDUT, m_u32Base, &vecRecv[0], m_u32Size, TIMEOUT_3S), "LoadCustomerMiscData");

    NOTIFY("LoadCustomerMiscData", LEVEL_UI, 1, 1, 1);
    file.Open(this, _T("miscdata\\Recv.bin"));
    file.Write(&vecRecv[0], m_u32Size);
    file.Close();
    
	if (0 != memcmp(&vecRecv[0], &vecSend[0], m_u32Size))
	{
		NOTIFY("CompareCustomerMiscData", LEVEL_UI, 1, 0, 1);
		return SP_E_SPAT_INVALID_DATA;
	}

    return res;
}
