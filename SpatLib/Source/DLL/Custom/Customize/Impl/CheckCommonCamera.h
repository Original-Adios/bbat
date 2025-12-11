#pragma once
#include "ImpBase.h"

#define MAX_CAMERA_COUNT  (10)
#define MAX_CAMERA_INFOR_LENGTH  (128)
typedef struct _tagCAMERA_INFOR
{
	char  szCameraType[MAX_CAMERA_INFOR_LENGTH + 1];
	char  szCameraSize[MAX_CAMERA_INFOR_LENGTH + 1];

	STRUCT_INITIALIZE(_tagCAMERA_INFOR);
}CAMERA_INFOR, *PCAMERA_INFOR;
//////////////////////////////////////////////////////////////////////////
class CCheckCommonCamera : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCommonCamera)
public:
    CCheckCommonCamera(void);
    virtual ~CCheckCommonCamera(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);

private:
	uint8						m_nCameraCount;
	std::vector<CAMERA_INFOR>	m_vecCameraInfo;
};