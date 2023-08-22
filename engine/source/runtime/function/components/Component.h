#pragma once

class GameObject;
class Component
{
    friend class GameObject;
public:
    Component() ;
    Component(const Component&) = delete;
    virtual ~Component();
    
    GameObject& GetOwner() const;
protected:
    virtual void Init() = 0;
    void SetOwner(GameObject* owner);
    virtual void OnPreUpdate() {};
    virtual void OnUpdate(float deltaTime) {};
    virtual void OnLateUpdate(float deltaTime) {};
    GameObject* owner = nullptr;
private:
};