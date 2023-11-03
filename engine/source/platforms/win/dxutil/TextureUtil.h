#pragma once
#include <memory>
#include <wrl.h>

class Texture;
class CubeTexture;
class DirectXGraphics;
namespace TinyEngine::Graph
{
    class DXDefaultRenderGraph;
}
namespace TinyEngine::Rendering
{
    class DirectXCubeTexture;
}

namespace TinyEngine::Util
{
    class DXTextureUtil
    {
        public:
            static std::shared_ptr<CubeTexture> RenderIrradianceMap(DirectXGraphics* pGfx, TinyEngine::Graph::DXDefaultRenderGraph&  graph, 
            TinyEngine::Rendering::DirectXCubeTexture& skyboxTexture, int width = 32, int height = 32);
            static std::shared_ptr<CubeTexture> GeneratePrefilterMap(DirectXGraphics* pGfx, TinyEngine::Graph::DXDefaultRenderGraph& graph, 
            TinyEngine::Rendering::DirectXCubeTexture& skyboxTexture, int width = 256, int height = 256, int mipLevels = 5);
            static std::shared_ptr<Texture> GenerateBRDFLUT(DirectXGraphics* pGfx, TinyEngine::Graph::DXDefaultRenderGraph & graph, 
            bool saveToFile = false, unsigned int width = 512, unsigned int height = 512);
    };
}