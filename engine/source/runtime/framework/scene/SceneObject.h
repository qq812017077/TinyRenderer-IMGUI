#pragma once
#include <string>
#include <vector>
#include "core/math/Matrix.h"

namespace TinyEngine
{
    static const size_t s_invalid_go_id        = std::numeric_limits<size_t>::max();
    static const size_t s_invalid_component_id = std::numeric_limits<size_t>::max();

    struct GameObjectTransformDesc
    {
        Matrix4x4 transform;
    };
    
    struct GameObjectComponentDesc
    {
        GameObjectTransformDesc transform_dec;
    };

    struct ComponentId
    {
        size_t go_id {s_invalid_go_id};
        size_t component_id {s_invalid_component_id};
        bool operator==(const ComponentId& rhs) const { return go_id == rhs.go_id && component_id == rhs.component_id; }
        size_t getHashValue() const { return go_id ^ (component_id << 1); }
        bool   isValid() const { return go_id != s_invalid_go_id && component_id != s_invalid_component_id; }
    };

    class GameObjectDesc
    {
    public:
        GameObjectDesc() : m_go_id(0) {}
        GameObjectDesc(size_t go_id, const std::vector<GameObjectComponentDesc>& components) :
            m_go_id(go_id), m_components(components)
        {}
        GameObjectDesc(const GameObjectDesc& _go) { *this = _go; }
        GameObjectDesc& operator=(const GameObjectDesc& _go)
        {
            m_go_id      = _go.m_go_id;
            m_components = _go.m_components;
            return *this;
        }

        size_t                                      getId() const { return m_go_id; }
        const std::vector<GameObjectComponentDesc>& getComponents() const { return m_components; }

    private:
        size_t                               m_go_id;
        std::vector<GameObjectComponentDesc> m_components;
    };

} // namespace Pilot

template<>
struct std::hash<TinyEngine::ComponentId>
{
    size_t operator()(const TinyEngine::ComponentId& rhs) const noexcept { return rhs.getHashValue(); }
};