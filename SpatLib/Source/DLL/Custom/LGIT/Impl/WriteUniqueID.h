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
class CWriteUniqueID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteUniqueID)
public:
    CWriteUniqueID(void);
    virtual ~CWriteUniqueID(void);

protected:
    virtual SPRESULT __PollAction(void);
    //virtual BOOL ParseIDFromQRCode(const QRCODE& Code,std::string& strID);

};
