#include "Scene.h"
#include "Frustum.h"
#include "BVH.h"


namespace TinyEngine
{
    void Scene::Load()
    {
        m_loaded = true;
    }


    void Scene::Clear()
    {
        m_loaded = false;
        
        selectedCamera = nullptr;
        m_renderers.clear();
        selectedRenderers.clear();
        CamVisibleRenderers.clear();
    }

    void Scene::updateVisibleObjects(std::vector<EffectDesc> & effectDescs, std::vector<ShadowCastDesc> & ShadowCastDescs)
    {
        // m_bvh_root = BVHAccel::Build(*this, BVHAccel::SplitMethod::NAIVE);
        updateVisibleObjectsDirectionalLight(ShadowCastDescs);
        updateVisibleObjectsPointLight(ShadowCastDescs);
        updateVisibleObjectsMainCamera(effectDescs);
    }

    void Scene::updateVisibleObjectsDirectionalLight(std::vector<ShadowCastDesc> & ShadowCastDescs)
    {
        // get viewproject matrix
        m_directional_light.m_lightViewProj = CalculateDirectionalLightCamera();
        // get frustum from viewproject matrix

        m_directional_light.visibleRenderers.clear();

        Frustum frustum(m_directional_light.m_lightViewProj);

        for(auto & desc : ShadowCastDescs)
        {
            for(auto & renderer : desc.renderers)
            {
                if(frustum.IntersectAABB(renderer->GetBounds()))
                {
                    m_directional_light.visibleRenderers.emplace_back(renderer);
                }
            }
        }
    }

    
    void Scene::updateVisibleObjectsPointLight(std::vector<ShadowCastDesc> & ShadowCastDescs)
    {
        // get bounds sphere from point light

        for(auto & point_light : m_point_lights)
        {
            point_light.visibleRenderers.clear();
            for(auto & desc : ShadowCastDescs)
            {
                for(auto & renderer : desc.renderers)
                {
                    if(renderer->GetBounds().IntersectsWithSphere(point_light.m_buffer.pos, point_light.m_buffer.range))
                    {
                        point_light.visibleRenderers.emplace_back(renderer);
                    }
                }
            }
        }
    }

    void Scene::updateVisibleObjectsMainCamera(std::vector<EffectDesc> & effectDescs)
    {
        Matrix4x4 camVP = m_main_camera->GetProjectionMatrix() * m_main_camera->GetViewMatrix();
        Frustum frustum(camVP);

        CamVisibleRenderers.clear();
        for(auto & desc : effectDescs)
        {
            CamVisibleRenderers.emplace_back(EffectDesc());
            auto & back = CamVisibleRenderers.back();
            back.pEffect = desc.pEffect;
            for(auto & renderer : desc.renderers)
            {
                if(frustum.IntersectAABB(renderer->GetBounds()))
                {
                    back.renderers.emplace_back(renderer);
                    back.renderEntities.emplace_back(renderer->GetRenderEntity());
                }
            }
        }
    }
    
    Matrix4x4 Scene::CalculateDirectionalLightCamera()
    {
        Matrix4x4 camVP = m_main_camera->GetProjectionMatrix() * m_main_camera->GetViewMatrix();
        Frustum frustum(camVP);

        Bounds frustum_bounds = frustum.GetBounds();
        
        Vector3 eye = frustum_bounds.GetCenter() + m_directional_light.m_buffer.m_direction * frustum_bounds.GetExtents().length();
        Vector3 center = frustum_bounds.GetCenter();
        Matrix4x4 view = Matrix4x4::LookAtLH(eye, center, Vector3::up);

        Bounds view_frustum_bounds = frustum_bounds.Transform(view);
        Bounds view_scene_bounds = scene_bounds.Transform(view);
        Matrix4x4 orth = Matrix4x4::OrthographicLH(
                        std::max(view_frustum_bounds.GetMin().x, view_scene_bounds.GetMin().x),
                        std::min(view_frustum_bounds.GetMax().x, view_scene_bounds.GetMax().x),
                        std::max(view_frustum_bounds.GetMin().y, view_scene_bounds.GetMin().y),
                        std::min(view_frustum_bounds.GetMax().y, view_scene_bounds.GetMax().y),
                        -view_scene_bounds.GetMax().z, // the objects which are nearer than the frustum bounding box may caster shadow as well
                        -std::max(view_frustum_bounds.GetMin().z, view_scene_bounds.GetMax().z));
        
        return orth * view;
    }
}