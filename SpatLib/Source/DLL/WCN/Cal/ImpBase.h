#pragma once
#include "SpatBase.h"

#define CHKRESULT_WITH_EXIT_EUT(statement)  \
{										                    \
	SPRESULT __sprslt = (statement);                        \
	if (SP_OK != __sprslt)                                  \
{                                                           \
	m_pWlanApi->DUT_EnterEUTMode(FALSE);                     \
	std::string itemName = #statement;					\
	itemName.replace(itemName.find('('), itemName.size() - itemName.find('(') , "()");			\
	NOTIFY(itemName.c_str(), LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);                \
	return __sprslt;                                    \
}                                                       \
}

#define CHKRESULT_WITH_EXIT_TXOFF_EUT(statement)  \
{										                    \
	SPRESULT __sprslt = (statement);                        \
	if (SP_OK != __sprslt)                                  \
{                                                           \
	m_pWlanApi->DUT_TxOn(FALSE);                     \
	m_pWlanApi->DUT_EnterEUTMode(FALSE);                     \
	std::string itemName = #statement;					\
	itemName.replace(itemName.find('('), itemName.size() - itemName.find('(') , "()");			\
	NOTIFY(itemName.c_str(), LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);                \
	return __sprslt;                                    \
}                                                       \
}

#define ShareMemory_My_WCNEFUSE                       L"My_F2491950-8E14-4903-B988-B3C13633529E_WcnEfuse"

///WCN Write EFUSE
typedef struct _tagWCN_WRITE_EFUSE_T
{
	BOOL bWriteAFC;
	int  nDac;
	BOOL bWriteAPC;
	STRUCT_INITIALIZE(_tagWCN_WRITE_EFUSE_T);
} WCN_WRITE_EFUSE_T;

class CImpBase : public CSpatBase
{
public:
	CImpBase(void);
	virtual ~CImpBase(void);

};


////////////////////////////////

// Bug929557
#define NOTIFY_CAL_RESULT(lpszName, nLv, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit) \
	_UiSendMsg(lpszName, nLv|LEVEL_CAL, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit);

#define NOTIFY_CAL_RESULT_WITH_ARG(lpszName, nLv, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit, lpsczCond, ...) \
	_UiSendMsg(lpszName, nLv|LEVEL_CAL, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit, lpsczCond, __VA_ARGS__);