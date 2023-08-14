#pragma once
#include <iostream>
#include "components/Renderer.h"
#include "components/Transform.h"
#include <string>
#include <memory>


class GameObject
{
public:
    GameObject();
    GameObject(std::string name);
    GameObject(const GameObject&) = delete;
    //move constructor
    GameObject(GameObject&& other) noexcept = delete;
    virtual ~GameObject();

    virtual void Init();
    // Getters
    std::string GetName() const;

    // Setters
    void SetName(std::string name);
    // bool HasRenderer() const;
    // void RemoveRenderer();
    // Renderer& GetRenderer() const;
    virtual void OnUpdate(float deltaTime);

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
        pCom->SetOwner(this);
        return pCom;
    }
    
    template <typename T>
    void RemoveComponent()
    {
        auto compName = std::string(typeid(T).name());
        if(GetComponent<T>() == nullptr) return ;
        components.erase(compName);
    }

    template <typename T>
    T* GetComponent()
    {
        auto compName = std::string(typeid(T).name());
        if(components.find(compName) == components.end()) return nullptr;
        return static_cast<T*>(components[compName].get());
    }

    Transform transform;
protected:
    void SetRenderer(std::unique_ptr<Renderer> pRenderer);
    GameObject& operator=(const GameObject&) = delete;

private:
    std::string name = "New GameObject";
    std::unique_ptr<Renderer> pRenderer;
    std::unordered_map<std::string, std::unique_ptr<Component>> components;
};