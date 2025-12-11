#pragma once
#include "NrApi.h"

class CNrApiV5 : public CNrApi
{
public:
    CNrApiV5(void);
    virtual ~CNrApiV5(void);

    virtual SPRESULT RunTxPhoneCommand(BOOL bOn);
    virtual SPRESULT RunRxPhoneCommand();


private:
    NR_CAL_TRX_REQ_V5_T m_PhoneReq;
    NR_CAL_TRX_RLT_REQ_T m_PhoneRltReq;
    NR_CAL_TRX_RLT_RLT_V4_T m_PhoneRltRlt;
};

