#include "GameObject.h"
#include "components/Component.h"
#include "geometry/Model.h"
std::vector<GameObject*> GameObject::pRootGameObjects;
std::vector<std::unique_ptr<GameObject>> GameObject::pGameObjects;
std::unordered_map<std::string, std::vector<int>> GameObject::goIdsByName;
std::vector<std::unique_ptr<GameObject>> GameObject::pGameObjectsToCreate;
std::vector<GameObject*> GameObject::pGameObjectsToDestroy;
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
    if (!isActived) return;
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
    components.clear();
}

/**********************************************************************************************/
/*****************************************Static Methods****************************************/
/**********************************************************************************************/
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
    pGameObjectsToCreate.emplace_back(std::make_unique<GameObject>(name));
    return pGameObjectsToCreate.back().get();
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
void GameObject::RemoveAllGameObjects()
{
    // clear pGameObjects
    for(auto& go : pGameObjects)
    {
        go.release();
    }
    pRootGameObjects.clear();
    pGameObjects.clear();
    goIdsByName.clear();
}
void GameObject::Destroy(GameObject& gameObject)
{
    //if go has children, destroy them too, recursively
    if(gameObject.transform.GetChildCount() > 0)
    {
        for(int i = 0; i < gameObject.transform.GetChildCount(); i++)
        {
            Destroy(gameObject.transform.GetChild(i)->GetGameObject());
        }
    }
    // add to destroy list
    pGameObjectsToDestroy.emplace_back(&gameObject);
}
void GameObject::Destroy(GameObject* pGameObject)
{
    Destroy(*pGameObject);
}

void GameObject::RefreshQueue()
{
    for(auto& go : pGameObjectsToCreate)
    {
        if(go == nullptr) continue;
        // add go to gameobjects
        int idx = static_cast<int>(pGameObjects.size());
        pGameObjects.emplace_back(std::move(go));
        goIdsByName[pGameObjects.back()->name].emplace_back(idx);
    }
    // destroy all gameobjects in destroy list
    for(auto& go : pGameObjectsToDestroy)
    {
        if(go == nullptr) continue;
        // find this go, and remove it from pGameObjects and goIdsByName
        int index = 0;
        while(index < pGameObjects.size() && pGameObjects[index].get() != go) index++;
        if(index < pGameObjects.size())
        {
            //remove from pGameObjects
            pGameObjects[index].release();
            pGameObjects.erase(pGameObjects.begin() + index);
            
            auto & goIds = goIdsByName[go->name];
            // find this go
            int _index = 0;
            while(_index < goIds.size() && pGameObjects[goIds[_index]].get() != go) _index++;
            if(_index < goIds.size())
            {
                //remove from pGameObjects
                goIds.erase(goIds.begin() + _index);
                //remove from goIdsByName
                goIdsByName[go->name].erase(goIdsByName[go->name].begin() + _index);
            }else
            {
                std::cout << "GameObject[" << go->name << "] not found in goIdsByName" << std::endl;
            }
        }
    }
    pGameObjectsToDestroy.clear();
}

void ParseNode(Model& model, Model::Node* pNode, Transform & parent)
{
    // create gameobject
    auto pGo = GameObject::CreateGameObject(pNode->name);
    // set parent
    pGo->transform.SetParent(parent);

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
        // auto matdata = model.materialdatas[model.meshdatas[idx].m_MaterialIndex];
        //TODO: set material properties
    }

    // add children
    for(auto& child : pNode->children)
    {
        ParseNode(model, child.get(), pGo->transform);
    }
}
GameObject* GameObject::ParseModel(Model & model)
{
    auto root = model.pRoot.get();
    if(root == nullptr) return nullptr;
    // create gameobject
    auto pGo = CreateGameObject(root->name);
    if(root->meshIndices.size() > 0)
    {
        // TODO: here we only use the first mesh, need to support multiple meshes.
        int idx = root->meshIndices[0];
        auto pRenderer = pGo->AddComponent<Renderer>(model.meshdatas[idx].mesh);
        // auto matdata = model.materialdatas[model.meshdatas[idx].m_MaterialIndex];
        //TODO: set material properties
    }

    // add children
    for(auto& child : root->children)
    {
        ParseNode(model, child.get(), pGo->transform);
    }
    return pGo;
}