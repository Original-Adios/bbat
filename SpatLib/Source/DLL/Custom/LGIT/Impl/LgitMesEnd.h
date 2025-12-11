#pragma once
#include "ImpBase.h"
#include "IItemDataObserver.h"
#include "LockQueue.h"
typedef CLockQueue<IItemDataObserver::ITEMDATA_T> CDataLockQueue;
//////////////////////////////////////////////////////////////////////////
class CLgitMesEnd : public CImpBase
    , public IItemDataObserver
{
    DECLARE_RUNTIME_CLASS(CLgitMesEnd)
public:
    CLgitMesEnd(void);
    virtual ~CLgitMesEnd(void);

protected:
    //virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

    // Interface of <IItemDataObserver>
    virtual void Release(void) { delete this; };
    virtual void Clear(void);
    virtual void PushData(CONST ITEMDATA_T& item);

private:
    std::string& GetCTQData(std::string &strCTQData);
    BOOL SaveCompletionInfo(const std::string filePath, const std::string strInfo);

private:
//    DWORD m_dwPort;
//    std::string m_strIP;
    CDataLockQueue m_LockQ;
    volatile BOOL  m_bStopUpload;  // Stop to upload
};
