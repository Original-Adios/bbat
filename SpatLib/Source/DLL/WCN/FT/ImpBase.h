#pragma once
#include "SpatBase.h"

#define CHKRESULTwithGpsLeave(statement)    {   								\
									SPRESULT __sprslt = (statement);		\
									if (SP_OK != __sprslt)					\
									{										\
										GPS_ModeEnable(GPS_LEAVE);          \
										m_pRFTester->EndTest(0);             \
										return __sprslt;					\
									}										\
								}

#define CHKRESULTwithNetWorkSet(statement)    {   								\
									SPRESULT __sprslt = (statement);		\
									if (SP_OK != __sprslt)					\
									{										\
										CHKRESULT(m_pclGpsApi->DUT_SetNetwork(1));\
										return __sprslt;					\
									}										\
								}

class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);

protected:
    virtual BOOL LoadXMLConfig(void);

protected:
    INT m_ueChip; // Bug1044543
};
