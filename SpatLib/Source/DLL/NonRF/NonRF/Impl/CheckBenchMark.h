#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
/*
Bug1223284:
SendATCommand: AT+BENCHMARK=T5D10
--> 30(0x0000001e) Bytes
    00000000h: 7E 00 00 00 00 1C 00 68 00 41 54 2B 42 45 4E 43 : ~......h.AT+BENC
    00000010h: 48 4D 41 52 4B 3D 54 35 44 31 30 0D 0A 7E       : HMARK=T5D10..~  
<<- 124(0x0000007c) Bytes
    00000000h: 00 00 00 00 7C 00 9C 00 55 63 70 75 62 65 6E 63 : ....|...Ucpubenc
    00000010h: 68 20 73 63 6F 72 65 3A 0A 43 50 55 31 3A 20 7B : h score:.CPU1: {
    00000020h: 33 31 38 35 2E 35 7D 3B 0A 43 50 55 32 3A 20 7B : 3185.5};.CPU2: {
    00000030h: 33 31 39 37 2E 34 7D 3B 0A 43 50 55 33 3A 20 7B : 3197.4};.CPU3: {
    00000040h: 33 31 38 39 2E 31 7D 3B 0A 43 50 55 34 3A 20 7B : 3189.1};.CPU4: {
    00000050h: 33 31 39 32 2E 37 7D 3B 0A 0D 0A 55 63 70 75 62 : 3192.7};...Ucpub
    00000060h: 65 6E 63 68 20 42 65 6E 63 68 6D 61 72 6B 20 74 : ench Benchmark t
    00000070h: 65 73 74 20 50 61 73 73 2E 20 0D 0A             : est Pass. ..    
<<- 14(0x0000000e) Bytes
    00000000h: 00 00 00 00 0E 00 9C 00 0D 0A 4F 4B 0D 0A       : ..........OK..  
*/
//////////////////////////////////////////////////////////////////////////
class CCheckBenchMark : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckBenchMark)
public:
    CCheckBenchMark(void);
    virtual ~CCheckBenchMark(void);

private:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
    void DisplayProgress(void);

private:
    std::string m_strArgs;
    DWORD m_dwSpan;
    volatile BOOL m_bFinished;
};
