#pragma
#include "behaviours/Behaviour.h"
#include "components/Camera.h"
#include "scene/Frustum.h"
#include "object/GameObject.h"
#include "framework/world/WorldManager.h"
#include "ui/GUI.h"

class FrustumTest : public Behaviour
{
public:
    //use base constructor
    using Behaviour::Behaviour;
    float time = 0.0f;
    void Init() override
    {
        orthCam = GameObject::CreateGameObject("orth cam")->AddComponent<Camera>();
        orthCam->orth = true;
        orthCam->SetOrthSize(orthSize);
        orthCam->SetNear(0.1f);
        orthCam->SetAspect(1280.0f / 720.0f);
        orthCam->SetFar(40.0f);
        orthCam->pTransform->SetPosition({ 0.0f, 0.0f, -10.0f });
        orthCam->pTransform->SetEulerAngle({ 0.0f, 0.0f, 0.0f });

        cam = GetGameObject()->GetComponent<Camera>();
        // cam->pTransform->SetPosition(Vector3(25.0f, 0.0f, 0.0f));
        cam->pTransform->SetPosition({ 0.0f, 0.0f, -10.0f });
        cam->pTransform->SetEulerAngle({ 0.0f, 0.0f, 0.0f });
        cam->SetNear(0.1f);
        cam->SetAspect(1280.0f / 720.0f);
        cam->SetFOV(60.0f);
        eyeCube = Primitive::CreateCube("eye cube");
        centerCube = Primitive::CreateCube("center cube");
        // eyeCube->transform().SetScale(Vector3(0.1f, 0.1f, 0.1f));
        // centerCube->transform().SetScale(Vector3(0.1f, 0.1f, 0.1f));
        eyeCube->GetComponent<Renderer>()->GetMaterial()->SetColor("albedo", Color::Red());
        centerCube->GetComponent<Renderer>()->GetMaterial()->SetColor("albedo", Color::Green());
        for(int i = 0; i < 8; i++)
        {
            pCube[i] = Primitive::CreateCube("cube "+ std::to_string(i));
            pCube[i]->GetComponent<Renderer>()->GetMaterial()->SetColor("albedo", Color::Blue());
            pCube[i]->transform().SetScale(Vector3(0.5f, 0.5f, 0.5f));
        }

        auto gos = TinyEngine::WorldManager::Get().GetCurrentActiveLevel()->GetAllGameObjects();
        for(auto & go : gos)
        {
            auto renderer = go.second->GetComponent<Renderer>();
            if(!renderer) continue;
            m_renderers.push_back(renderer);
        }
    }

    void OnUpdate(float deltaTime) override
    {
        orthCam->SetOrthSize(orthSize);
        cam->SetFar(farPlane);
        auto scene_min = Vector3::max;
        auto scene_max = Vector3::min;
        for(auto & renderer : m_renderers)
        {
            auto bounds = renderer->GetBounds();
            scene_min = Vector3::Min(scene_min, bounds.GetMin());
            scene_max = Vector3::Max(scene_max, bounds.GetMax());
        }

        Bounds scene_bounds;
        scene_bounds.SetMinMax(scene_min, scene_max);
        Matrix4x4 ViewMatrix = cam->GetViewMatrix();
        Matrix4x4 ProjectionMatrix = cam->GetProjectionMatrix();
        Matrix4x4 camVP = ProjectionMatrix * ViewMatrix;
        TinyEngine::Frustum frustum(camVP);

        auto cam_frustum_bounds = frustum.GetBounds();
        // cam_frustum_bounds.GetExtents().printVec3();
        auto light = Light::GetDirectionalLight();
        
        Vector3 center = cam_frustum_bounds.GetCenter();
        Vector3 eye = center - light->pTransform->forward() * cam_frustum_bounds.GetExtents().length();
        Matrix4x4 view = Matrix4x4::LookAtLH(eye, center, Vector3::up);
        orthCam->pTransform->SetPosition(eye);
        orthCam->pTransform->SetEulerAngle(light->pTransform->GetEulerAngle());
        
        eyeCube->transform().SetPosition(eye);
        centerCube->transform().SetPosition(center);
        Bounds view_frustum_bounds = cam_frustum_bounds.Transform(view);
        Bounds view_scene_bounds = scene_bounds.Transform(view);
        Vector3 min_view_frustum = view_frustum_bounds.GetMin();
        Vector3 max_view_frustum = view_frustum_bounds.GetMax();
        Vector3 min_view_scene = view_scene_bounds.GetMin();
        Vector3 max_view_scene = view_scene_bounds.GetMax();
        orthCam->SetOrthSize( std::min(max_view_frustum.y, max_view_scene.y) - std::max(min_view_frustum.y, min_view_scene.y));
        orthCam->SetFar(std::min(max_view_frustum.z, max_view_scene.z));
        orthCam->SetNear(min_view_scene.z);
        Matrix4x4 orth = Matrix4x4::OrthographicLH(
                        std::max(min_view_frustum.x, min_view_scene.x),
                        std::min(max_view_frustum.x, max_view_scene.x),
                        std::max(min_view_frustum.y, min_view_scene.y),
                        std::min(max_view_frustum.y, max_view_scene.y),
                        min_view_scene.z, // the objects which are nearer than the frustum bounding box may caster shadow as well
                        std::min(max_view_frustum.z, max_view_scene.z));

        Matrix4x4 viewProj = orth * view;
        TinyEngine::Frustum f_frustum = TinyEngine::Frustum(viewProj);
        f_frustum.GetBounds().GetCorners(positions);
        for(int i = 0; i < 8; i++)
        {
            pCube[i]->transform().SetPosition(positions[i]);
        }
        int count{};
        for(auto & renderer : m_renderers)
        {
            if(f_frustum.IntersectAABB(renderer->GetBounds()))
            {
                count++;
            }
        }

        std::cout << "count: " << count << std::endl;
    }

    void OnGUI() override
    {
        GUI::Begin("Came Test");
        GUI::DragFloat("far plane", &farPlane, 0.1f, 10.0f, 100.0f);

        GUI::DragFloat("orth size", &orthSize, 0.1f, 0.1f, 100.0f);
        GUI::End();
    }
protected:
    Camera * cam;
    Camera * orthCam;
    GameObject * pCube[8];
    Vector3 positions[8];

    GameObject * eyeCube;
    GameObject * centerCube;
    std::vector<Renderer *> m_renderers;
    float farPlane=10.0f;
    float orthSize = 50.0f;
};
