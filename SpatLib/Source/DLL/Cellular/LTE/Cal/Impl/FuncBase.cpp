#include "StdAfx.h"
#include "FuncBase.h"

CFuncBase::CFuncBase(void)
{
	m_lpFuncName = NULL;
	m_pFuncCenter = NULL;
	m_pSpatBase = NULL;
}

CFuncBase::CFuncBase( LPCWSTR lpName, CFuncCenter* pFuncCenter )
{
    m_pFuncCenter = pFuncCenter;
    m_pSpatBase = (CSpatBase*)pFuncCenter->GetFunc(SPATBASE);
    m_lpFuncName = lpName;

    LPCWSTR lpProjectName = m_pSpatBase->GetConfigValue(L"Header:ProjectInfo:ProjectName", L"");
    m_Customize.InitRfSwitch(m_pSpatBase->GetAdjParam().nTASK_ID, SP_LTE, m_pSpatBase, lpProjectName);


    AddFunc(lpName, this);
}

CFuncBase::~CFuncBase(void)
{

}

SPRESULT CFuncBase::PreInit()
{
    return SP_OK;
}

SPRESULT CFuncBase::SetShareMemory( LPCWSTR lpszName, const void* lpValue, UINT32 nSize, UINT32 nOption/*= IContainer::Normal*/ )
{
    return m_pSpatBase->SetShareMemory(lpszName, lpValue, nSize, nOption);
}

SPRESULT CFuncBase::GetShareMemory( LPCWSTR lpszName, void* lpValue, UINT32 nSize, UINT32* pOption/*= NULL*/ )
{
    return m_pSpatBase->GetShareMemory(lpszName, lpValue, nSize, pOption);
}

BOOL CFuncBase::LogRawStrA( SPLOG_LEVEL nLv, LPCSTR lpszString )
{
    return m_pSpatBase->LogRawStrA(nLv, lpszString);
}

BOOL CFuncBase::LogRawStrW( SPLOG_LEVEL nLv, LPCWSTR lpszString )
{
    return m_pSpatBase->LogRawStrW(nLv, lpszString);
}

BOOL CFuncBase::LogFmtStrA( SPLOG_LEVEL nLv, LPCSTR lpszFmt, ... )
{
    va_list  args = NULL;  
    va_start(args, lpszFmt);                          

    size_t nLength = _vscprintf(lpszFmt, args) + 1;
    char* pBuff = (char*)malloc(nLength);

    if (_vsnprintf_s(pBuff, nLength, _TRUNCATE, lpszFmt, args) <= 0) 
    {
        va_end(args); 
        free(pBuff);
        pBuff = NULL;
        return FALSE;
    }
    else
    {
        va_end(args);
        SPRESULT res = m_pSpatBase->LogRawStrA(nLv, pBuff);
        free(pBuff);
        pBuff = NULL;
        return res;
    }
}

BOOL CFuncBase::LogFmtStrW( SPLOG_LEVEL nLv, LPCWSTR lpszFmt, ... )
{
    va_list  args = NULL;  
    va_start(args, lpszFmt);                          

    size_t nLength = _vscwprintf(lpszFmt, args) + 1;   
    wchar_t* pBuff = (wchar_t*)malloc(nLength * 2);
    if (_vsnwprintf_s(pBuff, nLength, _TRUNCATE, lpszFmt, args) <= 0) 
    {
        va_end(args); 
        free(pBuff);
        pBuff = NULL;
        return FALSE;
    }
    else
    {
        va_end(args);
        SPRESULT res = m_pSpatBase->LogRawStrW(nLv, pBuff);
        free(pBuff);
        pBuff = NULL;
        return res;
    }
}

BOOL CFuncBase::LogBufData( SPLOG_LEVEL nLv, const void* lpData, UINT32 nDataSize, UINT32 nFlag /*= LOG_WRITE*/, const UINT32* lpnExpSize /*= NULL*/ )
{
    return m_pSpatBase->LogBufData(nLv, lpData, nDataSize, nFlag, lpnExpSize);
}

