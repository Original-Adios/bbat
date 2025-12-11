#pragma once

typedef int (*pfnGetVersion)(char *ver);
typedef int (*pfnMesStart)(char *inMessage, char *retMessage, int iMesType);
typedef int (*pfnMesComplete)(char *inMessage, int iTestRes, char *retMessage, int iMesType);
typedef int (*pfnGetCodeFromStore)(const char* pMsg, const char* pStore, const unsigned int iLength, char* pRes);


typedef struct _tagMES_DRIVER_T
{
	pfnGetVersion m_pfnMesGetVersion;
	pfnMesStart m_pfnMesStart;
	pfnMesComplete m_pfnMesComplete;
	pfnGetCodeFromStore m_pfnGetCodeFromStore;

    _tagMES_DRIVER_T(void) {
        Reset();
    };

    void Reset(void) {
		m_pfnMesGetVersion = NULL;
		m_pfnMesStart = NULL;
		m_pfnMesComplete = NULL;
		m_pfnGetCodeFromStore = NULL;
    };

    BOOL IsValid(void) {
        return ((NULL != m_pfnMesGetVersion) && (NULL != m_pfnMesStart) && (NULL != m_pfnMesComplete)  && (NULL != m_pfnGetCodeFromStore));
    };

} MES_DRIVER_T;

