#pragma once
#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <codecvt>

using SpatParamsPath = std::vector<std::wstring>;
using SpatParamsContainer = std::multimap<std::wstring, std::wstring>;
using SpatClassContainer = std::map<std::wstring, std::wstring>;

namespace SpatContainer
{
    class CParamsContainer
    {
    public:
        CParamsContainer()
        {
            m_mapContainer.clear();
        }
        ~CParamsContainer() = default;
        
    public:
#define SPLIT_STR L"->"
    
        template <typename THead, typename... TArgs>
        void Bind( std::wstring&& class_name, std::wstring&& new_path, THead&& head, TArgs&& ... args )
        {
            Bind( std::forward<std::wstring>( class_name ), std::forward<std::wstring>( new_path ), std::forward<THead>( head ) );
            if ( 0 < sizeof...( args ) )
            {
                Bind( std::forward<std::wstring>( class_name ), std::forward<std::wstring>( new_path ), std::forward<TArgs>( args )... );
            }
        }
        
        template <typename THead>
        void Bind( std::wstring&& class_name, std::wstring&& new_path, THead&& head )
        {
            std::unique_lock<std::mutex> locker( m_mutex );
            std::wstring new_str = class_name + SPLIT_STR + new_path;
            std::wstring old_str = class_name + SPLIT_STR + head;
            m_mapContainer.insert( std::make_pair( new_str, old_str ) );
        }
        
        SpatParamsPath FindAllPath( std::wstring&& class_name, std::wstring&& new_path )
        {
            std::unique_lock<std::mutex> locker( m_mutex );
            SpatParamsPath arrAllPath;
            for ( auto it : m_mapContainer )
            {
                std::wstring new_str = class_name + SPLIT_STR + new_path;
                if ( new_str == it.first )
                {
                    std::wstring old_path = it.second;
                    auto pos = old_path.find( SPLIT_STR );
                    old_path = old_path.substr( pos + wcslen( SPLIT_STR ) );
                    arrAllPath.push_back( old_path );
                }
            }
            return arrAllPath;
        }
        
        std::wstring FindNewPath( std::wstring&& class_name, std::wstring&& old_path )
        {
            std::unique_lock<std::mutex> locker( m_mutex );
            std::wstring new_path = L"";
            std::wstring old_str = class_name + SPLIT_STR + old_path;
            for ( auto it : m_mapContainer )
            {
                if ( old_str == it.second )
                {
                    new_path = it.first;
                    auto pos = new_path.find( SPLIT_STR );
                    new_path = new_path.substr( pos + wcslen( SPLIT_STR ) );
                }
            }
            return new_path;
        }
        
    private:
        SpatParamsContainer m_mapContainer;
        std::mutex m_mutex;
    };
    
    class CClassContainer
    {
    public:
        CClassContainer()
        {
            m_mapContainer.clear();
        };
        ~CClassContainer() = default;
        
    public:
        template <typename THead, typename... TArgs>
        void Bind( std::wstring&& new_name, THead&& head, TArgs&& ... args )
        {
            Bind( std::forward<std::wstring>( new_name ), std::forward<THead>( head ) );
            if ( 0 < sizeof...( args ) )
            {
                Bind( std::forward<std::wstring>( new_name ), std::forward<TArgs>( args )... );
            }
        }
        
        template <typename THead>
        void Bind( std::wstring&& new_name, THead&& head )
        {
            std::unique_lock<std::mutex> locker( m_mutex );
			m_mapContainer.insert( std::make_pair( head, new_name ) );
        }
        
        std::wstring Find( std::wstring old_name )
        {
            std::unique_lock<std::mutex> locker( m_mutex );
            for ( auto it : m_mapContainer )
            {
                if ( old_name == it.first )
                {
                    return it.second;
                }
            }
            return L"";
        }
    private:
        SpatClassContainer m_mapContainer;
        std::mutex m_mutex;
    };
}

