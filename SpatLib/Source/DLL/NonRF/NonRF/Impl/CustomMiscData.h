#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
/// This class is an example to show how to read or write data into customer area of miscdata.
/// As the maximize of customer data is 256KB, please do not exceed 256KB
/// 
class CCustomerMiscData : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCustomerMiscData)
public:
    CCustomerMiscData(void);
    virtual ~CCustomerMiscData(void);

protected:
	virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
	uint32 m_u32Base;
	uint32 m_u32Size;
};
