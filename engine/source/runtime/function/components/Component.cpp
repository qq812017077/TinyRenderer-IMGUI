#include "components/Component.h"
#include "core/GameObject.h"

Component::Component()
{
}

Component::~Component()
{
    owner = nullptr;
}

GameObject& Component::GetOwner() const
{
    if(owner == nullptr)
    {
        throw std::runtime_error("Component has no owner");
    }
    
    return *owner;
}

void Component::SetOwner(GameObject* owner)
{
    this->owner = owner;
}

