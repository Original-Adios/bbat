#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckCalFlag : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CCheckCalFlag)
public:
	CCheckCalFlag(void);
	virtual ~CCheckCalFlag(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:
	struct FLAGS
	{
		BOOL bADC;
		BOOL bAFC;
		BOOL bCalAGC2G[BI_GSM_MAX_BAND];
		BOOL bCalAPC2G[BI_GSM_MAX_BAND];
		BOOL bFT2G[BI_GSM_MAX_BAND];
		BOOL bCalEDGE;
		BOOL bFTEDGE;
		BOOL bCalAGCTD;
		BOOL bCalAPCTD;
		BOOL bFTTD;
		BOOL bCalWCDMA[BI_W_MAX_BAND];
		BOOL bFTWCDMA;
		BOOL bCalTDD;
		BOOL bCalFDD;
		BOOL bFTTDD;
		BOOL bFTFDD;
		BOOL bANTTDD;//add TDD Antenna flag Bug 1478777  2021.3.1
		BOOL bANTFDD;//add FDD Antenna flag Bug 1478777  2021.3.1
		BOOL bAFCTDD;//add TDD AFC flag Bug 1692513  2021.11.11
		BOOL bAFCFDD;//add FDD AFC flag Bug 1692513  2021.11.11
		BOOL bCalCDMA;
		BOOL bFTCDMA;
		BOOL bAntenna;

		BOOL bDMRAFC;
		BOOL bDMRAPC;
		BOOL bDMRAGC;
		BOOL bDMRFT;

		STRUCT_INITIALIZE(FLAGS);
	} m_Flags;
};
