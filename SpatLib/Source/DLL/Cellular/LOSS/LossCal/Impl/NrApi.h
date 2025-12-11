#pragma once
#include "CommonApiBase.h"
#include "PhoneCommand.h"
#include "PhoneCommand_NR.h"

class CNrApi : public CCommonApiBase
{
public:
    CNrApi(void);
    virtual ~CNrApi(void);

    virtual SPRESULT RunTxPhoneCommand( BOOL bOn );
    virtual SPRESULT RunRxPhoneCommand();

    virtual double GetUlFreq(int nBand, uint32 uArfcn);

    virtual double GetDlFreq(int nBand, uint32 uArfcn);

private:
    NR_CAL_TRX_REQ_T m_PhoneReq;
    NR_CAL_TRX_RLT_REQ_T m_PhoneRltReq;
    NR_CAL_TRX_RLT_RLT_T m_PhoneRltRlt;
};
