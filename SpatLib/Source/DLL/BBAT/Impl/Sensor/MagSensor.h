#pragma once
#include "../ImpBase.h"
 
//////////////////////////////////////////////////////////////////////////
class CMagSensor : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CMagSensor)
public:
	CMagSensor(void);
	virtual ~CMagSensor(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:


	SPAT_VALUE m_MagValueX;
	SPAT_VALUE m_MagValueY;
	SPAT_VALUE m_MagValueZ;

	SPRESULT RunMagTest();
	SPRESULT CheckMag(MagReturn DataRecv);

};






