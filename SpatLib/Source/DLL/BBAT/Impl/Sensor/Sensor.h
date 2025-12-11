#pragma once
#include "../ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CSensor)
public:
	CSensor(void);
	virtual ~CSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:
	BOOL m_bMagnetic;
	BOOL m_bProximity;
	BOOL m_bLight;

	SPRESULT RunMag();
	SPRESULT CheckMag(MagReturn DataRecv);
	SPRESULT RunPro();
	SPRESULT RunLig();

	SPAT_VALUE m_MagValueX;
	SPAT_VALUE m_MagValueY;
	SPAT_VALUE m_MagValueZ;
	SPAT_VALUE m_ProValue;
	SPAT_VALUE m_LigValue;

};

#define SPECIAL 0x20