#pragma once
#include "Material.h"
#include <iostream>
#include "Mesh.h"

class GameObject;
class Renderer
{
public:
    friend class Graphics;

    Renderer(GameObject& go, Mesh mesh);
    Renderer(GameObject& go, Mesh mesh, std::shared_ptr<Material> pMaterial);
    ~Renderer();
    Renderer(Renderer const& other);
    Renderer& operator=(Renderer& other) noexcept = delete;

    std::shared_ptr<Material> GetMaterial() const;
    void SetMaterial(std::shared_ptr<Material> pMaterial);

    Mesh& GetMesh();
    UniformBuffer& GetObjBufferData();
    
protected:
    void ClearObjBuffer();
    void UpdateObjBuffer();
    
    std::shared_ptr<Material> pMaterial;
    Mesh mesh;
    GameObject& go;
    UniformBuffer objBuffer;
};