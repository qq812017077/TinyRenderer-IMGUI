#pragma
#include "core/GameObject.h"

class Rotater : public Component
{
private:
    //use base constructor
    using Component::Component;
public:
    void Init()
    {
    }

    void OnUpdate(float deltaTime) override
    {
        owner->transform.Rotate({ 0.0f, 0.0f, 1.0f }, deltaTime * 15.f);
        owner->transform.Rotate({ 0.0f, 1.0f, 0.0f }, deltaTime * 15.f);
        owner->transform.Rotate({ 1.0f, 0.0f, 0.0f }, deltaTime * 15.f);
    }
};
