// CustMFC.h : main header file for the CustMFC DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCustMFCApp
// See CustMFC.cpp for the implementation of this class
//

class CCustMFCApp : public CWinApp
{
public:
	CCustMFCApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
