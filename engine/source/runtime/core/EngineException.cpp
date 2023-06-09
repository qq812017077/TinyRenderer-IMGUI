#include "EngineException.h"

EngineException::EngineException(int line, const char* file, const char* msg) noexcept
    :
    line(line),
    file(file),
    msg(msg)
{}

const char* EngineException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

int EngineException::GetLine() const noexcept
{
    return line;
}

const std::string& EngineException::GetFile() const noexcept
{
    return file;
}

std::string EngineException::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[File] " << file << std::endl
        << "[Line] " << line << std::endl
        << "[Message] " << msg;
    return oss.str();
}

