#pragma once
/*
    Implement the functions to read or write code

*/
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////

class CUeHelper 
{
public:
    CUeHelper(CImpBase* pImp);
    virtual ~CUeHelper(void);

public:
    // SN1 & SN2
    SPRESULT WriteSN(SN_TYPE eSN, LPCSTR lpSN);

    // IMEI & WIFI,BT MAC Address
    SPRESULT WriteProductData(CONST PC_PRODUCT_DATA& data);

    // IMEI & MEID (UMS312)
    SPRESULT WriteGEID(CONST PC_GEID_T& GEID);

    // BT & WIFI MAC Address, Android9.x and later support
    SPRESULT WriteBTAddrByAT(LPCSTR lpAddr);
    SPRESULT WriteWIFIAddrByAT(LPCSTR lpAddr);

protected:
    // Send AT Commands
    SPRESULT SendAT(
        LPCSTR lpszAT, std::string& strRsp, 
        uint32 u32RetryCount = 1, uint32 u32Interval = 200, uint32 u32TimeOut = TIMEOUT_3S
        );

private:
    CImpBase* m_pImp;
    SP_HANDLE m_hDUT;
};
