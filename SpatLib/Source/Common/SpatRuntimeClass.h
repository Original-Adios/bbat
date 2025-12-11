#pragma once
#include "global_def.h"

///
typedef SP_HANDLE (*LPFN_CREATE_OBJECT)(void);
//////////////////////////////////////////////////////////////////////////
class CSpatRuntimeClass sealed
{
    CLASS_UNCOPYABLE(CSpatRuntimeClass)
public:
    CSpatRuntimeClass(
        LPCWSTR lpClassName,
        LPFN_CREATE_OBJECT lpfnCreateObject,
        CSpatRuntimeClass* lpNextClass = NULL
        );
    ~CSpatRuntimeClass(void);

    SP_HANDLE CreateObject(void);
    SP_HANDLE CreateObject(LPCWSTR lpClassName);

public:
    LPCWSTR                     m_lpClassName;
    LPFN_CREATE_OBJECT          m_lpfnCreateObject;
    CSpatRuntimeClass*          m_lpNextClass;
    static CSpatRuntimeClass*   m_lpFirstClass;

};

struct SPAT_INIT_RUNTIME_CLASS
{ 
    SPAT_INIT_RUNTIME_CLASS(CSpatRuntimeClass* pNewClass); 
};

#define SPAT_RUNTIME_CLASS(class_name) \
    (&class_name::class##class_name)

#define DECLARE_RUNTIME_CLASS(class_name) \
public: \
    static CSpatRuntimeClass class##class_name; \
    static SP_HANDLE   CreateObject(void); \
    CSpatRuntimeClass* GetRuntimeClass(void)const;  


#define IMPLEMENT_RUNTIME_CLASS(class_name) \
    static wchar_t g_lpsz##class_name[]  = L#class_name;  \
    CSpatRuntimeClass class_name::class##class_name( \
        g_lpsz##class_name, \
        class_name::CreateObject, \
        NULL \
        ); \
    \
    CSpatRuntimeClass* class_name::GetRuntimeClass(void)const { \
        return &class_name::class##class_name; \
    } \
    \
    static SPAT_INIT_RUNTIME_CLASS g_init_##class_name(&class_name::class##class_name); \
    \
    SP_HANDLE class_name::CreateObject(void) { \
        try { \
            return static_cast<SP_HANDLE>(new class_name); \
        } \
        catch (const std::bad_alloc& /*e*/) { \
            return INVALID_NPI_HANDLE; \
        } \
    } 
