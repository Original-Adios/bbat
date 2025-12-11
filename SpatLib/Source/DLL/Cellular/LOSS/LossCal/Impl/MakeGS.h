#pragma once
#include "ImpBase.h"
#include "ILossFunc.h"

class CMakeGS :
    public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMakeGS)
public:
    CMakeGS(void);
    virtual ~CMakeGS(void);
    virtual SPRESULT   __InitAction(void);
    virtual SPRESULT   __PollAction(void);
    virtual BOOL       LoadXMLConfig(void);
private:
    BOOL LoadGsmConfig();
    BOOL LoadWcdmaConfig();
    BOOL LoadTdConfig();
    BOOL LoadC2kConfig();
    BOOL LoadLteConfig();
    BOOL LoadNrConfig();
    BOOL LoadWcnConfig();
    BOOL LoadDmrConfig();
    SPRESULT RunGsm();
    SPRESULT RunWcdma();
    SPRESULT RunTd();
    SPRESULT RunC2k();
    SPRESULT RunLte();
    SPRESULT RunNr();
    SPRESULT RunWcn();
    SPRESULT RunDmr();
    SPRESULT RunAll();
};
