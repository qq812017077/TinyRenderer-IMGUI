#include "Tools.h"

unsigned int Tools::UniqueCode(std::string materialName)
{
    return static_cast<unsigned int>(std::hash<std::string>{}(materialName));
}


