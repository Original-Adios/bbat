#include "StdAfx.h"
#include "SpatRuntimeClass.h"


CSpatRuntimeClass* CSpatRuntimeClass::m_lpFirstClass = NULL;

//////////////////////////////////////////////////////////////////////////
CSpatRuntimeClass::CSpatRuntimeClass(
    LPCWSTR lpClassName,
    LPFN_CREATE_OBJECT lpfnCreateObject,
    CSpatRuntimeClass* lpNextClass/* = NULL */
    )
    : m_lpClassName(lpClassName)
    , m_lpfnCreateObject(lpfnCreateObject)
    , m_lpNextClass(lpNextClass)
{

}

CSpatRuntimeClass::~CSpatRuntimeClass(void)
{

}

SP_HANDLE CSpatRuntimeClass::CreateObject(void)
{
    return (NULL != m_lpfnCreateObject) ? (*m_lpfnCreateObject)() : INVALID_NPI_HANDLE;
}

SP_HANDLE CSpatRuntimeClass::CreateObject(LPCWSTR lpClassName)
{
    CSpatRuntimeClass* lpCurrClass = m_lpFirstClass;
    while (NULL != lpCurrClass) 
    {
        if (0 == _wcsicmp(lpClassName, lpCurrClass->m_lpClassName))
        { 
            return lpCurrClass->CreateObject(); 
        }

        lpCurrClass = lpCurrClass->m_lpNextClass;
    }

    return INVALID_NPI_HANDLE;
}

//////////////////////////////////////////////////////////////////////////
SPAT_INIT_RUNTIME_CLASS::SPAT_INIT_RUNTIME_CLASS(CSpatRuntimeClass* pNewClass)
{
    pNewClass->m_lpNextClass = CSpatRuntimeClass::m_lpFirstClass;
    CSpatRuntimeClass::m_lpFirstClass = pNewClass;
}