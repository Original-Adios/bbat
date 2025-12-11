#pragma once
#include "../ImpBase.h"
#pragma comment(lib, "FunctionBox.lib")


#define BOOLTOSPRESULT(statement, strTemp) {										\
									if( TRUE != (statement))    \
                                    {                                   \
                                        PrintErrorMsg(SP_E_BBAT_CMD_FAIL, strTemp, LEVEL_ITEM);\
                                        return SP_E_BBAT_CMD_FAIL;      \
                                    }                                   \
                                    PrintSuccessMsg(SP_OK, strTemp, LEVEL_ITEM);\
								   }
#define ERR_FUNBOX_OBJECT				0x00000001 
#define	ERR_FUNBOX_HANDLE				0x00000002
#define	ERR_FUNBOX_OPENPORT			    0x00000004
#define	ERR_FUNBOX_MONIT0R              0x00000008

//////////////////////////////////////////////////////////////////////////
class CEnterModeG3 : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CEnterModeG3)
public:
    CEnterModeG3(void);
    virtual ~CEnterModeG3(void);

protected:
    virtual SPRESULT __InitAction(void);
    virtual SPRESULT __FinalAction(void);
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    enum HEADSET_TYPE
    {
        EUROPE = 1,
        AMERICA,
        MAX_TYPE_HEADSET
    };

    SPRESULT G3_EnterMode();
    SPRESULT CheckVBAT(void);
    SPRESULT InitCheck(void);
    SPRESULT AFCB_Headmic(HEADSET_TYPE nType);

private:
    HANDLE m_hEnterModeSuccess = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE m_hExitThread = CreateEvent(NULL, TRUE, FALSE, NULL);
    BOOL m_bUsbMode = true;
    BOOL m_bChargeOn = TRUE;
    ChargeVoltage m_eVoltage = Charge_4V;
    int m_nPhonePort = 0;
    int m_nWaitPortTimeout = 0;
    int m_nEnterModeTimeout = 30000;
    int m_nEnterModeInitResult = 0;
    HEADSET_TYPE m_eHeadsetType = EUROPE;
};



#define _SPOK(e)                    (SP_OK == (e))