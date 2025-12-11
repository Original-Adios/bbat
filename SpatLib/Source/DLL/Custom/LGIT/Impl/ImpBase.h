#pragma once
#include "SpatBase.h"
#include "ActionApp.h"

/*
Example Scan: 8928.800.316,355391090035308,01I_T4CHU
    ¡ú 8928.800.316 : Customer P/N
    ¡ú 355391090035308 : IMEI
    ¡ú 01I_T4CHU : FW Version
    FID = hhmm + ]
    ¡ú hhmm : Start Time that is received by MES
    We use FID to combine the IMEI with start time in MES
*/
// CHAR szTime[8] = hhmm\0
#define LGIT_ShareMemory_MES_StartTime      L"LGIT_MES_StartTime"

// Jig port and baudrate 
#define LGIT_ShareMemory_JigPort            L"LGIT_JigPort"
#define LGIT_ShareMemory_JigBaud            L"LGIT_JigBaud"

//LGIT MES Config
#define LGIT_ShareMemory_LgitMesIp          L"LGIT_MES_IP"
#define LGIT_ShareMemory_LgitMesPort        L"LGIT_MES_PORT"
//
extern CActionApp myApp;
//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);
  
    typedef struct
    {
        std::string strQRCode;
        std::string strFormat;
    } QRCODE;

protected:
    string_t GetJsonFile(void);
    BOOL     GetQRCode(QRCODE& Code, LPCTSTR lpszJsonFile = NULL);
    BOOL     ParseIDFromQRCode(const QRCODE& Code,std::string& strID);
    SPRESULT SendAT(LPCSTR lpszAT, std::string& strRsp, uint32 u32RetryCount = 1, uint32 u32Interval = 200, uint32 u32TimeOut = TIMEOUT_3S);
};
