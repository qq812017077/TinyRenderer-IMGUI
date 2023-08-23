#pragma
#include "behaviours/Behaviour.h"

class Rotater : public Behaviour
{
public:
    //use base constructor
    using Behaviour::Behaviour;
    void Init() override
    {
    }

    void OnUpdate(float deltaTime) override
    {
        owner->transform.Rotate(Vector3(0.0f, 0.0f, 1.0f) * deltaTime * 15.f);
        owner->transform.Rotate(Vector3(0.0f, 1.0f, 0.0f) * deltaTime * 15.f);
        owner->transform.Rotate(Vector3(1.0f, 0.0f, 0.0f) * deltaTime * 15.f);
    }
protected:
    using Behaviour::owner;
};
