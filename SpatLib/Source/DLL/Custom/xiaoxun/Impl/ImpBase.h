#pragma once
#include "SpatBase.h"
#include "UnisocMesBase.h"

#define Xiaoxun_ShareMemory_OTP_IP      L"Xiaoxun_OTP_IP"
#define Xiaoxun_ShareMemory_OTP_MODE    L"Xiaoxun_OTP_MODE"
#define OTP_WIFI_BYTE_LENGTH	(2*6)
#define OTP_DID_BYTE_LENGTH		(2*8)
#define OTP_KEY_BYTE_LENGTH		(2*16)
#define OTP_UID_BYTE_LENGTH		(2*20)
#define MISC_DATA_FOR_OTP		512

#define MAX_OTP_TRIAD_LENGTH	(OTP_WIFI_BYTE_LENGTH + OTP_DID_BYTE_LENGTH + OTP_KEY_BYTE_LENGTH + 2)//2个“_”分隔符 
#define MAX_OTP_TETRAD_LENGTH	(OTP_WIFI_BYTE_LENGTH + OTP_DID_BYTE_LENGTH + OTP_KEY_BYTE_LENGTH + OTP_UID_BYTE_LENGTH + 3)//3个“_”分隔符    

#define MISCDATA_CUSTOMER_OFFSET_XIAOXUN    (800*1024)


//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

protected:
	SPRESULT WriteCustMisdata(uint32 u32InBase, const void * pData, int length);
};
