#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCamera : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CCamera)
public:
	CCamera(void);
	virtual ~CCamera(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:

	int m_nCameraId;
	int m_nCameraOpen;
	int m_nCameraClose;
	int m_nCameraRead;
	int m_nCameraOpenDelay = 2500;

	const char* m_pCameraMsg;

	SPRESULT CameraSwitch();
	SPRESULT CameraOpen();
	SPRESULT CameraClose();
	SPRESULT CameraRead();

};