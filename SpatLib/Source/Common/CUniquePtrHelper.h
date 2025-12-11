#pragma once
#include <memory>

template<class T>
class CUniquePtrHelper
{
public:
    _NODISCARD T* operator->()
    {
        if (!m_ptr)
        {
            m_ptr = std::make_unique<T>();
        }

        return m_ptr.get();
    };
	
    _NODISCARD T* get()
    {
        if (!m_ptr)
        {
            m_ptr = std::make_unique<T>();
        }

        return m_ptr.get();
    };

private:
    std::unique_ptr<T> m_ptr = nullptr;
};
