#pragma once
#include "global_def.h"

//
class CSpatBase;
//////////////////////////////////////////////////////////////////////////
class CScanConfig
{
public:
    CScanConfig(void);
    virtual ~CScanConfig(void);

    ///  R/W scan parameter file APIs
    BOOL    LoadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD dwNbrOfCharacters);
    BOOL    SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString);
    INT     LoadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault);
    BOOL    SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nValue);
    double  LoadValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, double dDefault);
    BOOL    SaveValue(LPCTSTR lpAppName, LPCTSTR lpKeyName, double dValue);

    /// 
    void    InitConfigFile(CSpatBase* pParent, BOOL bNoTaskID = FALSE);
    LPCTSTR GetFilePath(void)const { return m_strFilePath.c_str(); };
    void    DeleteConfigFile();
    BOOL    isDeleteConfigFile();

private:
    string_t m_strFilePath;
};

