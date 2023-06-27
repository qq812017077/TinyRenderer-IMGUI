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
    void SetOwner(GameObject* owner);
    GameObject* owner = nullptr;
};