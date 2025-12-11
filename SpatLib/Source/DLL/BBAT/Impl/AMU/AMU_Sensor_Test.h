#pragma once
#include "..//ImpBase.h"

///////////////////////////////////////
class CAMU_Sensor_Test :public CImpBase
{
	DECLARE_RUNTIME_CLASS(CAMU_Sensor_Test)

public:
	CAMU_Sensor_Test(void);
	virtual ~CAMU_Sensor_Test(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

	enum BBAT_SENSOR_SPECIAL
	{
		Accelerometer_Sensor = 1,
		Light_Int_Sensor,	//光强度
		Gyroscope_Sensor,
		Magnetic_Sensor = 4,
		Pressure_Sensor,
		Light_Dis_Sensor	//距离光
	};

	enum BBAT_SENSOR_OPERATE
	{
		OPEN = 1,
		CLOSE = 2,
	};

	struct SensorValue
	{
		int8 SensorTest;
		int8 iSensorID;
		// BBAT_SENSOR_SPECIAL
		int8 Operation;
		// BBAT_SENSOR_OPERATE
	};

	struct Data_Acc //1 = 加速度
	{
		uint8 X_Symbol;
		uint8 X_Val[3];
		uint8 Y_Symbol;
		uint8 Y_Val[3];
		uint8 Z_Symbol;
		uint8 Z_Val[3];
	};

	struct Data_Distance//6 = 距离光
	{
		int8 Distance_Level;
		uint8 Noise_Val;
	};

	enum Distance_Level
	{
		NEARBY = 0,
		FARAWAY = 5
	};

	struct Data_LightIntensity //2 = 环境光
	{
		uint8 Light_Sensor_Val_High;
		uint8 Light_Sensor_Val_Low;
	};

		
private:
	BOOL    m_bAccelerometer;//加速度
	BOOL    m_bMagnetic;	//磁力
	BOOL    m_bGyroscope;	//陀螺仪
	BOOL    m_bLight;	//光强度

    double m_AccX = 0.0;
    double m_AccY = 0.0;
    double m_AccZ = 0.0;
    double m_Offset = 0.0;

    int m_NoiseVal = 0;	//距离光，底噪值
    int m_IntVal = 0;		//环境光，光强值


	SPRESULT RunAcc();
	SPRESULT RunMag();
	SPRESULT RunGyr();
	SPRESULT RunLight();  //环境光和距离光同时测
	SPRESULT RunLightDis(int DisLevel);
	SPRESULT RunLightInt(int DisLevel);


	SPRESULT CheckLightDis(Data_Distance DataRecv, int DisLevel);
	SPRESULT CheckLightInt(int IntVal, int DisLevel);
	SPRESULT CheckAcc(Data_Acc DataRecv);
	
};

#define SPECIAL 0x20


