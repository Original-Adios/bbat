#pragma once
#include "ImpBase.h"
/*
  Function: Write SN(AT*FID) to UE
  AT*FID
  -	Write
  AT*FID=MS180813000001
  OK

  -	Read
  AT*FID?
  *FID:MS180813000001
  OK


  FID = hhmm + IMEI
  ¡ú hhmm : Start Time that is received by MES
*/
//////////////////////////////////////////////////////////////////////////
class CWriteFID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteFID)
public:
    CWriteFID(void);
    virtual ~CWriteFID(void);

protected:
    virtual SPRESULT __PollAction(void);
};
