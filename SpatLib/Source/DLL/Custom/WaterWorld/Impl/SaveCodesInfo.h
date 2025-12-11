#pragma once
#include "ImpBase.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"

#include <queue>
using namespace std;
typedef std::queue<string> std_que_str;
//////////////////////////////////////////////////////////////////////////
class CSaveCodesInfo : public CImpBase
{
    DECLARE_RUNTIME_CLASS( CSaveCodesInfo )
public:
    CSaveCodesInfo( void );
    virtual ~CSaveCodesInfo( void );
    
    INPUT_CODES_T m_InputSN[BC_MAX_NUM];
    
protected:
    virtual BOOL    LoadXMLConfig( void );
    virtual SPRESULT __PollAction( void );
    bool Creat_Dir( const char* pszDir );
private:
    std::string m_strLogFolder;
};