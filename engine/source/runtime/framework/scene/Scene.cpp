#include "Scene.h"
#include "Frustum.h"
#include "BVH.h"
#include "Texture.h"
#include "core/math/EngineMath.h"

namespace TinyEngine
{
    
    void Scene::Load()
    {
        m_loaded = true;
    }


    void Scene::Clear()
    {
        m_loaded = false;
        p_map_resource = nullptr;
        selectedCamera = nullptr;
        m_renderers.clear();
        selectedRenderers.clear();
        CamVisibleRenderers.clear();
        shadowCastRenderers.clear();
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
        if(m_directional_light.use_csm)
        {
            /**
             * @ref https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html---PSSM
             *      code: https://blog.csdn.net/qq_39300235/article/details/107765941
             *      https://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
             * 
             */
            //1. Splitting the View Frustum
            splitingViewFrustum(m_directional_light.m_csm.m_split_depth, 
                m_directional_light.m_csm.m_split_num, m_directional_light.m_csm.m_split_lambda);
            
            //2. Calculate the light's view-projection transformation matrix for each split part Vi 
            updateLightViewProjMatrix(m_directional_light.m_csm.m_split_view_proj, m_directional_light.m_csm.m_split_depth);
            
            m_directional_light.m_csm.m_split_visible_renderers.resize(m_directional_light.m_csm.m_split_num);
            //3. Calculate the visible objects for each split part Vi
            for(int i = 0 ; i < m_directional_light.m_csm.m_split_num; ++i)
            {
                m_directional_light.m_csm.m_split_visible_renderers[i].clear();
                Frustum frustum(m_directional_light.m_csm.m_split_view_proj[i]);
                for(auto & desc : ShadowCastDescs)
                {
                    for(auto & renderer : desc.renderers)
                    {
                        if(frustum.IntersectAABB(renderer->GetBounds()))
                        {
                            m_directional_light.m_csm.m_split_visible_renderers[i].emplace_back(renderer);
                        }
                    }
                }
            }
            auto & depth = m_directional_light.m_csm.m_split_depth;
            Vector4 fd = Vector4(depth[0].y, depth[1].y, depth[2].y, depth[3].y);
            // convert depth to ndc space
            float F = m_main_camera->GetFar();
            float N = m_main_camera->GetNear();
            float A = F / (F - N), B = -F * N / (F - N);
            for(int i = 0; i < 4; ++i) fd[i] = A + B / fd[i];
            m_directional_light.m_csm.m_far_planes = fd;
            
        }else{
            // get viewproject matrix
            m_directional_light.m_nsm.m_view_proj = CalculateDirectionalLightCamera();
            // get frustum from viewproject matrix

            m_directional_light.m_nsm.visibleRenderers.clear();

            Frustum frustum(m_directional_light.m_nsm.m_view_proj);
            for(auto & desc : ShadowCastDescs)
            {
                for(auto & renderer : desc.renderers)
                {
                    if(frustum.IntersectAABB(renderer->GetBounds()))
                    {
                        m_directional_light.m_nsm.visibleRenderers.emplace_back(renderer);
                    }
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
        auto scene_min = Vector3::max;
        auto scene_max = Vector3::min;
        for(auto & renderer : shadowCastRenderers)
        {
            auto bounds = renderer->GetBounds();
            scene_min = Vector3::Min(scene_min, bounds.GetMin());
            scene_max = Vector3::Max(scene_max, bounds.GetMax());
        }

        Bounds scene_bounds;
        scene_bounds.SetMinMax(scene_min, scene_max);

        Matrix4x4 ViewMatrix = m_main_camera->GetViewMatrix();
        Matrix4x4 ProjectionMatrix = m_main_camera->GetProjectionMatrix();

        Matrix4x4 camVP = ProjectionMatrix * ViewMatrix;
        Frustum cam_frustum(camVP);

        Bounds cam_frustum_bounds = cam_frustum.GetBounds();
        Vector3 eye = cam_frustum_bounds.GetCenter() - m_directional_light.m_buffer.m_direction * cam_frustum_bounds.GetExtents().length();
        Vector3 center = cam_frustum_bounds.GetCenter();
        Matrix4x4 view = Matrix4x4::LookAtLH(eye, center, Vector3::up);
        
        Bounds view_frustum_bounds = cam_frustum_bounds.Transform(view);
        Bounds view_scene_bounds = scene_bounds.Transform(view);
        Vector3 min_view_frustum = view_frustum_bounds.GetMin();
        Vector3 max_view_frustum = view_frustum_bounds.GetMax();
        Vector3 min_view_scene = view_scene_bounds.GetMin();
        Vector3 max_view_scene = view_scene_bounds.GetMax();
        Matrix4x4 orth = Matrix4x4::OrthographicLH(
                        std::max(min_view_frustum.x, min_view_scene.x),
                        std::min(max_view_frustum.x, max_view_scene.x),
                        std::max(min_view_frustum.y, min_view_scene.y),
                        std::min(max_view_frustum.y, max_view_scene.y),
                        min_view_scene.z, // the objects which are nearer than the frustum bounding box may caster shadow as well
                        std::min(max_view_frustum.z, max_view_scene.z));

        // Matrix4x4 orth = Matrix4x4::OrthographicLH(
        //                 std::max(view_frustum_bounds.GetMin().x, view_scene_bounds.GetMin().x),
        //                 std::min(view_frustum_bounds.GetMax().x, view_scene_bounds.GetMax().x),
        //                 std::max(view_frustum_bounds.GetMin().y, view_scene_bounds.GetMin().y),
        //                 std::min(view_frustum_bounds.GetMax().y, view_scene_bounds.GetMax().y),
        //                 view_scene_bounds.GetMin().z, // the objects which are nearer than the frustum bounding box may caster shadow as well
        //                 std::max(view_frustum_bounds.GetMax().z, view_scene_bounds.GetMax().z));
        
        return orth * view;
    }

    void Scene::splitingViewFrustum(std::vector<Vector2>& split_depth, int split_num, float lambda)
    {
        split_depth.resize(split_num);
        // we need calcaute for each split
        float cam_far = m_main_camera->GetFar();
        float cam_near = m_main_camera->GetNear();
        float ratio = cam_far / cam_near;

        split_depth[0].x = cam_near;
        for(int i = 1 ; i < split_num; ++i)
        {
            float si = (i + 1) / (float)split_num;
            
            // log scheme
            float log_split = cam_near * pow(ratio, si);
            // uniform scheme
            float uniform_split = cam_near + (cam_far - cam_near) * si;
            // pssm scheme
            float pssm_split = lambda * log_split + (1 - lambda) * uniform_split;
            
            split_depth[i - 1].y = pssm_split * 1.005f; // set far of last split
            split_depth[i].x = pssm_split;
        }
        
        split_depth[split_num - 1].y = cam_far;
    }

    
    void Scene::updateLightViewProjMatrix(std::vector<Matrix4x4>& split_view_projs, std::vector<Vector2>& split_depth)
    {
        int split_num = split_depth.size();

        float fov = m_main_camera->GetFOV() * kDeg2Rad;
        Vector3 eye = m_main_camera->pTransform->GetPosition();
        Vector3 forward = m_main_camera->pTransform->forward();
        Vector3 up = m_main_camera->pTransform->up();
        Vector3 right = m_main_camera->pTransform->right();

        split_view_projs.resize(split_num);
        Vector3 vertices[8];

        auto scene_min = Vector3::max;
        auto scene_max = Vector3::min;
        for(auto & renderer : shadowCastRenderers)
        {
            auto bounds = renderer->GetBounds();
            scene_min = Vector3::Min(scene_min, bounds.GetMin());
            scene_max = Vector3::Max(scene_max, bounds.GetMax());
        }

        Bounds scene_bounds;
        scene_bounds.SetMinMax(scene_min, scene_max);

        for(int i = 0; i < split_num; ++i)
        {
            float nd = split_depth[i].x;
            float fd = split_depth[i].y;

            Vector3 split_near_center = eye + forward * nd;
            Vector3 split_far_center = eye + forward * fd;

            float split_near_height = nd * tan(fov * 0.5f);
            float split_near_width = split_near_height * m_main_camera->GetAspect();
            float split_far_height = fd * tan(fov * 0.5f);
            float split_far_width = split_far_height * m_main_camera->GetAspect();

            // get eight corners of this split frustum
            vertices[0] = split_near_center - up * split_near_height - right * split_near_width; // near left bottom
            vertices[1] = split_near_center + up * split_near_height - right * split_near_width; // near left top
            vertices[2] = split_near_center + up * split_near_height + right * split_near_width; // near right top
            vertices[3] = split_near_center - up * split_near_height + right * split_near_width; // near right bottom
            vertices[4] = split_far_center - up * split_far_height - right * split_far_width; // far left bottom
            vertices[5] = split_far_center + up * split_far_height - right * split_far_width; // far left top
            vertices[6] = split_far_center + up * split_far_height + right * split_far_width; // far right top
            vertices[7] = split_far_center - up * split_far_height + right * split_far_width; // far right bottom

            // get bounding box of this split frustum
            Vector3 min{Vector3::max}, max{Vector3::min};
            for(int i = 0 ; i < 8; ++i)
            {
                min = Vector3::Min(min, vertices[i]);
                max = Vector3::Max(max, vertices[i]);
            }

            Bounds split_frustum_bounds;
            split_frustum_bounds.SetMinMax(min, max);

            Vector3 center = split_frustum_bounds.GetCenter();
            Vector3 light_pos = center - m_directional_light.m_buffer.m_direction * split_frustum_bounds.GetExtents().length();
            Matrix4x4 view = Matrix4x4::LookAtLH(light_pos, center, Vector3::up);

            Bounds view_frustum_bounds = split_frustum_bounds.Transform(view);
            Bounds view_scene_bounds = scene_bounds.Transform(view);
            Vector3 min_view_frustum = view_frustum_bounds.GetMin();
            Vector3 max_view_frustum = view_frustum_bounds.GetMax();
            Vector3 min_view_scene = view_scene_bounds.GetMin();
            Vector3 max_view_scene = view_scene_bounds.GetMax();
            Matrix4x4 orth = Matrix4x4::OrthographicLH(
                            std::max(min_view_frustum.x, min_view_scene.x),
                            std::min(max_view_frustum.x, max_view_scene.x),
                            std::max(min_view_frustum.y, min_view_scene.y),
                            std::min(max_view_frustum.y, max_view_scene.y),
                            min_view_scene.z, // the objects which are nearer than the frustum bounding box may caster shadow as well
                            std::min(max_view_frustum.z, max_view_scene.z));
        
            split_view_projs[i] = orth * view;
        }
    }
}