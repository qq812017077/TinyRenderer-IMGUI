#pragma once
#include <iostream>
#include "Renderer.h"
#include "Transform.h"
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

    bool HasRenderer() const;
    void SetRenderer(Renderer& renderer);
    void RemoveRenderer();
    Renderer& GetRenderer() const;
    virtual void OnUpdate(float deltaTime);

    Transform transform;
protected:
    void SetRenderer(std::unique_ptr<Renderer> pRenderer);
    GameObject& operator=(const GameObject&) = delete;

private:
    std::string name = "New GameObject";
    std::unique_ptr<Renderer> pRenderer;
};