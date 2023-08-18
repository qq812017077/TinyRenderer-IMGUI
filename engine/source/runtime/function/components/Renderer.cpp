#include "components/Renderer.h"
#include "core/GameObject.h"
#include "MaterialManager.h"

// Renderer::Renderer():
//     Renderer(Mesh())
// {
// }
Renderer::Renderer(Mesh mesh)
:Renderer(mesh, Material::GetDefaultMaterialPtr())
{
}

Renderer::Renderer(Mesh mesh, std::shared_ptr<Material> pMaterial)
    :mesh(mesh), pSharedMaterial(pMaterial)
{
    MaterialManager::AddRenderer(*this);
}

Renderer::~Renderer()
{
    MaterialManager::RemoveRenderer(*this);
    std::cout << "Renderer destructor called" << std::endl;
}
// Renderer& Renderer::operator=(Renderer& other) noexcept
// {
//     if(this == &other) return *this;
//     pMaterial = other.pMaterial;
//     mesh = other.mesh;
//     return *this;
// }

void Renderer::Init()
{
    // objBuffer.Init();
    // objBuffer.AddData(owner->transform.GetWorldMatrix());
}

std::shared_ptr<Material> Renderer::GetSharedMaterial() const
{
    return pSharedMaterial;
}

/**
 * @brief 
 *  use this function, mean we want to use a material which is not shared by other renderer
 * 
 * @return Material* 
 */
Material* Renderer::GetMaterial()
{
    return pMaterial.get();
}

void Renderer::SetSharedMaterial(std::shared_ptr<Material> pMaterial)
{
    if(this->pSharedMaterial == pMaterial) return;
    if(this->pSharedMaterial != nullptr) 
        MaterialManager::RemoveRenderer(*this);
    this->pSharedMaterial = pMaterial;
    MaterialManager::AddRenderer(*this);
}

void Renderer::SetMesh(Mesh mesh)
{
    this->mesh = mesh;
}

Mesh& Renderer::GetMesh()
{
    return mesh;
}

UniformBuffer& Renderer::GetObjBufferData()
{
    return objBuffer;
}

void Renderer::ClearObjBuffer()
{
    objBuffer.Clear();
}

void Renderer::UpdateObjBuffer()
{
    auto worldMatrix = owner->transform.GetWorldMatrix();
    objBuffer.AddData(worldMatrix);
}
