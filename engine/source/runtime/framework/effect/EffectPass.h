#pragma once
#include <memory>
#include <vector>
#include "Pass.h"
class Graphics;
namespace TinyEngine
{
    class Scene;
    class RenderTarget;
    class DepthStencil;

    class EffectPassBase
    {
        public:
		EffectPassBase() noexcept {}
        virtual ~EffectPassBase() {
            
		}
        virtual void Initialize(Graphics * pGfx) {}
		bool IsEnabled() const noexcept { return enabled; }
		virtual void Apply(Graphics* pGfx, std::shared_ptr<Scene> pFrameBuffer) = 0;
		
		
	protected:
		std::vector<ShaderPass> m_ShaderPasses;
		bool enabled;
    };

    /***
     * @brief shadow pass
     *  including directional light and point light
     *    note: we need check shadow enable or not and we also need check mesh cast shadow or not
    */
    class ShadowEffectPass : public EffectPassBase
    {
    };

    /***
     * @brief shading pass
     *  including all lighting part and unlit part
    */
    class ShadingEffectPass : public EffectPassBase
    {
        public:
        ShadingEffectPass();

        ~ShadingEffectPass() override;

        void Apply(Graphics* pGfx, std::shared_ptr<Scene> scene) override;

        private:
    };


    class PostProcessEffectPass : public EffectPassBase
    {
    };

    class MousePickEffectPass : public EffectPassBase
    {
    public:
        MousePickEffectPass();
        ~MousePickEffectPass() override;

        void Apply(Graphics* pGfx, std::shared_ptr<Scene> scene) override;
        
        std::shared_ptr<RenderTarget> pRenderTarget{nullptr};
        std::shared_ptr<DepthStencil> pDepthStencil{nullptr};

    private:
        ShaderPass maskPass;
        ShaderPass outlinePass;
        ShaderPass fullscreenPass;
    };
}