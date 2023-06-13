#include "EngineWin.h"
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

// 安全COM组件释放宏
#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }

// ------------------------------
// CreateShaderFromFile函数
// ------------------------------
// [In]csoFileNameInOut 编译好的着色器二进制文件(.cso)，若有指定则优先寻找该文件并读取
// [In]hlslFileName     着色器代码，若未找到着色器二进制文件则编译着色器代码
// [In]entryPoint       入口点(指定开始的函数)
// [In]shaderModel      着色器模型，格式为"*s_5_0"，*可以为c,d,g,h,p,v之一
// [Out]ppBlobOut       输出着色器二进制信息
HRESULT CreateShaderFromFile(const WCHAR * csoFileNameInOut, const WCHAR * hlslFileName,
    LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut)
{
    if ( !csoFileNameInOut || !entryPoint || !shaderModel || !ppBlobOut )
       return E_INVALIDARG;
    
    HRESULT hr = S_OK;

    // 寻找是否有已经编译好的顶点着色器
    if (csoFileNameInOut && D3DReadFileToBlob(csoFileNameInOut, ppBlobOut) == S_OK)
    {
        return hr;
    }
    else
    {
        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        // 设置 D3DCOMPILE_DEBUG 标志用于获取着色器调试信息。该标志可以提升调试体验，
        // 但仍然允许着色器进行优化操作
        dwShaderFlags |= D3DCOMPILE_DEBUG;

        // 在Debug环境下禁用优化以避免出现一些不合理的情况
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        ID3DBlob* errorBlob = nullptr;
        hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
            dwShaderFlags, 0, ppBlobOut, &errorBlob);
        if (FAILED(hr))
        {
            if (errorBlob != nullptr)
            {
                const char* errorMsg = reinterpret_cast<const char*>(errorBlob->GetBufferPointer());
                OutputDebugStringA(errorMsg);
                MessageBox(nullptr, errorMsg, "Shader Compilation Error", MB_RETRYCANCEL);
            }
            SAFE_RELEASE(errorBlob);
            return hr;
        }

        // 若指定了输出文件名，则将着色器二进制信息输出
        if (csoFileNameInOut)
        {
            return D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE);
        }
    }

    return hr;
}