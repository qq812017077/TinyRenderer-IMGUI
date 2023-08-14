#include "components/Renderer.h"
#include "core/GameObject.h"
#include "MaterialManager.h"

// Renderer::Renderer():
//     Renderer(Mesh())
// {
// }
Renderer::Renderer(Mesh mesh)
:Renderer(mesh, Material::Load("shaders/DefaultVertexShader.hlsl", "shaders/DefaultPixelShader.hlsl"))
{
}

Renderer::Renderer(Mesh mesh, std::shared_ptr<Material> pMaterial)
    :mesh(mesh), pMaterial(pMaterial)
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

std::shared_ptr<Material> Renderer::GetMaterial() const
{
    return pMaterial;
}

void Renderer::SetMaterial(std::shared_ptr<Material> pMaterial)
{
    this->pMaterial = pMaterial;
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
