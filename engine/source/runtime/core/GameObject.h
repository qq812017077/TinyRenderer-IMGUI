#pragma once
#include <iostream>
#include "components/Renderer.h"
#include "components/Transform.h"
#include "components/Component.h"
#include <string>
#include <memory>
#include <unordered_map>

struct Model;
class GameObject
{
public:
    GameObject() = delete;
    GameObject(std::string name);
    virtual ~GameObject();
    
    
    // Getters
    std::string GetName() const { return name; }
    // Setters
    void SetName(std::string name) { this->name = name; }

    void Init();
    bool IsInitialized() const;
    bool IsActived() const { return isActived; }
    void SetActive(bool active) { isActived = active; }
    void OnPreUpdate();
    void OnUpdate(float deltaTime);
    void OnLateUpdate(float deltaTime);
    void OnGUI();
    // Components
    void RemoveAllComponents();
    template <typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        auto compName = std::string(typeid(T).name());
        auto pCom = GetComponent<T>();
        if(pCom != nullptr) return pCom;

        static_assert (std::is_base_of<Component, T>::value, "type parameter of this class must derive from Component");
        auto & pComp = std::make_unique<T>(std::forward<Args>(args)...);
        pCom = pComp.get();
        components[compName] = std::move(pComp);
        pCom->SetGameObject(this);
        componentsToInit.push_back(pCom);
        return pCom;
    }
    
    template <typename T>
    void RemoveComponent()
    {
        auto compName = std::string(typeid(T).name());
        if(GetComponent<T>() == nullptr) return ;
        // if components has this component, remove it
        if(components.find(compName) != components.end())
        {
            components.erase(compName);
        }
    }

    template <typename T>
    T* GetComponent()
    {
        auto compName = std::string(typeid(T).name());
        if(components.find(compName) == components.end()) return nullptr;
        return static_cast<T*>(components[compName].get());
    }


    static const std::vector<std::unique_ptr<GameObject>>& GetRootGameObjects() { return pGameObjects;}
    static GameObject* Find(const char* name);
    static GameObject* CreateGameObject(std::string name="GameObject");
    static GameObject* CreateFromFile(const char * filePath);
    static void RemoveAllGameObjects();
    static void Destroy(GameObject& gameObject);
    static void Destroy(GameObject* pGameObject);
    static void RefreshQueue();
    
    Transform transform;
private:
    GameObject(const GameObject&) = delete;
    //move constructor
    GameObject(GameObject&& other) noexcept = default;
    GameObject& operator=(const GameObject&) = delete;
    static GameObject* ParseModel(Model & model);

    friend class App;
    bool initialized = false;
    bool isActived{true};
    std::string name = "New GameObject";
    std::unordered_map<std::string, std::unique_ptr<Component>> components;
    std::vector<Component*> componentsToInit;
    
    static std::vector<GameObject*> pRootGameObjects;
    static std::vector<std::unique_ptr<GameObject>> pGameObjects;
    static std::unordered_map<std::string, std::vector<int>> goIdsByName;

    static std::vector<GameObject*> pGameObjectsToDestroy;
    static std::vector<std::unique_ptr<GameObject>> pGameObjectsToCreate;
};