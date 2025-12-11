#include "StdAfx.h"
#include "FuncCenter.h"

CFuncCenter::CFuncCenter(void)
{
}

CFuncCenter::~CFuncCenter(void)
{
}

void CFuncCenter::AddFunc( LPCWSTR lpName, void* pFunc )
{
    m_mapCenter[lpName] = pFunc;
}

void* CFuncCenter::GetFunc( LPCWSTR lpName )
{
    return m_mapCenter[lpName];
}