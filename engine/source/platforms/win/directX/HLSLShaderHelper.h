#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

#include <wrl.h>
#include <d3d11shader.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>

#include "core/Singleton.h"
#include "Shader.h"
#include "Mesh.h"
#include "ShaderDesc.h"
#include "TextureFormat.h"
#include "SampleMode.h"
#include "DirectXGraphics.h"
class HLSLShaderHelper : public Singleton<HLSLShaderHelper> , public IShaderHelper
{
public:
    friend DirectXGraphics;
    const static unsigned int MaxCommonSlot = 2u;
    const static unsigned int PerFrameCBufSlot = 0u;
    const static unsigned int PerDrawCBufSlot = 1u;
    const static unsigned int PerLightingCBufSlot = 2u;

    
    const static unsigned int ShadowMapSlot = 0u;
    const static unsigned int ShadowCubeMapSlot = 3u;

    const static unsigned int IrradianceMapSlot = 4u;
    const static unsigned int PrefilterMapSlot = 5u;
    const static unsigned int BrdfLutMapSlot = 6u;

    HLSLShaderHelper() = default;

    // std::shared_ptr<ICBufferVariable> GetConstantBufferVariable(const char * varName) override;
    bool SetGlobalVariable(const char * varName, void * data, unsigned int bytesize) override;
    bool SetGlobalMatrix(const char * varName, Matrix4x4& matrix) override;
    bool SetGlobalUINT(const char * varName, size_t uintVal) override;
    bool SetGlobalVector3(const char * varName, Vector3& vec) override;
    bool SetGlobalVector4(const char * varName, Vector4& vec) override;
    
    CBufferData* GetCommonCBufferBySlot(UINT slot);
    
    // void Update(UniformBuffer * buffer);
    static HRESULT LoadShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel, 
    wrl::ComPtr<ID3D11VertexShader>& pVertexShaderOut, wrl::ComPtr<ID3DBlob> & pBlobOut, wrl::ComPtr<ID3D11ShaderReflection> & pShaderReflectionOut);
    static HRESULT LoadShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel, 
    wrl::ComPtr<ID3D11PixelShader>& pPixelShaderOut, wrl::ComPtr<ID3DBlob> & pBlobOut, wrl::ComPtr<ID3D11ShaderReflection> & pShaderReflectionOut);
	static HRESULT LoadShaderInfo(ID3D11ShaderReflection* pShaderReflection, ShaderDesc * pShaderDesc);
    
    static bool IsGlobalTextureSlot(unsigned int slot) ; 
protected:
    HLSLShaderHelper(const HLSLShaderHelper&) = delete;
    HLSLShaderHelper& operator=(const HLSLShaderHelper&) = delete;
private:
    bool AddCommonConstantBuffer(const char * bufName, UINT slot, UINT byteWidth)
    {
        if(m_CommonCBufferBySlot.find(slot) != m_CommonCBufferBySlot.end())
            return false;
		m_CommonCBufferBySlot.emplace(slot, std::make_unique<CBufferData>(bufName, slot, byteWidth));
        return true;
    }
	void AddCommonVariable(UINT slot, const char * bufName, UINT startOffset, UINT size)
	{
		m_ConstantBufferVariables.emplace(bufName, std::make_shared<CBufferVariable>(startOffset, size, m_CommonCBufferBySlot[slot].get()));
	}
    std::unordered_map<int, std::unique_ptr<CBufferData>> m_CommonCBufferBySlot; // common cbuffer
    
    std::unordered_map<std::string, std::shared_ptr<CBufferVariable>> m_ConstantBufferVariables;
    std::unordered_map<std::string, std::shared_ptr<VertexShader>> m_VertexShaders;
	// std::unordered_map<std::string, std::shared_ptr<HullShaderInfo>> m_HullShaders;			
	// std::unordered_map<std::string, std::shared_ptr<DomainShaderInfo>> m_DomainShaders;		
	// std::unordered_map<std::string, std::shared_ptr<GeometryShaderInfo>> m_GeometryShaders;	
	std::unordered_map<std::string, std::shared_ptr<PixelShader>> m_PixelShaders;
	// std::unordered_map<std::string, std::shared_ptr<ComputeShaderInfo>> m_ComputeShaders;

    static std::unordered_map<UINT, bool> m_GlobalTextureSlots;
};