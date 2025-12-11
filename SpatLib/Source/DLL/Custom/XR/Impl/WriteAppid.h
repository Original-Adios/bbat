#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>
#include "UnisocMesBase.h"

//////////////////////////////////////////////////////////////////////////
class CWriteAppid : public CUnisocMesBase
{
    DECLARE_RUNTIME_CLASS(CWriteAppid)
public:
    CWriteAppid(void);
    virtual ~CWriteAppid(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);

private:
	uint32 m_u32AppidBase;		//APPID写入起始地址
	uint8 m_u8AppidDigts;			//APPID位数
};
