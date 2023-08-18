#include "GameObject.h"
#include "components/Component.h"

GameObject::GameObject():
    transform()
{
}

GameObject::GameObject(std::string name):
    name(name),
    transform()
{
}



GameObject::~GameObject()
{
    RemoveAllComponents();
    //display gameobject address
    std::cout << "GameObject[" << name << "] destroyed at " << this << std::endl;
}

void GameObject::Init()
{
    // init all components
    transform.Init();

    for(auto& comp : components)
    {
        comp.second->Init();
    }
}


std::string GameObject::GetName() const
{
    return name;
}



void GameObject::OnUpdate(float deltaTime)
{
    // update all components
    for(auto& comp : components)
    {
        comp.second->OnUpdate(deltaTime);
    }
}

// Components
void GameObject::RemoveAllComponents()
{
    components.clear();
}
