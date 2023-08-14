#include "GameObject.h"
#include "components/Component.h"

GameObject::GameObject():   
    pRenderer(nullptr),
    transform()
{
}

GameObject::GameObject(std::string name):
    name(name),
    pRenderer(nullptr),
    transform()
{
}


// GameObject::GameObject(GameObject&& other) noexcept
// {
//     name = std::move(other.name);
//     pRenderer = std::move(other.pRenderer);
//     transform = std::move(other.transform);
// }

GameObject::~GameObject()
{
    RemoveAllComponents();
    //display gameobject address
    std::cout << "GameObject[" << name << "] destroyed at " << this << std::endl;
}

void GameObject::Init()
{
    
}


std::string GameObject::GetName() const
{
    return name;
}


// bool GameObject::HasRenderer() const
// {
//     return pRenderer != nullptr;
// }

// void GameObject::SetRenderer(std::unique_ptr<Renderer> pRenderer)
// {
//     if(HasRenderer())
//     {
//         //throw exception
//         throw std::runtime_error("GameObject already has a renderer");
//         return ;
//     }
//     this->pRenderer = std::move(pRenderer);
// }

// void GameObject::RemoveRenderer()
// {
//     if(!HasRenderer()) return ;
//     pRenderer = nullptr;
// }

// Renderer& GameObject::GetRenderer() const
// {
//     if(!HasRenderer())
//     {
//         //throw exception
//         throw std::runtime_error("GameObject does not have a renderer");
//     }
//     return *pRenderer;
// }

void GameObject::OnUpdate(float deltaTime)
{
    
}

// Components
void GameObject::RemoveAllComponents()
{
    components.clear();
}
