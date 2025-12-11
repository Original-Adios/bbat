#include "InputCodes.h"

//////////////////////////////////////////////////////////////////////////

class CHciBtInputCode : public CInputCodes
{
	DECLARE_RUNTIME_CLASS(CHciBtInputCode)
public:
	CHciBtInputCode(void);
	virtual ~CHciBtInputCode(void);
protected:
	virtual SPRESULT __PollAction(void);

};