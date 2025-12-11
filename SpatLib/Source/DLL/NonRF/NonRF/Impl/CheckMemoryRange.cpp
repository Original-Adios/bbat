#include "StdAfx.h"
#include "CheckMemoryRange.h"
#include <regex>
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckMemoryRange)

///
CCheckMemoryRange::CCheckMemoryRange(void)
{
    m_nDDR_UL = 3;
	m_nDDR_DL = 3;
	m_nEMMC_UL = 32;
	m_nEMMC_DL = 32;
}

CCheckMemoryRange::~CCheckMemoryRange(void)
{

}

BOOL CCheckMemoryRange::LoadXMLConfig(void)
{
	m_nDDR_DL = GetConfigValue(L"Option:DDR_LSL", 3);
	m_nDDR_UL = GetConfigValue(L"Option:DDR_USL", 3);
	m_nEMMC_DL = GetConfigValue(L"Option:EMMC_LSL", 32);
	m_nEMMC_UL = GetConfigValue(L"Option:EMMC_USL", 32);
	if(m_nDDR_DL > m_nDDR_UL)
	{
		return FALSE;
	}
	if(m_nEMMC_DL > m_nEMMC_UL)
	{
		return FALSE;
	}
     return TRUE;
}

SPRESULT CCheckMemoryRange::__PollAction(void)
{
    LPCSTR AT = "AT+EMMCDDRSIZE";
    std::string strRsp = "";
	int DDR = 0,EMMC = 0;
    CHKRESULT_WITH_NOTIFY(SendAT(AT, strRsp), AT);//eg:"15G+2G"
	//Memory½âÎö
	std::string::size_type pos = strRsp.find("+", 0);
	if (std::string::npos == pos)
	{
		NOTIFY("CheckMemoryRange", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,"Get memory range fail");
		return SP_E_FAIL;
	}
	else
	{
		std::string temp = strRsp.substr(0, pos);
		EMMC = atoi(temp.c_str());
		strRsp.erase(0, pos + 1);
		DDR = atoi(strRsp.c_str());
	}
	//·¶Î§±È½Ï
	if(DDR<m_nDDR_DL || DDR>m_nDDR_UL)
	{
		NOTIFY("CheckMemoryRange", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,"DDR:%d,out of range[%d,%d]",DDR,m_nDDR_DL,m_nDDR_UL);
		return SP_E_SPAT_CHECK_MEMORY;
	}
	if(EMMC<m_nEMMC_DL || EMMC>m_nEMMC_UL)
	{
		NOTIFY("CheckMemoryRange", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,"EMMC:%d,out of range[%d,%d]",EMMC,m_nEMMC_DL,m_nEMMC_UL);
		return SP_E_SPAT_CHECK_MEMORY;
	}
	//
    NOTIFY("CheckMemoryRange", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL,"DDR:%d,EMMC:%d",DDR,EMMC);
    return SP_OK;
}

