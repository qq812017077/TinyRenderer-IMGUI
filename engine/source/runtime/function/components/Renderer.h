#pragma once
#include "Material.h"
#include <iostream>
#include "Mesh.h"
#include "components/Component.h"

class GameObject;
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
    void SetSharedMaterial(std::shared_ptr<Material> pMaterial);
    // void SetMaterial(Material material);
    std::shared_ptr<Material> GetSharedMaterial() const;
    Material * GetMaterial();
    void SetMesh(Mesh mesh);
    Mesh& GetMesh();

    UniformBuffer& GetObjBufferData();    
protected:
    void ClearObjBuffer();
    void UpdateObjBuffer();
    
    std::shared_ptr<Material> pSharedMaterial;
    std::unique_ptr<Material> pMaterial;
    Mesh mesh;
    UniformBuffer objBuffer;
};