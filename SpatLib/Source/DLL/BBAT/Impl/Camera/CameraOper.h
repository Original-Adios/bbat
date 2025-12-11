#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCameraOper : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CCameraOper)
public:
	CCameraOper(void);
	virtual ~CCameraOper(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:


	BOOL m_bOpen = FALSE;
	BOOL m_bClose = FALSE;

	SPRESULT CameraOpen();
	SPRESULT CameraClose();

	BOOL m_bMcu = FALSE;
};