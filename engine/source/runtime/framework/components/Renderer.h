#pragma once
#include <iostream>
#include "Mesh.h"
#include "components/Component.h"

class GameObject;
class Material;
class IShaderHelper;
class Renderer : public Component
{
public:
    friend class Graphics;
    // Renderer();
    Renderer(Mesh mesh);
    Renderer(Mesh mesh, std::shared_ptr<Material> pMaterial);
    ~Renderer();
    Renderer(Renderer const& other) = delete;
    Renderer& operator=(Renderer& other) noexcept = delete;

    void Init() override ;
    void OnPreUpdate() override;
    void OnUpdate(float deltaTime) override;
    bool IsVisible() const;
    bool IsSharedMaterial() const { return sharedMaterial;}
    Material * GetMaterialPtr();
    Material * GetMaterial();
    std::shared_ptr<Material> GetSharedMaterial();
    void SetMaterial(std::shared_ptr<Material> pMaterial);
    void SetSharedMaterial(std::shared_ptr<Material> pMaterial);
    void SetMesh(Mesh mesh);
    Mesh& GetMesh();

    void UpdateObjBuffer(IShaderHelper& shaderHelper);
protected:
    void ClearObjBuffer();
    
    bool sharedMaterial = true;
    std::shared_ptr<Material> pMaterial;
    Mesh mesh;
    
};