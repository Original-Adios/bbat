#pragma once
#include "ImpBase.h"
/*
    Query signed tri-ad data from KOP:

Step1: Get signed token by script.exe, input param is Json format
       --> 
        {
            "mode": "get_triad_sign",
            "appKey": "e6d0a5c0e90734ab943f1b1eea647de2",
            "appSecret": "a246b855abcb4b7f80875822234d3b07",
            "timestamp": "1559635369",
            "data": {
                "deviceName": "ECD1019010100001"
            }
        }
        <-- 
        {
            "mode": "get_triad_sign",
            "timestamp": "1559635369",
            "data": {
                "sign": "B748B1E8C7D530EEB9E8517F475864F4"
            }
        }
Step2:
    POST:
    http://kop.didichuxing.com/gateway?api=user.get&apiVersion=1.0.0&appKey=Kd_taxi&clientId=123456789&osType=1&osVersion=4.
    5×tamp=1427081285&token=123456789&sign=B748B1E8C7D530EEB9E8517F475864F4

    OK: 
    {
        "code": 200,
        "data": {
        "deviceName": "ECD0001012201032",
        "productKey": "AyhLF06Knmn",
        "deviceSecret": "784D45656A62494D7252625757724D30"
        },
        "msg": "success"
    }
    {
        "code": 200,
        "msg": "success"
    }

    FAIL
    {
        "code": 999203,
        "data": {
        },
        "msg": "; FAQ url: wiki.intra.xiaojukeji.com/x/nGxABQ"
    }

Step3: Query bleTid, bleKey
    --> 
    {
        "mode": "get_bleTid_and_bleKey",
        "appKey": "e6d0a5c0e90734ab943f1b1eea647de2",
        "appSecret": "a246b855abcb4b7f80875822234d3b07",
        "timestamp": "1559635369",
        "data": {
            "deviceName": "ECD1019010100001",
            "deviceSecret": "1234567890123456",
            "productKey": "123"
        }
    }
    <--
    {
        "mode": "get_bleTid_and_bleKey",
        "timestamp": "1559635369",
        "data": {
            "bleTid": "436500001",
            "bleKey": "DA7398D05993AE464349C41880570622"
         }
    }

*/
#define get_triad_sign          "get_triad_sign"
#define get_bleTid_and_bleKey   "get_bleTid_and_bleKey"

//
#pragma pack(push, 4) 

//  三元组
#define LOWPWR_FLIGHT_MODE    (1)
typedef struct _tagTRIAD_SIGN
{
    CHAR  devName[20];              // 设备id 16字节
    CHAR  productKey[12];           // 产品id 10字节
    CHAR  deviceSecret[36];         // 设备秘钥 32字节
    CHAR  bleTid[12];               // 蓝牙id 9字节
    CHAR  bleKey[36];               // 蓝牙秘钥 32字节
    CHAR  hwVer[20];                // 硬件版本号 16字节
    INT32 flightMode;               // flightmode=1 时，ECU置为低功耗飞行模式

    STRUCT_INITIALIZE(_tagTRIAD_SIGN);
} TRIAD_SIGN;
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
class CWriteTriAdData : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteTriAdData)
public:
    CWriteTriAdData(void);
    virtual ~CWriteTriAdData(void);

protected:
    BOOL    LoadXMLConfig(void);
    SPRESULT __PollAction(void);

private:
    SPRESULT SaveTriAdData(CONST TRIAD_SIGN& triAd);
    SPRESULT GetDeviceNameFromBarCode(void);

	BOOL GetCodesFromHTML(TRIAD_SIGN *pTriadSign);
	BOOL ParseCodes(std::string strContent, TRIAD_SIGN *pTriadSign);
	BOOL GetSign();
	BOOL GetBlueTooth(TRIAD_SIGN *pTriadSign);
	std::string GetItemString(CONST std::string strContent, CONST std::string strItem);
   
private:
    std::string m_strAppKey;
    std::string m_strAppSecret;
	std::string m_strSign;
	std::string m_strTimestamp;

    TRIAD_SIGN m_triAdData;
	BOOL m_bTestMode;
};
