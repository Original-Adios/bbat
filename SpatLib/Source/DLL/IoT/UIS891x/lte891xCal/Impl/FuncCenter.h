#pragma once
#include <map>
#include <string>

class CFuncCenter
{
public:
    CFuncCenter(void);
    ~CFuncCenter(void);

    void AddFunc(LPCWSTR lpName, void* pFunc);
    void* GetFunc(LPCWSTR lpName);

private:
    std::map<std::wstring, void*> m_mapCenter;
};


