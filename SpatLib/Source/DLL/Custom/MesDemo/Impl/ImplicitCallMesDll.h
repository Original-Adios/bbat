#pragma once
#include "impbase.h"

#include "../CallMesDll/ICallMesDll.h"
#pragma comment(lib, "../../../../../../Bin/App/SpatLib/CallMesDemo.lib")//

class CImplicitCallMesDll :	public CImpBase
{
	DECLARE_RUNTIME_CLASS(CImplicitCallMesDll)
public:
	CImplicitCallMesDll(void);
	~CImplicitCallMesDll(void);

protected:
	virtual SPRESULT __PollAction (void);
	virtual BOOL     LoadXMLConfig(void);

	std::wstring m_strServerType;
	std::wstring m_strServerIP;
	std::wstring m_strServerDatabase;
	std::wstring m_strServerUser;
	std::wstring m_strServerPassword;
};

