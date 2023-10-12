#pragma once
#include "Pass.h"

namespace TinyEngine
{
    class Scene;

    class Buffer
    {
    public:
        void Register() {

        }

        void Release() {

        }
    private:
        size_t bufferID;
    };

    class EffectPassBase
    {
        public:
		EffectPassBase() noexcept {}

		bool IsEnabled() const noexcept { return enabled; }
		virtual void Apply(Graphics* pGfx, std::shared_ptr<Scene> pFrameBuffer) = 0;
		virtual ~EffectPassBase() {
            
		}
		
	protected:
		std::vector<ShaderPass> m_ShaderPasses;
		bool enabled;

		ShaderPass maskPass;
		ShaderPass outlinePass;
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
        Buffer pFrameBuffer;
        Buffer pLightingBuffer;
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

        private:
        ShaderPass maskPass;
        ShaderPass outlinePass;
    };
}