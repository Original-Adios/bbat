#pragma once
#include "SpatBase.h"
#include "../drv/MesDriver.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include <shlwapi.h>
using namespace std;
#pragma comment(lib,"shlwapi.lib")

#define ShareMemory_KidoMesConfig    L"KidoMesConfig"

#define MAX_CUSTOMIZED_MISCDATA_SIZE (256*1024) // 最大不能超过 256KB,256*1024
#define CUSTOM_MISCDATA_OFFSET (768*1024)
#define HARDCODE_LEN            24
#define QRCODE_LEN              24
#define TOKENCODE_LEN           48
#define MAX_STATION_NUMBER      120
#define CUSTOM_TEST_STATION_POSITION    (CUSTOM_MISCDATA_OFFSET+HARDCODE_LEN+QRCODE_LEN+TOKENCODE_LEN)

typedef struct _tagKIDOMES_T
{
	uint8     type;
	char     ProductName[128];
	char     Batch[128];
	char     Station[128];
	char     LineNumber[128];
	char     Operator[128];
	char     CurrPath[MAX_PATH];
	STRUCT_INITIALIZE(_tagKIDOMES_T);
}KIDOMES_T, *LPKIDOMES_T;
//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

protected:
	SPRESULT GetMesDriverFromShareMemory(void);
	SPRESULT SetMesDriverIntoShareMemory(void);
	SPRESULT Get_Tool_Info(char*pszToolName,char*pszToolVersion);
	SPRESULT MES_GetSN(char*pszSN,int iSize);
	SPRESULT WriteCustMisdata(uint32 u32InBase, const void * pData, int length);

	CMesDriver*m_pMesDrv;
	INPUT_CODES_T m_InputSN[BC_MAX_NUM];
};
