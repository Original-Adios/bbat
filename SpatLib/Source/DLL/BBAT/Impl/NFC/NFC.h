#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CNfc : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CNfc)
public:
	CNfc(void);
	virtual ~CNfc(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);


private:
	SPRESULT NfcOpen();
	SPRESULT NfcRead();
	SPRESULT NfcClose();

};