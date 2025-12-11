#pragma once
#include "../ImpBase.h"

typedef enum
{
    SINGAL_SIDED = 0,
    DOUBLE_SIDED = 1
} TEST_MODE;

typedef enum
{
    CC0 = 0, 
    CC1 = 1
} CC_TYPE;

typedef enum
{
    USB2_0 = 0,
    USB3_0 = 1
} USB_TYPE;

//////////////////////////////////////////////////////////////////////////
class CTYPEC_CCTEST : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CTYPEC_CCTEST)
public:
    CTYPEC_CCTEST(void);
    virtual ~CTYPEC_CCTEST(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	/*struct TypeCData
	{
		int8 iCCValue;
		int8 iUSBValue;
	};*/

private:
    TEST_MODE   m_eTESTMODE = SINGAL_SIDED;
    CC_TYPE     m_eCCType;
    USB_TYPE    m_eUsbType;
    uint32 m_nSecondEnumPort = 0;
    uint32 m_nSleepTime = 15000;

	SPRESULT RunTypeC();
	SPRESULT CheckCCType_1st(int8 iCCValue);
    SPRESULT CheckCCType_2nd(int8 iCCValue);
	SPRESULT CheckUsbType(int8 iUSBValue);
    SPRESULT GetPhonePort(void);
    SPRESULT ConnectPhone(void);
    SPRESULT MessageTips(void);
};


