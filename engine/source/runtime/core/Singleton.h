#pragma once

template<typename T>
class Singleton
{
public:
    static T& Get()
    {
        static T instance;
        return instance;
    }

protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};