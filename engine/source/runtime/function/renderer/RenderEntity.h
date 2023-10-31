#pragma once
#include "Mesh.h"
#include "Material.h"
#include "core/math/EngineMath.h"
#include "Shader.h"
#include "effect/Pass.h"
#include <vector>

namespace TinyEngine
{
    enum class EDataType
    {
        Integer,
        Float,
        Color,
        Matrix4x4,
        Vector4
    };

    template<typename T>
    struct VarMap { using Type = T; };
    #define VAR_TYPE(type, enumType) template<> struct VarMap<type> { using Type = type; static constexpr EDataType TypeEnum = EDataType::enumType; }
    VAR_TYPE(int, Integer);
    VAR_TYPE(float, Float);
    VAR_TYPE(Color, Color);
    VAR_TYPE(Matrix4x4, Matrix4x4);
    VAR_TYPE(Vector4, Vector4);
    #undef VAR_TYPE

    class Variable
    {
    public:
        template<typename T>
        operator T&()
        {
            if(type != VarMap<T>::TypeEnum) { throw std::runtime_error("Type mismatch"); }
            return std::get<T>(variable);
        }
        template<typename T>
        operator T() const
        {
            return const_cast<Variable&>(*this).operator T&();
        }

        template<typename T>
        void operator=(const T& value)
        {
            variable = value;
            type = VarMap<T>::TypeEnum;
        }
        EDataType GetType() const { return type; }
    private:
        using Var = std::variant<int, float, Matrix4x4, Vector4>;
        Var variable;
        EDataType type;
        bool dirty;
    };

    class RenderEntity
    {
    public:
        uint32_t  m_instance_id {0};
        Matrix4x4 m_world_matrix {Matrix4x4::Identity()};
        Matrix4x4 m_world_matrix_inv {Matrix4x4::Identity()};
        std::map<std::string, Variable> m_variable_map;
        
        // pass
        ShaderPass* m_pass {nullptr};
        // mesh
        Mesh * m_mesh {nullptr};
        // material
        Material * m_material {nullptr};
    };

    
}