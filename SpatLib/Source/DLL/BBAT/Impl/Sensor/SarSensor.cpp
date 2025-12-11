#include "StdAfx.h"
#include "SarSensor.h"
#include <chrono>
#include <future>
#include <thread>

//
IMPLEMENT_RUNTIME_CLASS(CSarSensor)

CSarSensor::CSarSensor(void)
{
}
CSarSensor::~CSarSensor(void)
{
}

SPRESULT CSarSensor::__PollAction(void)
{
    CHKRESULT(RunSarTest());

    return SP_OK;
}

BOOL CSarSensor::LoadXMLConfig(void)
{
    m_UpDelayTime = GetConfigValue(L"Option:VoltageUpDelayTime", 3000);
    m_DownDelayTime = GetConfigValue(L"Option:VoltageDownDelayTime", 1000);

    return TRUE;
}

// bug #2908209 PAD-PCBA产线生产-SARSENSOR项增入SIMBA-BBAT-Test-FAIL问题，请帮忙调试支持。
SPRESULT CSarSensor::DelayTask()
{
    // 进入sarTest后延时3s
    Sleep(m_UpDelayTime);

    // 控制5V上电
    m_pObjFunBox->FB_EXT1_Power_Ctrl(true);

    // 延时1s
    Sleep(m_DownDelayTime);

    //控制5V下电
    m_pObjFunBox->FB_EXT1_Power_Ctrl(false);

    return SP_OK;
}

SPRESULT CSarSensor::RunSarTest()
{
	int nSarLevel = 0;

    auto future = std::async(std::launch::async, &CSarSensor::DelayTask, this);

    CHKRESULT(PrintErrorMsg(SP_BBAT_SarTest(m_hDUT, &nSarLevel), "Sar Sensor Open Fail", LEVEL_ITEM));

    future.wait();

    PrintSuccessMsg(SP_OK, "Sar Sensor Test Successful!", LEVEL_ITEM);

	return SP_OK;
}