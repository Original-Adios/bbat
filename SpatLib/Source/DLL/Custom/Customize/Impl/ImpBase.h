#pragma once
#include "UnisocMesBase.h"

//////////////////////////////////////////////////////////////////////////
class CImpBase : public CUnisocMesBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

    SPRESULT SendATCommand(LPCSTR lpszCmd, uint32 u32TimeOut = TIMEOUT_3S);

protected:
    std::string m_strRevBuf;
};
