#pragma once
#include "global_def.h"
#include "Global/uni_error_def.h"
#include "IRFDevice.h"

class CCommonApiBase
{
public:
    struct PointData
    {
        int nBand;
        uint32 uArfcn;
        double dFreq;
        BOOL bTx;
        int nParam;
        double dPower;
        RF_ANT_E RfAnt;
        int nAnt;
        int nChannel;
        int PathGroup;
        double dResult;
        double dPowerOffset;
        int nParamOffset;
        void *pTag;
        int nVoltage = 20;

        PointData()
        {
            memset(this, 0, sizeof PointData);
        }
    };

public:
    CCommonApiBase(void);
    virtual ~CCommonApiBase(void);

    SPRESULT Run();

protected:
    virtual SPRESULT RunTxPhoneCommand( BOOL bOn ) = 0;
    virtual SPRESULT RunRxPhoneCommand() = 0;
    virtual double GetUlFreq(int nBand, uint32 uArfcn) = 0;
    virtual double GetDlFreq(int nBand, uint32 uArfcn) = 0;

public:
    PointData *m_pLossData;

    SP_HANDLE m_hDUT;
    IRFDevice *m_pRFTester;

private:
    virtual SPRESULT RunTxRfTester();
    virtual SPRESULT FetchTxRfTester();
    virtual SPRESULT RunRxRfTester();
};
