#pragma
#include "behaviours/Behaviour.h"

class Rotater : public Behaviour
{
public:
    //use base constructor
    using Behaviour::Behaviour;
    float time = 0.0f;
    void Init() override
    {
        pMaterial = owner->GetComponent<Renderer>()->GetMaterialPtr();
        time = 0.0f;
    }

    void OnUpdate(float deltaTime) override
    {
        time += deltaTime;
        float sinValue = sinf(time);
        if(pMaterial != nullptr)
            pMaterial->SetFloat("offset", sinValue * 0.1f);
        // owner->transform.Rotate(Vector3(0.0f, 0.0f, 1.0f) * deltaTime * 15.f);
        owner->transform.Rotate(Vector3(0.0f, 1.0f, 0.0f) * deltaTime * 15.f);
        // owner->transform.Rotate(Vector3(1.0f, 0.0f, 0.0f) * deltaTime * 15.f);
    }
protected:
    using Behaviour::owner;
    Material * pMaterial;
};
