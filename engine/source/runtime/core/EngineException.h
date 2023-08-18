#pragma once
#include <exception>
#include <string>
#include <iostream>
#include <sstream>

class EngineException : public std::exception
{
public:
    EngineException(int line, const char* file, const char* msg) noexcept;
    const char* what() const noexcept override;
    virtual const char* GetType() const noexcept;
    int GetLine() const noexcept;
    const std::string& GetFile() const noexcept;
    std::string GetOriginString() const noexcept;
private:
    int line;
    std::string file;
    std::string msg;
protected:
    mutable std::string whatBuffer;
};