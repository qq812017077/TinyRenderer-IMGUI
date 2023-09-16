#pragma once
#include <string>
#include <vector>
#include "components/Transform.h"

namespace TinyEngine
{
    struct GameObjectRes{
        std::string              m_name;
        Transform                m_transform;
        std::string              m_definition;
        std::vector<std::string> m_components;
    };
}