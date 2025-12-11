#pragma once
#include "ImpBase.h"
#include "ILossFunc.h"

class CCalcLS :
    public CImpBase
{
    DECLARE_RUNTIME_CLASS( CCalcLS )
public:
    CCalcLS( void );
    virtual ~CCalcLS( void );
    virtual SPRESULT   __InitAction( void );
    virtual SPRESULT   __PollAction( void );
    virtual BOOL       LoadXMLConfig( void );
    
private:
    SPRESULT CheckResult( SPRESULT spRlt );
    void AddPreLoss(RF_CABLE_LOSS_UNIT_EX& loss, double dPreLoss, BOOL bCheckZero = TRUE);
  
    SPRESULT RunGsm();
    SPRESULT RunWcdma();
    SPRESULT RunTd();
    SPRESULT RunC2k();
    SPRESULT RunLte();
    SPRESULT RunNr();
    SPRESULT RunWcn();
    SPRESULT RunDmr();
};
