#pragma once
#include "../ImpBase.h"
#include "BTApiHCI.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
//////////////////////////////////////////////////////////////////////////

class CHciBtWriteAddr : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CHciBtWriteAddr)
public:
    CHciBtWriteAddr(void);
    virtual ~CHciBtWriteAddr(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    SPRESULT SaveDataIntoLocal(void);
    SPRESULT CheckUniqueBT(const char* lpAddr);

    BOOL m_bCheckUniqueBT;
    BOOL m_bSaveDataIntoTXT;
    INPUT_CODES_T m_InputSN[BC_MAX_NUM];
	std::wstring m_strTXTpath;

};
