#include "components/Component.h"
#include "object/GameObject.h"

Component::Component()
{
}

Component::~Component()
{
    pGameObject = nullptr;
}

GameObject* Component::GetGameObject() const
{
    if(pGameObject == nullptr) throw std::runtime_error("Component has no owner");
    return pGameObject;
}

void Component::SetGameObject(GameObject* owner)
{
    this->pGameObject = owner;
    this->pTransform = &(owner->transform());
}

