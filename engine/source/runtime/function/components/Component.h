#pragma once

class GameObject;
class Transform;
class Component
{
    friend class GameObject;
public:
    Component() ;
    Component(const Component&) = delete;
    virtual ~Component();
    
    GameObject& GetOwner() const;
    Transform* pTransform = nullptr;
protected:
    virtual void Init() = 0;
    void SetOwner(GameObject* owner);
    virtual void OnPreUpdate() {};
    virtual void OnUpdate(float deltaTime) {};
    virtual void OnLateUpdate(float deltaTime) {};
    virtual void OnGUI() {};
    GameObject* owner = nullptr;
private:
};