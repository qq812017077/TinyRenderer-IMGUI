#include "components/Renderer.h"
#include "object/GameObject.h"
#include "managers/MaterialManager.h"
#include "managers/RenderQueueManager.h"
#include "scene/SceneManager.h"
#include "graphics/Graphics.h"
#include "Material.h"
#include "Shader.h"
Renderer::Renderer(Mesh mesh)
:Renderer(mesh, Material::GetDefaultMaterialPtr())
{
}

Renderer::Renderer(Mesh mesh, std::shared_ptr<Material> pMaterial)
    :mesh(mesh), pMaterial(pMaterial)
{
    this->pMaterial->Bind(this);
}

Renderer::~Renderer()
{
    if(pMaterial) pMaterial->UnBind(this);
    // std::cout << "Renderer destructor called" << std::endl;
}

void Renderer::Init()
{
}

void Renderer::OnPreUpdate()
{
    ClearObjBuffer();
    if(IsVisible())
    {
        // RenderQueueManager::Get().AddMaterial(pMaterial.get());
        RenderQueueManager::Get().AddRenderer(this);
        TinyEngine::SceneManager::Get().AddRenderer(this);

        Vector3 worldCenter = pGameObject->transform().GetWorldMatrix() * Vector4(mesh.bounds.GetCenter(), 1.0f);
        bounds = Bounds(worldCenter, mesh.bounds.GetSize() * pGameObject->transform().GetScale());
    }
}

void Renderer::OnUpdate(float deltaTime)
{
}

bool Renderer::IsVisible() const
{
    return enabled && pGameObject->IsActived();
}

Material * Renderer::GetMaterialPtr()
{
    return pMaterial.get();
}
/**
 * @brief 
 *  use this function, mean we want to use a material which is not shared by other renderer
 * 
 * @return Material* 
 */
Material* Renderer::GetMaterial()
{
    if(pMaterial == nullptr) pMaterial = Material::GetDefaultMaterialPtr();
    
    int refCount = Material::GetRefCount(pMaterial);
    // if pMaterial is shared by other renderer, we need to create a create a instance of shared material
    if(refCount > 1)
    {
        this->pMaterial->UnBind(this);
        pMaterial = Material::CreateInstance(pMaterial);
    }
    pMaterial->Bind(this);
    sharedMaterial = false;
    return pMaterial.get();
}
std::shared_ptr<Material> Renderer::GetSharedMaterial()
{
    if(pMaterial == nullptr)
        pMaterial = Material::GetDefaultMaterialPtr();
    return pMaterial;
}

void Renderer::SetMaterial(std::shared_ptr<Material> pMaterial)
{
    if(pMaterial == nullptr) pMaterial = Material::GetDefaultMaterialPtr();
    if(this->pMaterial == nullptr) this->pMaterial = Material::GetDefaultMaterialPtr();
    
    sharedMaterial = false;
    int refCount = Material::GetRefCount(pMaterial);
    if(this->pMaterial.get() == pMaterial.get())
    {
        if(refCount == 1) return;
        // UnBind and CreateInstance
        this->pMaterial->UnBind(this);
        this->pMaterial = Material::CreateInstance(pMaterial);
        this->pMaterial->Bind(this);
    }else
    {
        this->pMaterial->UnBind(this);
        this->pMaterial = refCount == 0? pMaterial : Material::CreateInstance(pMaterial);
        this->pMaterial->Bind(this);
    }
}
void Renderer::SetSharedMaterial(std::shared_ptr<Material> pMaterial)
{
    if(pMaterial == nullptr) pMaterial = Material::GetDefaultMaterialPtr();
    if(this->pMaterial == nullptr) this->pMaterial = Material::GetDefaultMaterialPtr();
    
    sharedMaterial = true;
    int refCount = Material::GetRefCount(pMaterial);
    if(this->pMaterial.get() == pMaterial.get())
    {
        if(refCount == 1) return;
        // UnBind and CreateInstance
        this->pMaterial->UnBind(this);
        this->pMaterial = pMaterial;
        this->pMaterial->Bind(this);
    }else
    {
        this->pMaterial->UnBind(this);   //unbind old material
        this->pMaterial = pMaterial;
        this->pMaterial->Bind(this);      //bind new material
    }
    
}

void Renderer::SetMesh(Mesh mesh)
{
    this->mesh = mesh;
}

Mesh& Renderer::GetMesh()
{
    return mesh;
}


Bounds Renderer::GetBounds()
{
    return bounds;
}

// UniformBuffer& Renderer::GetObjBufferData()
// {
//     return objBuffer;
// }

void Renderer::ClearObjBuffer()
{
}

void Renderer::UpdateObjBuffer(IShaderHelper & shaderHelper)
{
    auto worldMatrix = pGameObject->transform().GetWorldMatrix();
    shaderHelper.SetGlobalMatrix("g_World", worldMatrix);
    shaderHelper.SetGlobalMatrix("g_WorldInv", worldMatrix.Inverse());
}
