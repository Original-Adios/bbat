#pragma once
#include "ImpBase.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include "GenCodes.h"

//////////////////////////////////////////////////////////////////////////


class CInputCodes : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CInputCodes)
public:
    CInputCodes(void);
    virtual ~CInputCodes(void);
protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    void GetXMLConfig(INPUT_CODES_T* pInput, std::wstring strCodeName, BOOL bHex = FALSE);
	void SetJsConfig(BC_INDEX eIndex, Value* vArray, Document::AllocatorType& allocator);
    SPRESULT GetInputCode(BC_INDEX eIndex);
    SPRESULT InitGenCode(BC_INDEX index);
	SPRESULT Debug();

protected:
	INPUT_CODES_T  m_InputSN[BC_MAX_NUM];
	CGenCodes     m_GenCodes[BC_MAX_NUM];
    BOOL  m_bCheckXmode;

	void SetJsConfig_XR(INPUT_CODES_T *m_Input, Value *vArray, Document::AllocatorType &allocator);
	SPRESULT CInputCodes::GetInputCode_XR(INPUT_CODES_T *pInputCode);
	//XunRui by binglin.wen
	INPUT_CODES_T m_InputAppid;
	BOOL m_bCheckInputKuCodes;
	int m_nKudaSnLength;
	INPUT_CODES_T m_DeviceId;
	INPUT_CODES_T m_KudaSN;
	INPUT_CODES_T m_OneNetDevId;
	INPUT_CODES_T m_OneNetProductId;
	INPUT_CODES_T m_DevSecret;

	void SetKudaXMLConfig(INPUT_CODES_T *pInputCode, int iMaxLenght,std::wstring strCodeName);
	void SetKudaInputCodes();

	void PrintKudaCodeInfo(INPUT_CODES_T *pInputCode);
	void PrintKudaCodesInfo();
	INPUT_CODES_T m_InputManualCode;

};
