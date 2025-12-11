#pragma once
#include <Windows.h>
#define ShareMemoryItemDataObserver         L"IItemDataObserver"

///
class IItemDataObserver
{
public:
    typedef struct _tagITEMDATA_T
    {
        DWORD           dwSize;
        UINT            nLv;
        CHAR            szActionName[32];
        CHAR            szItemName[32];
        CHAR            szCond[256];
        CHAR            szBand[16];
		CHAR			szUnit[16];
        INT             nChannel;
        double          dLower;
        double          dValue;
        double          dUpper;      
        _tagITEMDATA_T(void) 
        {
            ZeroMemory(this, sizeof(*this));
            dwSize = sizeof(*this);
        };

    } ITEMDATA_T;
public:
    virtual ~IItemDataObserver(void) { };
    virtual void Release(void) = 0;
 
    virtual void Clear(void) = 0;
    virtual void PushData(CONST ITEMDATA_T& item) = 0;
};

