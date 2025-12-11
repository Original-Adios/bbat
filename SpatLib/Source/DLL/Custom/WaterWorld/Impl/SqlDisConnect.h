#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CSqlDisConnect : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CSqlDisConnect)
public:
	CSqlDisConnect(void);
	virtual ~CSqlDisConnect(void);

protected:
	virtual SPRESULT __PollAction(void);
};