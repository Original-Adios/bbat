#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CAudioSaveRecorder : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAudioSaveRecorder)
public:
    CAudioSaveRecorder(void);
    virtual ~CAudioSaveRecorder(void);

private:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::wstring m_strFilePath;
	SP_AUDIO_REC_T m_RecordSet;
	BOOL m_bSNSuffix;
	BOOL m_bTimeSuffix;
	BOOL m_bRecordInfoSuffix;
};
