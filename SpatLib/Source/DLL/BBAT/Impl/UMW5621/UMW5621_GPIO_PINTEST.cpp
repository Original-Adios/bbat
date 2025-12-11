#include "StdAfx.h"
#include "UMW5621_GPIO_PINTEST.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMW5621_GPIO_PINTEST)

//////////////////////////////////////////////////////////////////////////
CUMW5621_GPIO_PINTEST::CUMW5621_GPIO_PINTEST(void)
{
	//SetupDUTRunMode(RM_CALIBRATION_MODE);
}

CUMW5621_GPIO_PINTEST::~CUMW5621_GPIO_PINTEST(void)
{
}

SPRESULT CUMW5621_GPIO_PINTEST::__PollAction(void)
{
	/////////////////////////////////////////////////
	SPRESULT res = SP_OK;

	char  szRecv[512] = {0};
	res = SP_SendATCommand(m_hDUT, "AT+SPWCNTEST=PINTEST", TRUE, szRecv, sizeof(szRecv), NULL, TIMEOUT_2S);
	string strReturn = szRecv;
		
    if (SP_OK != res)
    {
        NOTIFY("PINTEST", LEVEL_ITEM, 1, 0, 1, NULL, -1, "-");
        return SP_E_BBAT_PINTEST_CMD_FAIL;
    }

    CHECK_AT_COMMAND_RESPONSE("AT+SPWCNTEST=PINTEST", szRecv, "SPWCNTEST:OK");

    NOTIFY("PINTEST", LEVEL_ITEM, 1, 1, 1, NULL, -1, "-", (SP_OK == res) ? "OK" : strReturn.c_str());
  
    return res;
}
BOOL CUMW5621_GPIO_PINTEST::LoadXMLConfig(void)
{
	return TRUE;
}