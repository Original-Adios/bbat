#pragma once
#include "ImpBase.h"


/*
AT+SPCHKMIPI?
return
+
SPCHKMIPI:OK
infomation,port
[ usid [0 pid [ mid[0]...
OK
目前返回
6 组 MIPI 器件数据 每组数据 包含 port, usid 、 pid 、 mid,Result(999888)9:FAIL/8:PASS
*/


class CLoadMIPI : public CImpBase
{
    DECLARE_RUNTIME_CLASS( CLoadMIPI )
    
public:
    CLoadMIPI( void );
    virtual ~CLoadMIPI( void );
    
    
protected:

    virtual SPRESULT __PollAction( void );
    
};

typedef struct MIPIINFO
{
    char iPort[5];
    char iUsID[5];
    char iPID[5];
    char iMID[5];
} MIPI;
