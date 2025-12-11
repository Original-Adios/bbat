#pragma once
#include "SpatBase.h"
#include <stdarg.h>

enum PRESERVE_SPOT_LEVEL
{
    PSPOT_LEVLE_NONE,
    PSPOT_LEVLE_ACTION,
    PSPOT_LEVLE_ITEM
};

//////////////////////////////////////////////////////////////////////////
class CDebuggingTips
{

#define _TITEL_CN_          " 失败场景保留功能 "
#define _CONTENT_CN_     " 勿动，请联系工程师处理。"

#define _TITEL_EN_          "Preserve The Failure Spot"
#define _CONTENT_EN_     "Don't move, please contact the engineer."

public:

    enum Language
    {
        Chinese,
        English
    };
    
public:
    CDebuggingTips( ) {}
    virtual ~CDebuggingTips( void ) {}
    
    void Messag( CDebuggingTips::Language language, LPCSTR lpszFmt, ... )
    {
        char szTips[1024] = {0};
        char szContent[1024] = {0};
        va_list  args;
        va_start( args, lpszFmt );
        _vsnprintf_s( szTips, sizeof( szTips ), lpszFmt, args );
        va_end( args );
        switch ( language )
        {
        case Chinese:
            {
                sprintf_s( szContent, "%s\r\n\r\n%s", _CONTENT_CN_, szTips );
                MessageBoxA( NULL, szContent,  _TITEL_CN_, MB_ICONSTOP | MB_SYSTEMMODAL );
            } break;
        default:
            {
                sprintf_s( szContent, "%s\r\n\r\n%s", _CONTENT_EN_, szTips );
                MessageBoxA( NULL, szContent, _TITEL_EN_, MB_ICONSTOP | MB_SYSTEMMODAL );
            } break;
        }
        
    }
};

