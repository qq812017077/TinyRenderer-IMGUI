#include "GameObject.h"
#include "components/Component.h"
#include "geometry/Model.h"
#include "world/WorldManager.h"
#include "res_type/common.h"
#include "Material.h"

// GameObject::GameObject(std::string name):
//     name(name),
//     transform()
// {
//     componentsToInit.push_back(&transform);
// }

GameObject::GameObject(size_t instanceId):
    instanceId(instanceId)
{
    pTransform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
    RemoveAllComponents();
#if DEBUG
    // std::cout << "GameObject[" << name << "] destroyed at " << this << std::endl;
#endif
}


void GameObject::Init()
{
    // init all components
    if(!initialized) initialized = true;

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
    if (!isActived) return;
    // update all components
    std::vector<Component*> comps;
    for(auto& comp : components)
    {
        if(comp.second == nullptr)
            continue;
        comp.second->OnPreUpdate();
    }
}

void GameObject::OnUpdate(float deltaTime)
{
    if (!isActived) return;
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
    if (!isActived) return;
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
    if (!isActived) return;
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
    for(auto& comp : components)
    {
        if(comp.second == nullptr)
            continue;
        delete comp.second;
    }
    components.clear();
}

/**********************************************************************************************/
/*****************************************Static Methods****************************************/
/**********************************************************************************************/
GameObject* GameObject::Find(const char* name)
{
    auto pLevel = TinyEngine::WorldManager::Get().GetCurrentActiveLevel();
    if(pLevel == nullptr) 
    {
        std::cout << "Failed to create gameobject[" << name << "], level is null" << std::endl;
        return nullptr;
    }
    return pLevel->Find(name);
}

GameObject* GameObject::CreateGameObject(std::string name)
{
    auto pLevel = TinyEngine::WorldManager::Get().GetCurrentActiveLevel();
    if(pLevel == nullptr) 
    {
        std::cout << "Failed to create gameobject[" << name << "], level is null" << std::endl;
        return nullptr;
    }
    return pLevel->CreateGameObject(name);
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
        // std::cout << "Model[" << name << "] loaded from " << filePath << std::endl;
        // std::cout << "Model[" << name << "] has " << model.meshdatas.size() << " meshes" << std::endl;
        // std::cout << "Model[" << name << "] has " << model.materialdatas.size() << " materials" << std::endl;
        return ParseModel(model);
    }else
    {
        std::cout << "Failed to load model from " << filePath << std::endl;
        return nullptr;
    }
    
}

void GameObject::Destroy(GameObject& gameObject)
{
    
    auto pLevel = TinyEngine::WorldManager::Get().GetCurrentActiveLevel();
    if(pLevel == nullptr) 
    {
        std::cout << "Failed to destroy gameobject[" << gameObject.name << "], level is null" << std::endl;
        return;
    }
    pLevel->DestroyGameObject(&gameObject);
}
void GameObject::Destroy(GameObject* pGameObject)
{
    auto pLevel = TinyEngine::WorldManager::Get().GetCurrentActiveLevel();
    if(pLevel == nullptr) 
    {
        std::cout << "Failed to destroy gameobject[" << pGameObject->name << "], level is null" << std::endl;
        return;
    }
    pLevel->DestroyGameObject(pGameObject);
}


bool GameObject::loadres(const TinyEngine::GameObjectRes& go_instance_res)
{
    name = go_instance_res.m_name;

    // load transform component
    *pTransform = go_instance_res.m_transform;
    // load other components
    for(auto& comp_res : go_instance_res.m_components)
    {
        throw std::runtime_error("not implemented");
        // auto pComp = Component::CreateComponent(comp_res);
        // if(pComp == nullptr) continue;
        // pComp->SetGameObject(this);
        // componentsToInit.push_back(pComp);
    }
    return true;
}

void ParseNode(Model& model, std::vector<std::shared_ptr<Material>>& materials, Model::Node* pNode, Transform & parent)
{
    // create gameobject
    auto pGo = GameObject::CreateGameObject(pNode->name);
    // set parent
    pGo->transform().SetParent(parent);

    // TODO: set transform
    // pGo->transform.SetPosition(node.transform.GetPosition());
    // pGo->transform.SetEulerAngle(node.transform.GetEulerAngle());
    // pGo->transform.SetScale(node.transform.GetScale());

    if(pNode->meshIndices.size() > 0)
    {
        // TODO: here we only use the first mesh, need to support multiple meshes.
        int idx = pNode->meshIndices[0];
        auto pRenderer = pGo->AddComponent<Renderer>(model.meshdatas[idx].mesh);
        pRenderer->SetMesh(model.meshdatas[idx].mesh);
        pRenderer->SetSharedMaterial(materials[model.meshdatas[idx].m_MaterialIndex]);
    }

    // add children
    for(auto& child : pNode->children)
    {
        ParseNode(model, materials, child.get(), pGo->transform());
    }
}
GameObject* GameObject::ParseModel(Model & model)
{
    auto root = model.pRoot.get();
    if(root == nullptr) return nullptr;
    // create gameobject
    auto pGo = CreateGameObject(root->name);
    // we need to generate all needed textures and materials and set values for them. then set them to
    std::vector<std::shared_ptr<Material>>& materials = model.GenerateMaterials();
    if(root->meshIndices.size() > 0)
    {
        // TODO: here we only use the first mesh, need to support multiple meshes.
        int idx = root->meshIndices[0];
        auto pRenderer = pGo->AddComponent<Renderer>(model.meshdatas[idx].mesh);
        pRenderer->SetSharedMaterial(materials[model.meshdatas[idx].m_MaterialIndex]);
    }
    // add children
    for(auto& child : root->children)
    {
        ParseNode(model, materials, child.get(), *(pGo->pTransform));
    }
    return pGo;
}