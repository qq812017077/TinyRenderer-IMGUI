#include "Renderer.h"
#include "GameObject.h"

Renderer::Renderer(GameObject& go, Mesh mesh)
:go(go), mesh(mesh), pMaterial(Material::Load("shaders/DefaultVertexShader.hlsl", "shaders/DefaultPixelShader.hlsl"))
{
}
Renderer::Renderer(GameObject& go, Mesh mesh, std::shared_ptr<Material> pMaterial)
    :go(go), mesh(mesh), pMaterial(pMaterial)
{

}

Renderer::Renderer(Renderer const& other)
: go(other.go), mesh(other.mesh), pMaterial(other.pMaterial)
{
}

Renderer::~Renderer()
{
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
    auto worldMatrix = go.transform.GetWorldMatrix();
    objBuffer.AddData(worldMatrix);
}
