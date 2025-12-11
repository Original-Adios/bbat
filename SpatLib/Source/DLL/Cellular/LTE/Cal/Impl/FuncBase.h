#pragma once
#include "IFunc.h"
#include "SpatBase.h"
#include "FuncCenter.h"
#include "GlobalDefine.h"
#include "UniDefine.h"
#include <vector>
#include "ExtraLogFile.h"
#include "CustomizedBase.h"

#define CHKRESULT_WITH_NOTIFY_LTE_ITEM(statement, Itemname)  \
{										                    \
	SPRESULT __sprslt = (statement);                        \
	if (SP_OK != __sprslt)                                  \
	{                                                       \
	    UiSendMsg(Itemname, LEVEL_ITEM, 1, 0, 1);                \
	    return __sprslt;                                    \
	}                                                       \
}	

#define CHKRESULT_WITH_NOTIFY_LTE(statement)  \
{										                    \
	SPRESULT __sprslt = (statement);                        \
	if (SP_OK != __sprslt)                                  \
	{                                                       \
		UiSendMsg(__FUNCTION__, LEVEL_ITEM, 1, 0, 1);                \
		return __sprslt;                                    \
	}                                                       \
}	


class CFuncBase :
	public IFunc
{
public:
	CFuncBase(LPCWSTR lpName, CFuncCenter* pFuncCenter);
	virtual ~CFuncBase(void);
        
    virtual SPRESULT PreInit();
    SPRESULT SetRfSwitch(int nBand, RF_ANT_E TxAnt, RF_ANT_E RxAnt);
    SPRESULT ResetRfSwitch();

protected:
	void SetCurrContainer(CFuncCenter* pFuncCenter);
    SPRESULT SetShareMemory(LPCWSTR lpszName, const void* lpValue, UINT32 nSize, UINT32 nOption = IContainer::Normal);
    SPRESULT GetShareMemory(LPCWSTR lpszName, void* lpValue, UINT32 nSize, UINT32* pOption = NULL);

    BOOL LogRawStrA(SPLOG_LEVEL nLv, LPCSTR  lpszString);
    BOOL LogRawStrW(SPLOG_LEVEL nLv, LPCWSTR lpszString);
    BOOL LogFmtStrA(SPLOG_LEVEL nLv, LPCSTR  lpszFmt, ...);
    BOOL LogFmtStrW(SPLOG_LEVEL nLv, LPCWSTR lpszFmt, ...);
    BOOL LogBufData(SPLOG_LEVEL nLv, const void* lpData, UINT32 nDataSize, UINT nFlag = LOG_WRITE, const UINT32* lpnExpSize = NULL);

    BOOL UiSendMsg(
        const char* lpszName, 
        uint32 nLv,
        double dLow, double dVal, double dUpp, 
        const char* lpszBand, int32 nChannel,
        const char* lpszUnit, 
        const char* lpsczCond, ...);
    BOOL UiSendMsg(
        const char* lpszName, 
        uint32 nLv,
        double dLow = 0.0, double dVal = 0.0, double dUpp = 0.0, 
        const char* lpszBand = NULL, int32 nChannel = -1,
        const char* lpszUnit = NULL);
    BOOL UiSendMsg(const char* lpsznvPath, int32 data); 

    void  AddFunc(LPCWSTR lpName, void* pFunc);
    void* GetFunc(LPCWSTR lpName);

    template<typename T>
    T& VectorIndex(std::vector<T>* arr, int nIndex);

    void     SetRepairBand(LPCSTR pBand);

	int      GetConfigValue(LPCWSTR lpPath, int     defValue);
	double   GetConfigValue(LPCWSTR lpPath, double  defValue);
	LPCWSTR  GetConfigValue(LPCWSTR lpPath, LPCWSTR defString);

protected:
    LPCWSTR m_lpFuncName;
    CFuncCenter* m_pFuncCenter;
    CSpatBase* m_pSpatBase;

private:
	CFuncBase(void);
    CCustomized m_Customize;
};

// Bug929557
#define NOTIFY_CAL_RESULT(lpszName, nLv, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit) \
    UiSendMsg(lpszName, nLv|LEVEL_CAL, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit);

#define NOTIFY_CAL_RESULT_WITH_ARG(lpszName, nLv, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit, lpsczCond, ...) \
    UiSendMsg(lpszName, nLv|LEVEL_CAL, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit, lpsczCond, __VA_ARGS__);