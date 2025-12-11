#pragma once
#include "CheckCodes.h"
//////////////////////////////////////////////////////////////////////////
typedef enum
{
	E_WIFI_ADDR = 0,
	E_BT_ADDR,
	E_ADDR_END
}E_ADDR_TYPE;

class CCheckCodesEx : public CCheckCodes
{
	DECLARE_RUNTIME_CLASS(CCheckCodesEx)
public:
	CCheckCodesEx(void);
	virtual ~CCheckCodesEx(void);

protected:
	virtual BOOL    LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);

	SPRESULT CheckAll(BOOL bMes);
};
