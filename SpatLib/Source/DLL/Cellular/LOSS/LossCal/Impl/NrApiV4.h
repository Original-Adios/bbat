#pragma once
#include "NrApi.h"

class CNrApiV4 : public CNrApi
{
public:
    CNrApiV4(void);
    virtual ~CNrApiV4(void);

    virtual SPRESULT RunTxPhoneCommand(BOOL bOn);
    virtual SPRESULT RunRxPhoneCommand();


private:
    NR_CAL_TRX_REQ_V4_T m_PhoneReq;
    NR_CAL_TRX_RLT_REQ_T m_PhoneRltReq;
    NR_CAL_TRX_RLT_RLT_V4_T m_PhoneRltRlt;
};

