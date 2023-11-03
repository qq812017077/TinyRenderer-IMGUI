#pragma once
#include <vector>
#include <array>
#include <memory>

class Texture;
class CubeTexture;
namespace TinyEngine
{
    struct SkyBoxMap
    {
        std::vector<std::string> paths;
    };

    struct SceneIBLResourceDesc
    {
        SkyBoxMap               m_skybox_irradiance_map;
        SkyBoxMap               m_skybox_specular_map;
        std::string             m_brdf_map;
    };

    struct SceneResourceDesc
    {
        SkyBoxMap               m_skybox_map;
        SceneIBLResourceDesc    m_ibl_res;
    };

    class SceneResource
    {
    public:
        std::shared_ptr<CubeTexture> m_skybox_cubemap{nullptr};
        std::shared_ptr<CubeTexture> m_irradiance_cubemap{nullptr};
        std::shared_ptr<CubeTexture> m_prefilter_cubemap{nullptr};
        std::shared_ptr<Texture> m_brdf_lut_map{nullptr};
    };
}