#include "Model.h"
#include <filesystem>
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Log.h"
#include "Bounds.h"
#include "Material.h"
#include "Mesh.h"
#include "core/EngineException.h"
#include "core/math/EngineMath.h"

using Node = Model::Node;
std::unique_ptr<Node> ParseNode(const aiNode* node);

Model::Model(): bounds(Vector3::zero,Vector3::zero) {}

std::vector<std::shared_ptr<Material>> Model::GenerateMaterials()
{
    std::vector<std::shared_ptr<Material>> materials;
    materials.resize(materialdatas.size());
    for(int i = 0; i < materialdatas.size(); ++i)
    {
        auto& matdata = materialdatas[i];
        auto pMat = Material::CreateDefault(matdata.name);
        std::cout << "Material[" << i <<"]: *******************************" << std::endl;
        if(matdata.Has<std::string>("$AmbientMap"))
        {
            std::cout << "AmbientMap: " << matdata.Get<std::string>("$AmbientMap") << std::endl;
        }
        if(matdata.Has<std::string>("$DiffuseMap"))
        {
            
            std::cout << "DiffuseMap: " << matdata.Get<std::string>("$DiffuseMap") << std::endl;
            auto texPath = model_path + "/" + matdata.Get<std::string>("$DiffuseMap");
            auto pTex = Texture::LoadFrom(texPath);
            pTex->SetMipMapLevel(0);
            if(pTex != nullptr) 
                pMat->SetTexture("_MainTex", pTex);
            else
                std::cout << "Failed to load texture from " << texPath << std::endl;
        }
        if(matdata.Has<std::string>("$SpecularMap"))
        {
            std::cout << "SpecularMap: " << matdata.Get<std::string>("$SpecularMap") << std::endl;
        }
        if(matdata.Has<std::string>("$NormalMap"))
        {
            std::cout << "NormalMap: " << matdata.Get<std::string>("$NormalMap") << std::endl;
        }
        if(matdata.Has<std::string>("$AlphaMap"))
        {
            std::cout << "AlphaMap: " << matdata.Get<std::string>("$AlphaMap") << std::endl;
        }
        if(matdata.Has<std::string>("$DisplacementMap"))
        {
            std::cout << "DisplacementMap: " << matdata.Get<std::string>("$DisplacementMap") << std::endl;
        }
        if(matdata.Has<std::string>("$ReflectionMap"))
        {
            std::cout << "ReflectionMap: " << matdata.Get<std::string>("$ReflectionMap") << std::endl;
        }
        if(matdata.Has<ColorRGBA>("$AmbientColor"))
        {
            std::cout << "AmbientColor: " << matdata.Get<ColorRGBA>("$AmbientColor") << std::endl;
        }
        if(matdata.Has<ColorRGBA>("$DiffuseColor"))
        {
            std::cout << "DiffuseColor: " << matdata.Get<ColorRGBA>("$DiffuseColor") << std::endl;
        }
        if(matdata.Has<ColorRGBA>("$SpecularColor"))
        {
            std::cout << "SpecularColor: " << matdata.Get<ColorRGBA>("$SpecularColor") << std::endl;
        }
        if(matdata.Has<ColorRGBA>("$EmissiveColor"))
        {
            std::cout << "EmissiveColor: " << matdata.Get<ColorRGBA>("$EmissiveColor") << std::endl;
        }
        if(matdata.Has<float>("$Shininess"))
        {
            std::cout << "Shininess: " << matdata.Get<float>("$Shininess") << std::endl;
        }
        if(matdata.Has<float>("$Opacity"))
        {
            std::cout << "Opacity: " << matdata.Get<float>("$Opacity") << std::endl;
        }
        materials[i] = pMat;
    }
    return materials;
}

