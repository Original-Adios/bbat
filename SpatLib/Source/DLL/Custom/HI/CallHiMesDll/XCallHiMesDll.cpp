#include "StdAfx.h"
#include "XCallHiMesDll.h"
#include <iostream>

#define CDBHelperSafeLog    if (NULL != m_pLogUtil)   m_pLogUtil

CXCallHiMesDll::CXCallHiMesDll(ISpLog* pLogUtil)
	:m_pLogUtil(pLogUtil)
{
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, "gcnew MesProt");
	m_pHiMes =gcnew MesProt();
}

CXCallHiMesDll::~CXCallHiMesDll(void)
{
	m_pHiMes = NULL;
}

MES_RESULT CXCallHiMesDll::_HiAoiCheck(LPCSTR lpcSn)
{
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, "AoiCheck, Handle:0X%x,Sn:%s", m_pHiMes, lpcSn);
	System::String ^strRet = m_pHiMes->AoiCheck(gcnew System::String(lpcSn));//返回内容:"002:成功!";或 return "003:错误信息!";
	std::string strErrMsg = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(strRet);
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, strErrMsg.c_str());
	if (NULL == strstr(strErrMsg.c_str(), "002"))
	{
		return MES_FAIL;
	}
	return MES_SUCCESS;
}

MES_RESULT CXCallHiMesDll::_HiAddTextTestInfo(LPCSTR lpcSn, LPCSTR lpcStationName, LPCSTR lpcMac, LPCSTR lpcFilePath, LPCSTR lpcFlag)
{
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, "AddTextTestInfo,Handle:0X%x,Sn:%s,Station:%s,Mac:%s,FilePath:%s,Flag:%s", m_pHiMes, lpcSn, lpcStationName, lpcMac, lpcFilePath, lpcFlag);
	System::String ^strRet = m_pHiMes->AddTextTestInfo(gcnew System::String(lpcSn),
		gcnew System::String(lpcStationName),
		gcnew System::String(lpcMac),
		gcnew System::String(lpcFilePath),
		gcnew System::String(lpcFlag));//返回内容:"002:成功!";或 return "003:"+异常信息

	std::string strErrMsg = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(strRet);
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, strErrMsg.c_str());
	if (NULL == strstr(strErrMsg.c_str(), "002"))
	{
		return MES_FAIL;
	}
	return MES_SUCCESS;
}

MES_RESULT CXCallHiMesDll::_HiAddTextTestInfoCheck(LPCSTR lpcSn, LPCSTR lpcMac)
{
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, "AddTextTestInfoCheck,Handle:0X%x,Sn:%s,Mac:%s", m_pHiMes, lpcSn, lpcMac);
	System::String ^strRet = m_pHiMes->AddTextTestInfoCheck(gcnew System::String(lpcSn), gcnew System::String(lpcMac));//返回内容:"002:成功!";或 return "003:"+异常信息

	std::string strErrMsg = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(strRet);
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, strErrMsg.c_str());
	if (NULL == strstr(strErrMsg.c_str(), "002"))
	{
		return MES_FAIL;
	}
	return MES_SUCCESS;
}

MES_RESULT CXCallHiMesDll::_HiGetChipInfo(LPCSTR lpcSn)
{
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, "GetChipInfo,Handle:0X%x,Sn:%s", m_pHiMes, lpcSn);
	System::String ^strRet = m_pHiMes->GetChipInfo(gcnew System::String(lpcSn));//返回内容:"002:成功!";或 return "003:"+异常信息

	std::string strErrMsg = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(strRet);
	CDBHelperSafeLog->LogFmtStrA(SPLOGLV_INFO, strErrMsg.c_str());

	if (NULL == strstr(strErrMsg.c_str(), "002"))
	{
		return MES_FAIL;
	}
	return MES_SUCCESS;
};