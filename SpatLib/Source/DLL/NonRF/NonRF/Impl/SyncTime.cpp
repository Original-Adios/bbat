#include "StdAfx.h"
#include "SyncTime.h"
#include <sstream>
#include <iomanip>
#include <locale>
#include <codecvt>

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSyncTime)

///
CSyncTime::CSyncTime(void)
: m_strAT("")
, m_strRsp("")
, m_u32TimeOut(TIMEOUT_3S)
{
}

CSyncTime::~CSyncTime(void)
{
}

// 获取当前时区偏移信息（分钟）
int CSyncTime::getTimeZoneOffset()
{
    TIME_ZONE_INFORMATION tzInfo;
    DWORD result = GetTimeZoneInformation(&tzInfo);

    // 计算偏移量（分钟）
    int bias = tzInfo.Bias;
    if (result == TIME_ZONE_ID_DAYLIGHT) {
        bias += tzInfo.DaylightBias;
    }
    else if (result == TIME_ZONE_ID_STANDARD) {
        bias += tzInfo.StandardBias;
    }

    return -bias; // 返回正数，表示向西的偏移量
}

// 将SYSTEMTIME转换为FILETIME
FILETIME CSyncTime::systemTimeToFileTime(const SYSTEMTIME& st)
{
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    return ft;
}

// 将FILETIME转换为SYSTEMTIME
SYSTEMTIME CSyncTime::fileTimeToSystemTime(const FILETIME& ft)
{
    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);
    return st;
}

// 将本地时间转换为UTC时间
SYSTEMTIME CSyncTime::convertToUTC(const SYSTEMTIME& localTime, int timeZoneOffset)
{
    FILETIME localFileTime = systemTimeToFileTime(localTime);
    ULARGE_INTEGER li;
    li.LowPart = localFileTime.dwLowDateTime;
    li.HighPart = localFileTime.dwHighDateTime;
    li.QuadPart += static_cast<ULONGLONG>(timeZoneOffset) * 60 * 10000000; // 将偏移量从分钟转换为100纳秒单位

    FILETIME utcFileTime;
    utcFileTime.dwLowDateTime = li.LowPart;
    utcFileTime.dwHighDateTime = li.HighPart;
    return fileTimeToSystemTime(utcFileTime);
}

// 格式化时间并返回字符串
std::string CSyncTime::formatSystemTime(const SYSTEMTIME& time, int timeZoneOffset)
{
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << (time.wYear % 100) << "/"
        << std::setw(2) << time.wMonth << "/"
        << std::setw(2) << time.wDay << ","
        << std::setw(2) << time.wHour << ":"
        << std::setw(2) << time.wMinute << ":"
        << std::setw(2) << time.wSecond << "+"
        << std::setw(2) << timeZoneOffset / 60 * 4; // 时区偏移量
    return oss.str();
}

BOOL CSyncTime::LoadXMLConfig(void)
{
    m_strRsp = _W2CA(GetConfigValue(L"Option:Response", L""));
    trimA(m_strRsp);
    m_u32TimeOut = GetConfigValue(L"Param:TimeOut", TIMEOUT_3S);

    return TRUE;
}

SPRESULT CSyncTime::__PollAction(void)
{ 
    SYSTEMTIME localTime;
    GetLocalTime(&localTime); // 获取当前本地时间

    int timeZoneOffset = getTimeZoneOffset(); // 获取当前时区偏移量（分钟）

    SYSTEMTIME utcTime = convertToUTC(localTime, -timeZoneOffset); // 将本地时间转换为UTC时间

    std::string formattedTime = formatSystemTime(utcTime, timeZoneOffset);//UTC时间转换为需要的格式

    formattedTime = "\"" + formattedTime + "\"";
    m_strAT = "AT+CCLK=" + formattedTime;

    CHAR   recvBuf[4096] = {0};
    uint32 recvSize = 0;
    SPRESULT res = SP_SendATCommand(m_hDUT, m_strAT.c_str(), TRUE, recvBuf, sizeof(recvBuf), &recvSize, m_u32TimeOut);
    if (SP_OK != res || NULL != strstr(recvBuf, "+CME ERROR"))
    {
        NOTIFY(m_strAT.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "%s", recvBuf);
        return SP_E_PHONE_AT_EXECUTE_FAIL;
    }

    std::string strRsp = recvBuf;
    replace_all(strRsp, "\r", "");
    replace_all(strRsp, "\n", "");
    trimA(strRsp);
    if (m_strRsp.length() > 0)
    {
        if (NULL != strstr(strRsp.c_str(), m_strRsp.c_str()))
        {
            NOTIFY(m_strAT.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
            return SP_OK;
        }
        else
        {
            NOTIFY(m_strAT.c_str(), LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, strRsp.c_str());
            return SP_E_PHONE_AT_EXECUTE_FAIL;
        }
    }
    else
    {
        NOTIFY(m_strAT.c_str(), LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, strRsp.c_str());
        return SP_OK;
    }
}
