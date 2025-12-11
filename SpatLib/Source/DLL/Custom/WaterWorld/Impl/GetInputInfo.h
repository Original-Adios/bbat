#pragma once
#include "ImpBase.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"

//////////////////////////////////////////////////////////////////////////
class CGetInputInfo : public CImpBase
{
    DECLARE_RUNTIME_CLASS( CGetInputInfo )
public:
    CGetInputInfo( void );
    virtual ~CGetInputInfo( void );
    
    INPUT_CODES_T m_InputSN[BC_MAX_NUM];
    
protected:
    virtual BOOL    LoadXMLConfig( void );
    virtual SPRESULT __PollAction( void );
    
    string& replace_all( string& str );
private:
    std::string m_strFilePath;
};