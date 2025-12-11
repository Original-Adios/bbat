#pragma once
#include "SpatContainer.hpp"

SpatContainer::CParamsContainer g_SpatParamsContainer;
SpatContainer::CClassContainer g_SpatClassContainer;

namespace SpatContainer
{
    class CRuntimeParamsContainer sealed
    {
    private:
        CRuntimeParamsContainer( const CRuntimeParamsContainer& ) = delete;
        CRuntimeParamsContainer( CRuntimeParamsContainer&& ) = delete;
        CRuntimeParamsContainer& operator = ( const CRuntimeParamsContainer& ) = delete;
        CRuntimeParamsContainer& operator = ( CRuntimeParamsContainer&& ) = delete;
        
    public:
        CRuntimeParamsContainer() = default;
        ~CRuntimeParamsContainer() = default;
        
        template <typename... TArgs>
        CRuntimeParamsContainer( std::wstring&& class_name, std::wstring&& new_path, TArgs&& ... args )
        {
            g_SpatParamsContainer.Bind( std::forward<std::wstring>( class_name ), std::forward<std::wstring>( new_path ), std::forward<TArgs>( args )... );
        };
    };
    
    class CRuntimeClassContainer sealed
    {
    private:
        CRuntimeClassContainer( const CRuntimeClassContainer& ) = delete;
        CRuntimeClassContainer( CRuntimeClassContainer&& ) = delete;
        CRuntimeClassContainer& operator = ( const CRuntimeClassContainer& ) = delete;
        CRuntimeClassContainer& operator = ( CRuntimeClassContainer&& ) = delete;
    public:
        CRuntimeClassContainer() = default;
        ~CRuntimeClassContainer() = default;
        
        template <typename... TArgs>
        CRuntimeClassContainer( std::wstring&& new_name, TArgs&& ... args )
        {
            g_SpatClassContainer.Bind( std::forward<std::wstring>( new_name ), std::forward<TArgs>( args )... );
        };
    };
}