BOOL CFuncBase::UiSendMsg( const char* lpszName, 
                           uint32 nLv,
                           double dLow, double dVal, double dUpp, 
                           const char* lpszBand, int32 nChannel,
                           const char* lpszUnit, 
                           const char* lpsczCond, ...)
{
    if (NULL != lpsczCond)
    {
        va_list  args = NULL;  
        va_start(args, lpsczCond);    

        size_t nLength = _vscprintf(lpsczCond, args) + 1;
        char* pBuff = (char*)malloc(nLength);

        if (_vsnprintf_s(pBuff, nLength, _TRUNCATE, lpsczCond, args) <= 0) 
        {
            va_end(args); 
            free(pBuff);
            pBuff = NULL;
            return FALSE;
        }
        else
        {
            va_end(args);
        }
        SPRESULT res = m_pSpatBase->_UiSendMsg(lpszName,
            nLv,
            dLow, dVal, dUpp,
            lpszBand, nChannel,
            lpszUnit,
            pBuff);
        free(pBuff);
        pBuff = NULL;
        return res;
    }
    else
    {
        return m_pSpatBase->_UiSendMsg( lpszName, 
            nLv,
            dLow,  dVal,  dUpp, 
            lpszBand,  nChannel,
            lpszUnit, 
            lpsczCond);
    }
}

BOOL CFuncBase::UiSendMsg( const char* lpszName, 
                           uint32 nLv ,
                           double dLow, double dVal, double dUpp, 
                           const char* lpszBand, int32 nChannel,
                           const char* lpszUnit)
{
    return m_pSpatBase->_UiSendMsg(
        lpszName, 
        nLv,
        dLow, dVal, dUpp, 
        lpszBand, nChannel,
        lpszUnit);
}


BOOL CFuncBase::UiSendMsg(const char* lpsznvPath, int32 data)
{
    return m_pSpatBase->_UiSendMsg(lpsznvPath, data);
}

void CFuncBase::AddFunc( LPCWSTR lpName, void* pFunc )
{
    m_pFuncCenter->AddFunc(lpName, pFunc);
}

void* CFuncBase::GetFunc( LPCWSTR lpName )
{
    return m_pFuncCenter->GetFunc(lpName);
}

template<typename T>
T& CFuncBase::VectorIndex( std::vector<T>* arr, int nIndex )
{
    if (arr->size() <= nIndex)
    {
        arr->resize(nIndex + 100);
    }

    return (*arr)[nIndex];
}

void CFuncBase::SetRepairBand( LPCSTR pBand )
{
    m_pSpatBase->SetRepairBand(pBand);
}

int CFuncBase::GetConfigValue( LPCWSTR lpPath, int defValue )
{
	return m_pSpatBase->GetConfigValue(lpPath, defValue);
}

double CFuncBase::GetConfigValue( LPCWSTR lpPath, double defValue )
{
	return m_pSpatBase->GetConfigValue(lpPath, defValue);
}

LPCWSTR CFuncBase::GetConfigValue( LPCWSTR lpPath, LPCWSTR defString )
{
	return m_pSpatBase->GetConfigValue(lpPath, defString);
}
void CFuncBase::SetCurrContainer( CFuncCenter* pFuncCenter )
{
	m_pFuncCenter = pFuncCenter;
    m_pSpatBase = (CSpatBase*)pFuncCenter->GetFunc(SPATBASE);
}


SPRESULT CFuncBase::SetRfSwitch(int nBand, RF_ANT_E TxAnt, RF_ANT_E RxAnt)
{
    return m_Customize.SetRfSwitch(nBand, TxAnt, RxAnt);
}

SPRESULT CFuncBase::ResetRfSwitch()
{
    return m_Customize.ResetRfSwitch();
}