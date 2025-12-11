#pragma once
#include "ImpBase.h"
/*
  Function: Write a unique ID(AT*IMEISET) to UE

  AT+SPIMEI
  Description: Set and read IMEI
  Set command:
    AT+ SPIMEI=<card_id>,<imei>
    Return £ºOK
  Read command:
    AT+ SPIMEI?
    Return £ºxxxxxxxxxxxxxxx
    OK
  Parameter:
    <card -id>: 0 sim1; 1 sim2
    <imei>: xxxxxxxxxxxxxxx(15  IMEI  characters)

*/
//////////////////////////////////////////////////////////////////////////
class CReadUniqueID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CReadUniqueID)
public:
    CReadUniqueID(void);
    virtual ~CReadUniqueID(void);

protected:
    virtual SPRESULT __PollAction(void);

};
