#pragma once
#include "..//ImpBase.h"

///////////////////////////////////////
class CAMU_Sensor_Calibration :public CImpBase
{
	DECLARE_RUNTIME_CLASS(CAMU_Sensor_Calibration)

public:
	CAMU_Sensor_Calibration(void);
	virtual ~CAMU_Sensor_Calibration(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:
	BOOL    m_bAccelerometer;//加速度
	BOOL    m_bMagnetic;	//磁力
	BOOL    m_bGyroscope;	//陀螺仪

	SPRESULT AccCalibration();
	SPRESULT MagCalibration();
	SPRESULT GyrCalibration();

};