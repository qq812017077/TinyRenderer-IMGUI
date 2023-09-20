#pragma once
#include <vector>
#include <string>
#include <variant>
#include "Mesh.h"
#include <unordered_map>
#include <memory>

struct Bounds;

template<class T, class V>
struct IsVariantMember;

template<class T, class... ALL_V>
struct IsVariantMember<T, std::variant<ALL_V...>> : public std::disjunction<std::is_same<T, ALL_V>...> {};

struct Vector4;
struct Matrix4x4;

using Property = std::variant<
    int, uint32_t, float, Vector4, ColorRGBA, Matrix4x4, 
    std::vector<float>, std::vector<Vector4>, std::vector<ColorRGBA>, std::vector<Matrix4x4>,
    std::string>;

using XID = size_t;
inline XID StringToID(std::string_view str)
{
    static std::hash<std::string_view> hash;
    return hash(str);
}

class Material;
struct Model
{
    struct Node
    {
        Node(std::string name)
            : name(name){}
        Node(std::string name, Matrix4x4 transform)
            : name(name), transform(transform){}
        std::string name;
        Matrix4x4 transform;
        std::vector<uint32_t> meshIndices;
        std::vector<std::unique_ptr<Node>> children;
    };
    struct MeshData
    {
        Mesh mesh;
        uint32_t m_MaterialIndex = 0;
        bool m_InFrustum = true;
    };
    
    Model();

    struct MaterialData
    {
        MaterialData() = default;

        void Clear() { m_Properties.clear(); }

        template<class T>
        void Set(std::string_view name, const T& value)
        {
            static_assert(IsVariantMember<T, Property>::value, "Type T isn't one of the Property types!");
            m_Properties[StringToID(name)] = value;
        }

        template<class T>
        const T& Get(std::string_view name) const
        {
            auto it = m_Properties.find(StringToID(name));
            return std::get<T>(it->second);
        }

        template<class T>
        bool Has(std::string_view name) const
        {
            auto it = m_Properties.find(StringToID(name));
            if (it == m_Properties.end() || !std::holds_alternative<T>(it->second))
                return false;
            return true;
        }

        bool HasProperty(std::string_view name) const
        {
            return m_Properties.find(StringToID(name)) != m_Properties.end();
        }

        std::string name;
        private:
        std::unordered_map<XID, Property> m_Properties;
    };
    std::string model_path;
    std::unique_ptr<Node> pRoot;
    std::vector<MeshData> meshdatas;
    std::vector<MaterialData> materialdatas;
    Bounds bounds;

    std::vector<std::shared_ptr<Material>> GenerateMaterials();
    static bool Load(const std::string& filePath, Model& model);
};