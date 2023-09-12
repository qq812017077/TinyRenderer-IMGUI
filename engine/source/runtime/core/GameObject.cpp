#include "GameObject.h"
#include "components/Component.h"
#include "geometry/Model.h"
std::vector<std::unique_ptr<GameObject>> GameObject::pGameObjects;
std::unordered_map<std::string, std::vector<int>> GameObject::goIdsByName;

GameObject::GameObject(std::string name):
    name(name),
    transform()
{
    componentsToInit.push_back(&transform);
}



GameObject::~GameObject()
{
    RemoveAllComponents();
    // remove go from gameobjects
    if(goIdsByName.find(name) != goIdsByName.end())
    {
        auto & goIds = goIdsByName[name];
        // find this go
        int index = 0;
        while(index < goIds.size() && pGameObjects[goIds[index]].get() != this) index++;
        if(index < goIds.size())
        {
            //remove from pGameObjects
            goIds.erase(goIds.begin() + index);
            //remove from goIdsByName
            goIdsByName[name].erase(goIdsByName[name].begin() + index);
        }
        return;
    }else
    {
        std::cout << "GameObject[" << name << "] not found in goIdsByName" << std::endl;
    }
    
    //display gameobject address
    std::cout << "GameObject[" << name << "] destroyed at " << this << std::endl;
}


void GameObject::Init()
{
    // init all components
    if(!initialized)
    {
        initialized = true;
    }

    for(auto& pComp : componentsToInit)
    {
        if(pComp == nullptr) continue;
        pComp->Init();
    }
    componentsToInit.clear();
}
bool GameObject::IsInitialized() const 
{
    return initialized && componentsToInit.size() == 0;
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
    auto & goids = goIdsByName[name];
    if(goids.size() > 0)
        return pGameObjects[goids[0]].get();
    return nullptr;
}

GameObject* GameObject::CreateGameObject(std::string name)
{
    // make unique_ptr
    int idx = static_cast<int>(pGameObjects.size());
    pGameObjects.emplace_back(std::make_unique<GameObject>(name));
    // add go to gameobjects
    goIdsByName[name].emplace_back(idx);
    return pGameObjects.back().get();
}
GameObject* GameObject::CreateFromFile(const char * filePath)
{
    // make unique_ptr
    std::string name = filePath;
    size_t start = name.find_last_of('/');
    start = start == std::string::npos ? 0 : start+1;
    size_t end = name.find_last_of('.');
    end = end == std::string::npos ? name.size() : end;
    name = name.substr(start, end-start);
    
    // load mesh from file
    Model model;
    bool success = Model::Load(filePath, model);
    if(success)
    {
        // display model info
        std::cout << "Model[" << name << "] loaded from " << filePath << std::endl;
        std::cout << "Model[" << name << "] has " << model.meshdatas.size() << " meshes" << std::endl;
        std::cout << "Model[" << name << "] has " << model.matdatas.size() << " materials" << std::endl;

        // TODO: 
        // make unique_ptr
        // create model based model
         
        // int idx = static_cast<int>(pGameObjects.size());
        // auto & pGO = std::make_unique<GameObject>(name);
        // pGO->AddComponent<Renderer>(mesh);

        // pGameObjects.emplace_back(std::move(pGO));
        // // add go to gameobjects
        // goIdsByName[name].emplace_back(idx);
        // return pGameObjects.back().get();
        return nullptr;
    }else
    {
        std::cout << "Failed to load model from " << filePath << std::endl;
        return nullptr;
    }
    
}
void GameObject::RemoveAllGameObjects()
{
    // clear pGameObjects
    for(auto& go : pGameObjects)
    {
        go.release();
    }
    pGameObjects.clear();
    goIdsByName.clear();
}