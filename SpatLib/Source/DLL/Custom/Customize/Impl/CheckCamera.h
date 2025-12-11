#pragma once
#include "ImpBase.h"
/*
[2018-07-30 14:26:34:411] SendAtCommand:
[2018-07-30 14:26:34:411] --> 23(0x00000017) Bytes
00000000h: 7E 00 00 00 00 15 00 68 00 41 54 2B 43 41 4D 30 ; ~......h.AT+CAM0
00000010h: 53 49 5A 45 0D 0A 7E                            ; SIZE..~
[2018-07-30 14:26:34:413] <-- 10(0x0000000a) Bytes
00000000h: 7E 00 00 00 00 08 00 D5 00 7E                   ; ~........~
[2018-07-30 14:26:34:413] <-- 16(0x00000010) Bytes
00000000h: 7E 00 00 00 00 0E 00 9C 00 37 39 30 57 0D 0A 7E ; ~........790W..~
[2018-07-30 14:26:34:413] <-- 16(0x00000010) Bytes
00000000h: 7E 00 00 00 00 0E 00 9C 00 0D 0A 4F 4B 0D 0A 7E ; ~..........OK..~
[2018-07-30 14:26:34:413] Error Code = 0 (PC_ERR_SUCCESS)
[2018-07-30 14:26:36:058] SendAtCommand:
[2018-07-30 14:26:36:059] --> 23(0x00000017) Bytes
00000000h: 7E 00 00 00 00 15 00 68 00 41 54 2B 43 41 4D 30 ; ~......h.AT+CAM0
00000010h: 53 49 5A 45 0D 0A 7E                            ; SIZE..~
[2018-07-30 14:26:36:060] <-- 10(0x0000000a) Bytes
00000000h: 7E 00 00 00 00 08 00 D5 00 7E                   ; ~........~
[2018-07-30 14:26:36:061] <-- 16(0x00000010) Bytes
00000000h: 7E 00 00 00 00 0E 00 9C 00 37 39 30 57 0D 0A 7E ; ~........790W..~
[2018-07-30 14:26:36:061] <-- 16(0x00000010) Bytes
00000000h: 7E 00 00 00 00 0E 00 9C 00 0D 0A 4F 4B 0D 0A 7E ; ~..........OK..~
*/
#define MAX_CAMERA_COUNT  (2)
//////////////////////////////////////////////////////////////////////////
class CCheckCamera : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCamera)
public:
    CCheckCamera(void);
    virtual ~CCheckCamera(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

    SPRESULT CheckCamera(INT nCamNo);

private:
    BOOL            m_bChkCamera[MAX_CAMERA_COUNT];
    std::string  m_strCameraType[MAX_CAMERA_COUNT];
    std::string  m_strCameraSize[MAX_CAMERA_COUNT];
};