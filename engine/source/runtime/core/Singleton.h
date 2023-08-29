#pragma once
#ifdef DEBUG
#include <iostream>
#endif
#include <memory>
template<typename T>
class Singleton
{
public:
    static T& Get()
    {
        return *pInstance;
    }
    static std::shared_ptr<T> GetPtr()
    {
        return pInstance;
    }

protected:
    Singleton()
    {
#ifdef DEBUG
        std::cout << "Singleton constructor <" << typeid(T).name() << "> called" << std::endl;
#endif
    }
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    static std::shared_ptr<T> pInstance;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::pInstance = std::make_shared<T>();