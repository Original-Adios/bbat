#pragma once
#include "SpatBase.h"
#include "Sql_Imp.h"
#include "../../../../../../Common/Json/JsonUtil.h"

#include <queue>
using namespace std;
typedef std::queue<string> std_que_str;

#define MISCDATA_BASE_OFFSET (768*1024)
#define MISCDATA_RESET_FLAG_OFFSET (MISCDATA_BASE_OFFSET + 20)
#define MISDATA_MAX_DETAILMODEL_LENGTH 20
#define MISDATA_RESET_FLAG_LENGTH 1

#define ShareMemory_Path_Input_Txt  L"WW_Path_Input_Txt"
//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

	SPRESULT GetMesDriverFromShareMemory(void);
	SPRESULT SetMesDriverIntoShareMemory(void);
	SPRESULT SendATCommand(LPCSTR lpszCmd, uint32 u32TimeOut = TIMEOUT_3S);


	BOOL ReadInputFile(const char*pszPath,std_que_str&input_info);
	BOOL WriteInputFile(const char*pszPath,std_que_str&input_info);

protected:

	std::string m_strRevBuf;
	CSql_Imp *m_pSql_Imp;
};
