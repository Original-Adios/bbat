#pragma once
#include "FuncBase.h"
#include "Nv.h"
#include "FileConfig.h"
#include "ImpBase.h"
#include "NvHelper.h"

class CNalClc :
    public CFuncBase
{
public:
#define TX 1
#define RX 2

    struct TxConfig
    {
        uint16 usWord;
        double dPower;
    };

    struct RxConfig
    {
        double dRssi;
        double dCellPower;
        uint8 byIndex;
    };

    struct Config
    {
        double dFreq;
        uint32 usChannel;
        uint8 byIndicator;
        uint8 byPath;
		uint8 byBand;
        TxConfig Tx;
        RxConfig Rx;
        Config()
        {
            ZeroMemory(this, sizeof(*this));
        }
    };

    struct Result
    {
        double dLoss;
        BOOL bDone;
		uint32 usAnt;
		Result()
		{
			dLoss = 0.0;
			bDone = FALSE;
			usAnt = 0;
		}
    };

public:
    CNalClc(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CNalClc(void);

    virtual SPRESULT PreInit();

    SPRESULT InitResult();
    SPRESULT ClearResult();
    SPRESULT SetResult();
    void Deserialization(std::vector<uint8>* parrData);

	CNvHelper* m_pNvHelper;

public:
    std::vector<Config> m_arrConfig[MAX_LTE_ANT];
    std::vector<Result> m_arrResult[MAX_LTE_ANT];

private:
    class vector_finder
    {
    public:
        vector_finder(const int nBand, const uint32 uArfcn, const double dFreq, RF_IO_E TRX) :
            m_nBand(nBand),
            m_trx(TRX),
            m_dFreq(dFreq),
            m_uArfcn(uArfcn) {}
        bool operator ()(const std::vector<RF_CABLE_LOSS_POINT_EX>::value_type& value)
        {

            return ((value.nBand == m_nBand && value.dFreq[m_trx] == m_dFreq && value.uArfcn[m_trx] == m_uArfcn)
                ||
                (value.nBand == m_nBand && value.dFreq[m_trx] == 0.0 && value.uArfcn[m_trx] == 0)
                );
        }
    private:
        double m_dFreq;
        int m_nBand;
        uint32 m_uArfcn;
        RF_IO_E m_trx;
    };

    class cmp    
    {    
    public:
        bool operator () (const RF_CABLE_LOSS_POINT& a,const RF_CABLE_LOSS_POINT& b)    
        {    
            return a.dFreq < b.dFreq;    
        }  
    }; 
private:
    SPRESULT InitResult(LTE_ANT_E Ant);
    SPRESULT ClearResult(LTE_ANT_E Ant);
    void Deserialization(uint8* pData, uint16 usLength, LTE_ANT_E Ant);

private:
    RF_CABLE_LOSS_EX* m_plossVal;
};
