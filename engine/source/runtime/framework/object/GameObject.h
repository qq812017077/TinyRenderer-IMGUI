#pragma once
#include <iostream>
#include "components/Renderer.h"
#include "components/Transform.h"
#include "components/Component.h"
#include <string>
#include <memory>
#include <unordered_map>
#include "world/Level.h"

struct Model;
struct TinyEngine::GameObjectRes;
class GameObject
{
    friend class TinyEngine::Level;
    public:
        virtual ~GameObject();
        // Getters
        std::string GetName() const { return name; }
        size_t GetInstanceID() const { return instanceId; }
        // Setters
        void SetName(std::string name) { this->name = name; }

        inline Transform& transform() const { return *pTransform; }
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
            pCom = new T(std::forward<Args>(args)...);
            components[compName] = pCom;
            // auto & pComp = std::make_unique<T>(std::forward<Args>(args)...);
            // pCom = pComp.get();
            // components[compName] = std::move(pComp);
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
                delete components[compName];
                components.erase(compName);
            }
        }

        template <typename T>
        T* GetComponent()
        {
            auto compName = std::string(typeid(T).name());
            // if is Transform, return transform
            if(components.find(compName) == components.end()) return nullptr;
            // return static_cast<T*>(components[compName].get());
            return static_cast<T*>(components[compName]);
        }

        static GameObject* Find(const char* name);
        static GameObject* CreateGameObject(std::string name="GameObject");
        static GameObject* CreateFromFile(const char * filePath);
        static void Destroy(GameObject& gameObject);
        static void Destroy(GameObject* pGameObject);
        

    private:
        GameObject() = delete;
        // GameObject(std::string name);
        GameObject(size_t instanceId);
        GameObject(const GameObject&) = delete;
        //move constructor
        GameObject(GameObject&& other) noexcept = default;
        GameObject& operator=(const GameObject&) = delete;
        bool loadres(const TinyEngine::GameObjectRes& go_instance_res);

        static GameObject* ParseModel(Model & model);
        
    private:
        Transform* pTransform;
        size_t instanceId   = 0;
        bool initialized = false;
        bool isActived{true};
        std::string name = "New GameObject";
        std::unordered_map<std::string, Component*> components;
        // std::unordered_map<std::string, std::unique_ptr<Component>> components;
        std::vector<Component*> componentsToInit;
        
};