bool Model::Load(const std::string& filePath, Model& model)
{
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE); // remove point and line primitive
    const aiScene* pScene = importer.ReadFile(filePath, 
    aiProcess_ConvertToLeftHanded |     // convert to left hand coordinate system
    aiProcess_Triangulate |             // triangulate
    aiProcess_GenBoundingBoxes |        // generate bounding box
    aiProcess_ImproveCacheLocality |    // improve cache locality
    aiProcess_JoinIdenticalVertices |   // join identical vertices
    aiProcess_SortByPType);             // sort by primitive type

    if (pScene && !(pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && pScene->HasMeshes())
    {
        
        model.model_path = filePath.substr(0, filePath.find_last_of('/'));
        float factor(1.0f);
        bool success = pScene->mMetaData->Get("UnitScaleFactor", factor);
        if(!success) Log("failed to get unit scale factor");

        auto numMeshes = pScene->mNumMeshes;
        auto numMaterials = pScene->mNumMaterials;
        model.meshdatas.resize(numMeshes);
        model.materialdatas.resize(numMaterials);

        // meshes
        for(unsigned int i = 0; i < numMeshes; ++i)
        {
            Mesh& mesh = model.meshdatas[i].mesh;
            auto pMesh = pScene->mMeshes[i];
            uint32_t numVertices = pMesh->mNumVertices;
            if(pMesh->HasPositions())
            {
                std::vector<Float3> positions(numVertices);
                for(unsigned int j = 0; j < numVertices; ++j) positions[j] = 
                Float3(pMesh->mVertices[j].x / factor, pMesh->mVertices[j].y / factor, pMesh->mVertices[j].z / factor);
                mesh.SetVertexPosition(positions);

                if(i == 0) model.bounds = mesh.GetBounds();
                else model.bounds.Encapsulate(mesh.GetBounds());
            }

            if(pMesh->HasNormals())
            {
                std::vector<Float3> normals(numVertices);
                for(unsigned int j = 0; j < numVertices; ++j) normals[j] = Float3(pMesh->mNormals[j].x, pMesh->mNormals[j].y, pMesh->mNormals[j].z);
                mesh.SetVertexNormal(normals);
            }

            if(pMesh->HasTangentsAndBitangents())
            {
                std::vector<Float3> tangents(numVertices);
                std::vector<Float3> binormals(numVertices);
                for(unsigned int j = 0; j < numVertices; ++j) 
                {
                    tangents[j] = Float3(pMesh->mTangents[j].x, pMesh->mTangents[j].y, pMesh->mTangents[j].z);
                    binormals[j] = Float3(pMesh->mBitangents[j].x, pMesh->mBitangents[j].y, pMesh->mBitangents[j].z);
                }
                mesh.SetVertexTangent(tangents);
                mesh.SetVertexBinormal(binormals);
            }

            // colors
            int numColorChannels = pMesh->GetNumColorChannels();
            while(numColorChannels && !pMesh->HasVertexColors(numColorChannels - 1)) numColorChannels--;
            if(numColorChannels)
            {
                std::vector<ColorRGBA> colors(numVertices);
                for(unsigned int j = 0; j < numVertices; ++j) 
                    colors[j] = ColorRGBA(pMesh->mColors[0][j].r, pMesh->mColors[0][j].g, pMesh->mColors[0][j].b, pMesh->mColors[0][j].a);
                mesh.SetVertexColor(colors);
            }
            
            // texcoords
            int numUVChannels = pMesh->GetNumUVChannels();
            while(numUVChannels && !pMesh->HasTextureCoords(numUVChannels - 1)) numUVChannels--;
            if(numUVChannels)
            {
                std::vector<Float2> texCoords(numVertices);
                for(unsigned int j = 0; j < numVertices; ++j) 
                    texCoords[j] = Float2(pMesh->mTextureCoords[0][j].x, pMesh->mTextureCoords[0][j].y);
                mesh.SetVertexTexCoord(texCoords);
            }

            // indices
            uint32_t numFaces = pMesh->mNumFaces;
            uint32_t numIndices = numFaces * 3;
            if(numIndices > 0)
            {
                if(numIndices < 65535)
                {
                    std::vector<uint16_t> indices;
                    for(unsigned int i = 0; i < numFaces; ++i)
                    {
                        auto pFace = pMesh->mFaces[i];
                        assert(pFace.mNumIndices == 3);
                        indices.push_back(pFace.mIndices[0]);
                        indices.push_back(pFace.mIndices[1]);
                        indices.push_back(pFace.mIndices[2]);
                    }
                    mesh.SetVertexIndices(indices);
                }else
                {
                    std::vector<uint32_t> indices;
                    for(unsigned int i = 0; i < numFaces; ++i)
                    {
                        auto pFace = pMesh->mFaces[i];
                        assert(pFace.mNumIndices == 3);
                        indices.push_back(pFace.mIndices[0]);
                        indices.push_back(pFace.mIndices[1]);
                        indices.push_back(pFace.mIndices[2]);
                    }
                    mesh.SetVertexIndices(indices);
                }
            }
            model.meshdatas[i].m_MaterialIndex = pMesh->mMaterialIndex;
        }

        // materials
        for (uint32_t i = 0; i < pScene->mNumMaterials; ++i)
        {
            auto pAiMaterial = pScene->mMaterials[i];
            MaterialData& matdata = model.materialdatas[i];
            matdata.name = pAiMaterial->GetName().C_Str();
            Vector4 vec4;
            float value{};
            uint32_t boolean{};
            uint32_t num = 3;

            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, (float*)&vec4, &num))
                matdata.Set("$AmbientColor", ColorRGBA(vec4.x, vec4.y, vec4.z, vec4.w));
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, (float*)&vec4, &num))
                matdata.Set("$DiffuseColor", ColorRGBA(vec4.x, vec4.y, vec4.z, vec4.w));
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, (float*)&vec4, &num))
                matdata.Set("$SpecularColor", ColorRGBA(vec4.x, vec4.y, vec4.z, vec4.w));
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, (float*)&vec4, &num))
                matdata.Set("$EmissiveColor", ColorRGBA(vec4.x, vec4.y, vec4.z, vec4.w));
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, (float*)&vec4, &num))
                matdata.Set("$TransparentColor", ColorRGBA(vec4.x, vec4.y, vec4.z, vec4.w));
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, (float*)&vec4, &num))
                matdata.Set("$ReflectiveColor", ColorRGBA(vec4.x, vec4.y, vec4.z, vec4.w));
            
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_SHININESS, value))
                matdata.Set("$Shininess", value);
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, value))
                matdata.Set("$ShininessStrength", value);
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_OPACITY, value))
                matdata.Set("$Opacity", value);
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_REFRACTI, value))
                matdata.Set("$Refraction", value);
            if (aiReturn_SUCCESS == pAiMaterial->Get(AI_MATKEY_REFLECTIVITY, value))
                matdata.Set("$Reflectivity", value);
            
            aiString path;
            if (pAiMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_AMBIENT, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$AmbientMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$DiffuseMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$SpecularMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$EmissiveMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_NORMALS, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$NormalMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_HEIGHT) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_HEIGHT, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$HeightMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_OPACITY) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_OPACITY, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$OpacityMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$DisplacementMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_LIGHTMAP) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$LightMap", tex_filename.string());
                }
            }

            if (pAiMaterial->GetTextureCount(aiTextureType_REFLECTION) > 0)
            {
                if (aiReturn_SUCCESS == pAiMaterial->GetTexture(aiTextureType_REFLECTION, 0, &path))
                {
                    std::filesystem::path tex_filename = path.C_Str();
                    matdata.Set("$ReflectionMap", tex_filename.string());
                }
            }
            
        }
        
        
        model.pRoot = ParseNode(pScene->mRootNode);
        return true;
    }
    return false;
}

std::unique_ptr<Node> ParseNode(const aiNode* node)
{
    std::unique_ptr<Node> pNode = std::make_unique<Node>(node->mName.C_Str());
    pNode->transform = Matrix4x4{
        node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
        node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
        node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
        node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4
    };
    
    for(size_t i=0; i < node->mNumMeshes; i++) pNode->meshIndices.push_back(node->mMeshes[i]);
    for(size_t i=0; i < node->mNumChildren; i++) pNode->children.push_back(ParseNode(node->mChildren[i]));
    return pNode;
}