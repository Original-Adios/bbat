#pragma once
#include <list>
#include "global_def.h"
#include "SharedDefine.h"
#include "ScanConfig.h"

//////////////////////////////////////////////////////////////////////////
#define MAX_CODES_ALLOC_NUM (32)

typedef struct _tagPRE_ALLOC_T
{
	INT nTaskID;
	unsigned __int64 nSectionID;
}PRE_ALLOC_T;

typedef struct _tagGEN_SECTION_CODES_T
{
	//Code Length
	UINT8 nMaxLength;
	//Code Prefix
	CHAR szPrefix[BARCODEMAXLENGTH + 1];
    //Code Start index
	unsigned __int64 nSectionStart;
	//Code End index
	unsigned __int64 nSectionEnd;
	//Code Current index
	unsigned __int64 nSectionCurr;
	//Complete Codes list
	std::list<unsigned __int64> SectionComplete;
	//PreAlloc Codes list
	std::list<PRE_ALLOC_T> SectionPreAlloc;
	_tagGEN_SECTION_CODES_T()
	{
		nMaxLength = 0;
		ZeroMemory(szPrefix, sizeof(szPrefix));
		nSectionStart = 0;
		nSectionEnd = 0;
		nSectionCurr = 0;
		SectionComplete.clear();
		SectionPreAlloc.clear();
	};
} GEN_SECTION_CODES_T;

class CSpatBase;

class CGenCodes
{
public:
	CGenCodes(void);
	virtual ~CGenCodes(void);
	/*Init Function
	Description: Init GenCodes from file or init data
	Params:
	pAppName[In]:  Code Type name, SN1, IMEI1 etc.
	pParent[In]: Action Point
	pData[In]: Init Data Point
	*/
	SPRESULT Init(LPCTSTR pAppName, CSpatBase* pParent, GEN_SECTION_CODES_T *pData = NULL);
	
	/*PreAlloc Function
	Description: PreAlloc codes from Gencodes
	Params:
	pCodes[Out]:Point of Generate Codes
	nNum[In]: Count of Generate Codes 
	bHex[In]: Codes format Hex or Dec
	*/
	SPRESULT PreAllocCodes(INPUT_CODES_T* pCodes, int nNum = 1, BOOL bHex = FALSE);
	/*Complete Codes Function
	Description: Complete PreAlloc codes after write code success
	Params:
	pCodes[In]: Point of Codes
	nNum[In]: count of Codes 
	*/
	SPRESULT CompleteCodes(INPUT_CODES_T* pCodes, int nNum = 1);
	/*Clear PreAlloc Codes Function
	Description: Clear PreAlloc codes after write code failed for current Task
	Params:
	NULL
	*/
	void     ClearPreAllocForTask();
private:

	void LoadFile(GEN_SECTION_CODES_T &stCodes);
	CSpatBase *m_Imp;
	TCHAR m_szAPPName[128];
	CScanConfig m_FileConfig;
};


