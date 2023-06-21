#pragma once
#include <string>

struct UniformVar
{
    std::string name;
    // std::string type;
    unsigned int size;
    // int offset;
    // int arraySize;
    // int arrayStride;
    // int matrixStride;
    UniformVar(std::string name, unsigned int size)
    {
        this->name = name;
        this->size = size;
    }
};