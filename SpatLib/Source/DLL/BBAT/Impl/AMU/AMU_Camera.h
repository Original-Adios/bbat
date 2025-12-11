#pragma once
#include "..//ImpBase.h"

///////////////////////////////////////
class CAMU_Camera :public CImpBase
{
	DECLARE_RUNTIME_CLASS(CAMU_Camera)

public:
	CAMU_Camera(void);
	virtual ~CAMU_Camera(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

	enum BBAT_MIPI_CAMERA
	{
		OPEN = 1,
		READ,
		CLOSE,
		SWITCH,
		SEND_CAMERA_DATA
	};

	struct CameraValue
	{
		int8 iOperate; // BBAT_MIPI_CAMERA
	};

private:
	SPRESULT CameraOpen();
	SPRESULT CameraRead();
	SPRESULT SaveFile();
	SPRESULT CameraClose();

    int m_Photo = 0;

};