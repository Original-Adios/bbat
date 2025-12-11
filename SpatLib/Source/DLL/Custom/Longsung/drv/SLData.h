#pragma once
#include "IItemDataObserver.h"
#include "../../../../../../Common/thread/Thread.h"
#include "LockQueue.h"
#include <string>

class CSLDriver;
class ISpLog;
typedef CLockQueue<IItemDataObserver::ITEMDATA_T> CDataLockQueue;
//////////////////////////////////////////////////////////////////////////
class CSLData : public IItemDataObserver
              , public CThread
{
public:
    CSLData(CSLDriver* pDrv, ISpLog* pLogUtil);
    virtual ~CSLData(void);

    BOOL StartUpload(BOOL bStart);

    // Interface of <IItemDataObserver>
    virtual void Release(void) { delete this; };
    virtual void Clear(void);
    virtual void PushData(CONST ITEMDATA_T& item);

    // Interface of <CThread>
    virtual void run(void);

private:
    std::string& RemoveKeyword(std::string& str, const std::string& keyword);
    std::string& trimA(std::string& str);

    std::string& get_pcl_from_condition(const std::string& cond, std::string& pcl);

public:
    CSLDriver*     m_pSLDrv;
    ISpLog*        m_pLogUtil;
    CDataLockQueue m_LockQ;
    volatile BOOL  m_bStopUpload;  // Stop to upload
};