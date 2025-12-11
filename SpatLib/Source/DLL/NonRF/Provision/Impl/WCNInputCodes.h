#include "InputCodes.h"

//////////////////////////////////////////////////////////////////////////

class CWCNInputCodes : public CInputCodes
{
	DECLARE_RUNTIME_CLASS(CWCNInputCodes)
public:
	CWCNInputCodes(void);
	virtual ~CWCNInputCodes(void);
protected:
	virtual SPRESULT __PollAction(void);

};