#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
/* 
    Bug 1185676 | Bug 1171719
    AT+GETSWPAC?
    +GETSWPAC:XXXXXXXXX          // 其中XXXX标识SWPACInfo.txt文件中的内容
    OK 

    Example 1:
    AT+GETSWPAC?
    +GETSWPAC: s9863a1h10_Natv-userdebug-gms_SHARKL3_9863A_9.pac
    OK 

    Example 2  
    AT+GETSWPAC?
    +CME ERROR:3
*/
class CCheckPacName : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckPacName)
public:
    CCheckPacName(void);
    virtual ~CCheckPacName(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strPacName;
};
