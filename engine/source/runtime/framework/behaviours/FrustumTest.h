#pragma
#include "behaviours/Behaviour.h"
#include "components/Camera.h"
#include "scene/Frustum.h"
#include "object/GameObject.h"

class FrustumTest : public Behaviour
{
public:
    //use base constructor
    using Behaviour::Behaviour;
    float time = 0.0f;
    void Init() override
    {
        cam = GetGameObject()->GetComponent<Camera>();
        cam->pTransform->SetPosition(Vector3(25.0f, 0.0f, 0.0f));
        eyeCube = Primitive::CreateCube("eye cube");
        centerCube = Primitive::CreateCube("center cube");
        // eyeCube->transform().SetScale(Vector3(0.1f, 0.1f, 0.1f));
        // centerCube->transform().SetScale(Vector3(0.1f, 0.1f, 0.1f));
        eyeCube->GetComponent<Renderer>()->GetMaterial()->SetColor("albedo", Color::Red());
        centerCube->GetComponent<Renderer>()->GetMaterial()->SetColor("albedo", Color::Green());
        // for(int i = 0; i < 8; i++)
        // {
        //     pCube[i] = Primitive::CreateCube("cube "+ std::to_string(i));
        //     pCube[i]->transform().SetScale(Vector3(0.1f, 0.1f, 0.1f));
        // }
    }

    void OnUpdate(float deltaTime) override
    {
        auto frustum = TinyEngine::Frustum(cam->GetProjectionMatrix() * cam->GetViewMatrix());

        auto bounds = frustum.GetBounds();
        bounds.GetVertices(positions);
        auto light = Light::GetDirectionalLight();
        
        // Vector3 eye = bounds.GetCenter() - light->pTransform->forward() * bounds.GetExtents().length();
        // Vector3 center = bounds.GetCenter();
        // Matrix4x4 view = Matrix4x4::LookAtLH(eye, center, Vector3::up);
        // eyeCube->transform().SetPosition(eye);
        // centerCube->transform().SetPosition(center);
        // Bounds view_frustum_bounds = bounds.Transform(view);
        // Bounds view_scene_bounds = bounds.Transform(view);
        // Matrix4x4 orth = Matrix4x4::OrthographicLH(
        //                 std::max(view_frustum_bounds.GetMin().x, view_scene_bounds.GetMin().x),
        //                 std::min(view_frustum_bounds.GetMax().x, view_scene_bounds.GetMax().x),
        //                 std::max(view_frustum_bounds.GetMin().y, view_scene_bounds.GetMin().y),
        //                 std::min(view_frustum_bounds.GetMax().y, view_scene_bounds.GetMax().y),
        //                 -view_scene_bounds.GetMax().z, // the objects which are nearer than the frustum bounding box may caster shadow as well
        //                 -std::max(view_frustum_bounds.GetMin().z, view_scene_bounds.GetMax().z));

        // for(int i = 0; i < 8; i++)
        // {
        //     pCube[i]->transform().SetPosition(positions[i]);
        // }
    }
protected:
    Camera * cam;
    GameObject * pCube[8];
    Vector3 positions[8];

    GameObject * eyeCube;
    GameObject * centerCube;
};
