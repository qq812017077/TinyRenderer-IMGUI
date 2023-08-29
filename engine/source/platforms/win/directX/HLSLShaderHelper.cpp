#include "HLSLShaderHelper.h"

HRESULT HLSLShaderHelper::LoadShaderInfo(const char * shaderName, ID3D11ShaderReflection* pShaderReflection, ShaderDesc * pShaderDesc)
{
    HRESULT hr;

	D3D11_SHADER_DESC sd;
	hr = pShaderReflection->GetDesc(&sd);
	if (FAILED(hr))
		return hr;

	for (UINT i = 0;; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		hr = pShaderReflection->GetResourceBindingDesc(i, &bindDesc);
		if (FAILED(hr)) break; // means no more resources

        switch (bindDesc.Type)
        {
            case D3D_SIT_CBUFFER:
                {
                    ID3D11ShaderReflectionConstantBuffer* constantBuffer = pShaderReflection->GetConstantBufferByName(bindDesc.Name);
                    D3D11_SHADER_BUFFER_DESC cbufferDesc{};
                    hr = constantBuffer->GetDesc(&cbufferDesc);
                    if (FAILED(hr)) return hr;
                    bool isParam = !strcmp(bindDesc.Name, "$Params");
                    if (!isParam)
                    {

                    }else if (cbufferDesc.Variables > 0)
                    {
                        
                    }
                    if(bindDesc.BindPoint <= MaxCommonSlot){
                        if(Get().AddCommonConstantBuffer(cbufferDesc.Name, bindDesc.BindPoint, cbufferDesc.Size))
                        {
                            for (UINT j = 0; j < cbufferDesc.Variables; ++j)
                            {
                                ID3D11ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
                                // get variable slot number
                                D3D11_SHADER_VARIABLE_DESC variableDesc;
                                variable->GetDesc(&variableDesc);

                                ID3D11ShaderReflectionType* variableType = variable->GetType();
                                D3D11_SHADER_TYPE_DESC stDesc;
                                variableType->GetDesc(&stDesc);
                                Get().AddCommonVariable(bindDesc.BindPoint, variableDesc.Name, variableDesc.StartOffset, variableDesc.Size);
                            }
                        }
                    }else
                    {
                        pShaderDesc->AddConstantBufferInfo(cbufferDesc.Name, bindDesc.BindPoint, cbufferDesc.Size);
                        for (UINT j = 0; j < cbufferDesc.Variables; ++j)
                        {
                            ID3D11ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
                            // get variable slot number
                            D3D11_SHADER_VARIABLE_DESC variableDesc;
                            variable->GetDesc(&variableDesc);

                            ID3D11ShaderReflectionType* variableType = variable->GetType();
                            D3D11_SHADER_TYPE_DESC stDesc;
                            variableType->GetDesc(&stDesc);
                            pShaderDesc->AddVariable(bindDesc.BindPoint, variableDesc.Name, variableDesc.StartOffset, variableDesc.Size);
                        }
                    }
                }
                break;
            case D3D_SIT_TEXTURE:
                {
                    pShaderDesc->AddTexture(bindDesc.BindPoint, bindDesc.Name);
                }
                break;
            case D3D_SIT_SAMPLER:
                {
                    SamplerInfo samplerInfo;
                    samplerInfo.slot = bindDesc.BindPoint;
                    samplerInfo.name = bindDesc.Name;
                    pShaderDesc->AddSamplerInfo(samplerInfo);
                }
                break;
            case D3D_SIT_STRUCTURED:
            case D3D_SIT_BYTEADDRESS:
            case D3D_SIT_TBUFFER:
                break;
        }
		
	}

	return S_OK;
}
// std::shared_ptr<ICBufferVariable> HLSLShaderHelper::GetConstantBufferVariable(const char * varName)
// {
//     if (m_ConstantBufferVariables.find(varName) == m_ConstantBufferVariables.end())
//         return nullptr;
//     return m_ConstantBufferVariables[varName];
// }
CBufferData* HLSLShaderHelper::GetCommonCBufferBySlot(UINT slot)
{
    if (m_CommonCBufferBySlot.find(slot) == m_CommonCBufferBySlot.end())
        return nullptr;
    return m_CommonCBufferBySlot[slot].get();
}

bool HLSLShaderHelper::SetGlobalVariable(const char * varName, void * data,unsigned int bytesize)
{
    if (m_ConstantBufferVariables.find(varName) == m_ConstantBufferVariables.end())
        return false;
    auto & viewVar = m_ConstantBufferVariables[varName];
    viewVar->SetRaw(data, 0, bytesize);
    return true;
}
bool HLSLShaderHelper::SetGlobalMatrix(const char * varName, Matrix4x4& matrix)
{
    if (m_ConstantBufferVariables.find(varName) == m_ConstantBufferVariables.end())
        return false;
    auto & viewVar = m_ConstantBufferVariables[varName];
    viewVar->SetFloatMatrix(4, 4, reinterpret_cast<float*>(&matrix));
    return true;
}
bool HLSLShaderHelper::SetGlobalVector(const char * varName, Vector3& vec)
{
    if (m_ConstantBufferVariables.find(varName) == m_ConstantBufferVariables.end())
        return false;
    auto & viewVar = m_ConstantBufferVariables[varName];
    viewVar->SetFloatVector(3, reinterpret_cast<float*>(&vec));
    return true;
}
bool HLSLShaderHelper::SetVector(const char * varName, Vector4& vec)
{
    if (m_ConstantBufferVariables.find(varName) == m_ConstantBufferVariables.end())
        return false;
    auto & viewVar = m_ConstantBufferVariables[varName];
    viewVar->SetFloatVector(4, reinterpret_cast<float*>(&vec));
    return true;
}