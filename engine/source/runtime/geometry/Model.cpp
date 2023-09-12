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

Model::Model(): bounds(Vector3::zero,Vector3::zero) {}

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
            float factor(0.0f);
            bool success = pScene->mMetaData->Get("UnitScaleFactor", factor);
            if(!success) Log("failed to get unit scale factor");

            auto numMeshes = pScene->mNumMeshes;
            auto numMaterials = pScene->mNumMaterials;
            model.meshdatas.resize(numMeshes);
            model.matdatas.resize(numMaterials);

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
                MaterialData& matdata = model.matdatas[i];
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
            return true;
        }
        return false;
    }