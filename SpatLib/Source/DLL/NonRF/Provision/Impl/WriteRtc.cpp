#include "StdAfx.h"
#include "WriteRtc.h"
#include <time.h>


IMPLEMENT_RUNTIME_CLASS(CWriteRtc)
//////////////////////////////////////////////////////////////////////////
CWriteRtc::CWriteRtc(void)
{
}

CWriteRtc::~CWriteRtc(void)
{
}

SPRESULT CWriteRtc::__PollAction(void)
{ 
	CONST CHAR ITEM_NAME[] = "WriteRtc";
	SPRESULT sRet = SP_OK;

	struct tm tWriteTm, tReadTm;
	ZeroMemory((void*)&tWriteTm, sizeof(tWriteTm));
	ZeroMemory((void*)&tReadTm, sizeof(tReadTm));

	time_t t = time(nullptr);
	gmtime_s(&tWriteTm, &t);//UTC TIME, Android,的Date是UTC时间
	LogFmtStrA(SPLOGLV_INFO, "UtcTime:%04d-%02d-%02d %02d:%02d:%02d tm_yday:%02d tm_wday:%01d", 1900 + tWriteTm.tm_year, 1 + tWriteTm.tm_mon, tWriteTm.tm_mday, tWriteTm.tm_hour, tWriteTm.tm_min, tWriteTm.tm_sec, tWriteTm.tm_yday, tWriteTm.tm_wday);

	//直接赋值，会在手机里面转换，年：1900 + p->tm_year, 月：1+ p->tm_mon, p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);//2019/2/28 14:18:31
	sRet = SP_SetRTC(m_hDUT, tWriteTm);
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SetRTC");
		return sRet;
	}

	sRet = SP_GetRTC(m_hDUT, &tReadTm);
	if (SP_OK != sRet)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "GetRTC");
		return sRet;
	}
	//Readback to compare
	//因为tm_yday读出后加1，不能直接整个结构对比
	//从每年的1月1日开始的天数–取值区间为[0,365]，其中0代表1月1日，1代表1月2日，但读出tm_yday读出后加1
	if (tWriteTm.tm_hour == tReadTm.tm_hour && tWriteTm.tm_mon == tReadTm.tm_mon && tWriteTm.tm_year == tReadTm.tm_year)
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "SP_SetRTC:%04d-%02d-%02d %02d:%02d:%02d", 1900 + tWriteTm.tm_year, 1 + tWriteTm.tm_mon, tWriteTm.tm_mday, tWriteTm.tm_hour, tWriteTm.tm_min, tWriteTm.tm_sec);
		return SP_OK;
	}
	else
	{
		NOTIFY(ITEM_NAME, LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Fail of Set And Get RTC Val!");
		return SP_E_INVALID_PARAMETER;
	}
}
