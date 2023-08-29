#include "GameObject.h"
#include "components/Component.h"
std::unordered_map<std::string, std::vector<GameObject*>> GameObject::gameObjectsByName;
GameObject::GameObject():
    GameObject("GameObject")
{
}

GameObject::GameObject(std::string name):
    name(name),
    transform()
{
    // add go to gameobjects
    gameObjectsByName[name].emplace_back(this);
}



GameObject::~GameObject()
{
    RemoveAllComponents();
    // remove go from gameobjects
    auto & goVec = gameObjectsByName[name];
    //remove from vector
    goVec.erase(std::remove(goVec.begin(), goVec.end(), this), goVec.end());
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


void GameObject::OnPreUpdate()
{
    // update all components
    for(auto& comp : components)
    {
        if(comp.second == nullptr)
            continue;
        comp.second->OnPreUpdate();
    }
}
void GameObject::OnUpdate(float deltaTime)
{
    // update all components
    for(auto& comp : components)
    {
        if(comp.second == nullptr)
            continue;
        comp.second->OnUpdate(deltaTime);
    }
}

void GameObject::OnLateUpdate(float deltaTime)
{
    // update all components
    for(auto& comp : components)
    {
        if(comp.second == nullptr)
            continue;
        comp.second->OnLateUpdate(deltaTime);
    }
}

void GameObject::OnGUI()
{
    // update all components
    for(auto& comp : components)
    {
        if(comp.second == nullptr)
            continue;
        comp.second->OnGUI();
    }
}

// Components
void GameObject::RemoveAllComponents()
{
    components.clear();
}

GameObject* GameObject::Find(const char* name)
{
    auto & gos = gameObjectsByName[name];
    if(gos.size() > 0)
        return gos[0];
    return nullptr;
}