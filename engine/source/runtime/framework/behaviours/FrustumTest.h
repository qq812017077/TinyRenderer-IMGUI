#pragma
#include "behaviours/Behaviour.h"
#include "components/Camera.h"
#include "scene/Frustum.h"

class FrustumTest : public Behaviour
{
public:
    //use base constructor
    using Behaviour::Behaviour;
    float time = 0.0f;
    void Init() override
    {
        cam = GetGameObject()->GetComponent<Camera>();
        auto pCube = GameObject::Find("Big Cube");
        pCubeRenderer = pCube->GetComponent<Renderer>();

    }

    void OnUpdate(float deltaTime) override
    {
        auto frustum = TinyEngine::Frustum(cam->GetProjectionMatrix() * cam->GetViewMatrix());
        bool intersect = frustum.IntersectAABB(pCubeRenderer->GetBounds());

        std::cout << "intersect cube: " << intersect << std::endl;
    }
protected:
    Camera * cam;
    Renderer * pCubeRenderer;
    
};
