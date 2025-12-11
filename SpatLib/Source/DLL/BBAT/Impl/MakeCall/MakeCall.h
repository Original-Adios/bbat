#pragma once
#include "../ImpBase.h"

#define MAX_CSQ_VALUE 31    //2G和3G通话的信号强度一般不会超过31，使用AT+CSQ命令，其他使用AT+CESQ指令

//////////////////////////////////////////////////////////////////////////
class CMakeCall : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CMakeCall)
public:
	CMakeCall(void);
	virtual ~CMakeCall(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:
    int m_nRssiDown = 0;
    int m_nRssiUp = 0;
    int m_nGsmOnly = 1;
    int m_nSleep = 0;
    int m_nSleep1 = 0;
    int m_nSleep2 = 0;
    BOOL m_bIOT = FALSE;
	std::string m_strPhoneNumber = "";
private:

	SPRESULT OpenSimCard();
    SPRESULT GsmOnly();
	SPRESULT OpenProtocolStack();
	SPRESULT MakeCall();
	SPRESULT ReadStatus();
	SPRESULT ReadRssi(int &nRssi);
	SPRESULT ReadLteRssi();
	SPRESULT FinishCall();
	

};
