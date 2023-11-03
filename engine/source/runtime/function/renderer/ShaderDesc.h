#pragma once
#include <map>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "Color.h"
#include "Matrix.h"

typedef int INT;
typedef float FLOAT;
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned long DWORD;

enum class TextureDim
{
	Texture2D,
	Texture2DArray,
	TextureCube,
	TextureCubeArray
};


struct VariableDesc
{
	bool isGlobal{ false };
	std::string varName;
	unsigned int startOffset;
	unsigned int size;

	VariableDesc() = default;
	VariableDesc(const char* name, unsigned int offset, unsigned int size)
		:varName(name), startOffset(offset), size(size)
	{
	}

	VariableDesc(std::string name, unsigned int offset, unsigned int size)
		:varName(name), startOffset(offset), size(size)
	{
		
	}
};

struct CBufferDesc
{
	unsigned int bindPoint;
	std::string bufferName;
	unsigned int bufSize;
	std::vector<VariableDesc> variables;
	CBufferDesc() = default;

	CBufferDesc(unsigned int slot, std::string name, unsigned int size)
		:bindPoint(slot), bufferName(name), bufSize(size)
	{
		variables.clear();
	}

	CBufferDesc(unsigned int slot, const char * name, unsigned int size)
		:bindPoint(slot), bufferName(name), bufSize(size)
	{
		variables.clear();
	}
};

struct TextureDesc
{
	unsigned int bindPoint;
	std::string textureName;
	TextureDim type;
	TextureDesc() = default;
	TextureDesc(unsigned int slot, std::string name, TextureDim type = TextureDim::Texture2D)
		:bindPoint(slot), textureName(name), type(type)
	{
	}
};

struct SamplerDesc
{
	unsigned int bindPoint;
	std::string samplerName;

	SamplerDesc() = default;
	SamplerDesc(unsigned int slot, std::string name)
		:bindPoint(slot), samplerName(name)
	{
	}
};

struct ShaderDesc
{
	std::string shaderPath;
	std::vector<CBufferDesc> cbuffers;
	std::vector<TextureDesc> textures;
	std::vector<SamplerDesc> samplers;
};



struct inputLayoutInfo
{
    std::string semanticName;
    unsigned int size;
    unsigned int offset;
};

struct SamplerInfo
{
	size_t uniqueID;
    int bindPoint;
    std::string name;

	SamplerInfo()
	{
		uniqueID = id++;
	}

	SamplerInfo(std::string name, int bindPoint)
		:name(name), bindPoint(bindPoint)
	{
		uniqueID = id++;
	}

	static size_t id;
};
struct CBufferData
{
	size_t uniqueID;
    bool isDirty = true;
	UINT slot;
	std::string cbufferName;
	UINT byteWidth;
    std::unique_ptr<BYTE[]> pData;

    CBufferData() : slot(), byteWidth() {uniqueID = id++;}
	CBufferData(const char* name, UINT startSlot, UINT byteWidth, BYTE* initData = nullptr) :
		cbufferName(name), pData(new BYTE[byteWidth]{}), slot(startSlot),
		byteWidth(byteWidth)
	{
		if (initData)
			memcpy_s(pData.get(), byteWidth, initData, byteWidth);

		uniqueID = id++;
	}
	CBufferData(std::string name, UINT startSlot, UINT byteWidth, BYTE* initData = nullptr) :
		cbufferName(name), pData(new BYTE[byteWidth]{}), slot(startSlot),
		byteWidth(byteWidth)
	{
		if (initData)
			memcpy_s(pData.get(), byteWidth, initData, byteWidth);

		uniqueID = id++;
	}
	static size_t id;
};


class Texture;
struct ITextureVariable
{
	virtual void Set(const std::string& name, std::shared_ptr<Texture>& pTexture,const  std::string& samplerName) = 0;
	virtual Texture* GetTexturePtr() = 0;
};

struct ICBufferVariable
{
    virtual void SetBool(bool val) = 0;
	virtual void SetUInt(UINT val) = 0;
	virtual void SetInt(INT val) = 0;
	virtual void SetFloat(FLOAT val) = 0;
	virtual void SetColor(Color color) = 0;
	virtual void SetMatrix(Matrix4x4 matrix) = 0;
    virtual void SetBoolVector(UINT numComponents, const bool data[4]) = 0;
	virtual void SetUIntVector(UINT numComponents, const UINT data[4]) = 0;
	virtual void SetIntVector(UINT numComponents, const INT data[4]) = 0;
	virtual void SetFloatVector(UINT numComponents, const FLOAT data[4]) = 0;

    virtual void SetBoolMatrix(UINT rows, UINT cols, const bool* noPadData) = 0;
	virtual void SetUIntMatrix(UINT rows, UINT cols, const UINT* noPadData) = 0;
	virtual void SetIntMatrix(UINT rows, UINT cols, const INT* noPadData) = 0;
	virtual void SetFloatMatrix(UINT rows, UINT cols, const FLOAT* noPadData) = 0;

	// 设置其余类型，允许指定设置范围
	virtual void SetRaw(const void* data, UINT byteOffset = 0, UINT byteCount = 0xFFFFFFFF) = 0;
	virtual bool GetRaw(void* pOutput, UINT byteOffset = 0, UINT byteCount = 0xFFFFFFFF) = 0;
};

struct TextureVariable: public ITextureVariable
{
	size_t uniqueID;
	bool isDirty = true;
	
	TextureDesc desc;
	std::shared_ptr<Texture> pTexture = nullptr;
	std::string samplerName;

	std::string name() const { return desc.textureName; }
	int slot() const { return desc.bindPoint; }
	TextureDim type() const { return desc.type; }

	TextureVariable(TextureDesc desc)
		:desc(desc), pTexture(nullptr)
	{
		uniqueID = id++;
		samplerName = "sampler" + desc.textureName;
	}
	// TextureVariable(int slot, std::string name, std::shared_ptr<Texture> pTexture, std::string samplerName)
	// 	:slot(slot), name(name), pTexture(pTexture), samplerName(samplerName)
	// {
	// 	uniqueID = id++;
	// }

	void Set(const std::string& name, std::shared_ptr<Texture>& pTexture,const  std::string& samplerName) override
	{
		this->desc.textureName = name;
		this->pTexture = pTexture;
		this->samplerName = samplerName;
		isDirty = true;
	}

	Texture * GetTexturePtr() override
	{
		return pTexture == nullptr? nullptr : pTexture.get();
	}

	static size_t id;
};

struct CBufferVariable : public ICBufferVariable
{
	static float colordata[4];
	bool isGlobal{ false };
	UINT startByteOffset = 0;
	UINT byteWidth = 0;
	CBufferData* pCBufferData = nullptr;

	CBufferVariable() = default;
	~CBufferVariable() = default;

	CBufferVariable(UINT offset, UINT size, CBufferData* pData)
		:startByteOffset(offset), byteWidth(size), pCBufferData(pData)
	{
	}

	CBufferVariable(VariableDesc varDesc, CBufferData* pData)
		:startByteOffset(varDesc.startOffset), byteWidth(varDesc.size), pCBufferData(pData), isGlobal(varDesc.isGlobal)
	{
	}

	unsigned int GetSlot() const {	return pCBufferData == nullptr? -1 : pCBufferData->slot; }
    void SetBool(bool val) override     { SetUInt(val); }
	void SetUInt(UINT val) override     { SetRaw(&val, 0, 4); }
	void SetInt(INT val) override       { SetRaw(&val, 0, 4); }
	void SetFloat(FLOAT val) override   { SetRaw(&val, 0, 4); }
	void SetColor(Color color) override {
		colordata[0] = color.GetRf();
		colordata[1] = color.GetGf();
		colordata[2] = color.GetBf();
		colordata[3] = color.GetAf();
		SetFloatVector(4, colordata);
	}
	void SetMatrix(Matrix4x4 matrix) override { SetFloatMatrix(4, 4, matrix.GetData()); }


	void SetBoolVector(UINT numComponents, const bool data[4]) override 
	{
		SetUIntVector(numComponents, reinterpret_cast<const UINT*>(data));
	}

	void SetUIntVector(UINT numComponents, const UINT data[4]) override
	{
		if (numComponents > 4)
			numComponents = 4;
		UINT byteCount = numComponents * sizeof(UINT);
		if (byteCount > byteWidth)
			byteCount = byteWidth;
		SetRaw(data, 0, byteCount);
	}

	void SetIntVector(UINT numComponents, const INT data[4]) override
	{
		if (numComponents > 4)
			numComponents = 4;
		UINT byteCount = numComponents * sizeof(INT);
		if (byteCount > byteWidth)
			byteCount = byteWidth;
		SetRaw(data, 0, byteCount);
	}

	void SetFloatVector(UINT numComponents, const FLOAT data[4]) override
	{
		if (numComponents > 4)
			numComponents = 4;
		UINT byteCount = numComponents * sizeof(FLOAT);
		if (byteCount > byteWidth)
			byteCount = byteWidth;
		SetRaw(data, 0, byteCount);
	}

	void SetBoolMatrix(UINT rows, UINT cols, const bool* noPadData) override
	{
		SetUIntMatrix(rows, cols, reinterpret_cast<const UINT*>(noPadData));
	}

	void SetUIntMatrix(UINT rows, UINT cols, const UINT* noPadData) override
	{
		SetMatrixInBytes(rows, cols, reinterpret_cast<const BYTE*>(noPadData));
	}

	void SetIntMatrix(UINT rows, UINT cols, const INT* noPadData) override
	{
		SetMatrixInBytes(rows, cols, reinterpret_cast<const BYTE*>(noPadData));
	}

	void SetFloatMatrix(UINT rows, UINT cols, const FLOAT* noPadData) override
	{
		SetMatrixInBytes(rows, cols, reinterpret_cast<const BYTE*>(noPadData));
	}

	void SetRaw(const void* data, UINT byteOffset = 0, UINT byteCount = 0xFFFFFFFF) override
	{
		if (byteOffset > byteWidth)
			return;
		if (byteOffset + byteCount > byteWidth)
			byteCount = byteWidth - byteOffset;

		// 仅当值不同时更新
		if (memcmp(pCBufferData->pData.get() + startByteOffset + byteOffset, data, byteCount))
		{
			memcpy_s(pCBufferData->pData.get() + startByteOffset + byteOffset, byteCount, data, byteCount);
			pCBufferData->isDirty = true;
		}
	}

	bool GetRaw(void* pOutput, UINT byteOffset = 0, UINT byteCount = 0xFFFFFFFF) override
	{
		if (byteOffset > byteWidth || byteCount > byteWidth - byteOffset)
			return false;
		if (!pOutput)
			return false;
		memcpy_s(pOutput, byteCount, pCBufferData->pData.get() + startByteOffset + byteOffset, byteCount);
		return true;
	}

	void SetMatrixInBytes(UINT rows, UINT cols, const BYTE* noPadData)
	{
		// 仅允许1x1到4x4
		if (rows == 0 || rows > 4 || cols == 0 || cols > 4)
			return;
		UINT remainBytes = byteWidth < 64 ? byteWidth : 64;
		BYTE* pData = pCBufferData->pData.get() + startByteOffset;
		while (remainBytes > 0 && rows > 0)
		{
			UINT rowPitch = sizeof(DWORD) * cols < remainBytes ? sizeof(DWORD) * cols : remainBytes;
			// 仅当值不同时更新
			if (memcmp(pData, noPadData, rowPitch))
			{
				memcpy_s(pData, rowPitch, noPadData, rowPitch);
				pCBufferData->isDirty = true;
			}
			noPadData += cols * sizeof(DWORD);
			pData += 16;
			remainBytes = remainBytes < 16 ? 0 : remainBytes - 16;
		}
	}

};


struct ShaderResource
{
	std::unordered_map<int, std::unique_ptr<CBufferData>> CBufferBySlot;
	std::unordered_map<std::string, std::shared_ptr<CBufferVariable>> variables;
	std::unordered_map<std::string, std::shared_ptr<TextureVariable>> textures;
	std::vector<SamplerInfo> samplers;
	std::map<int, int> texIndexBufBySlot;
	std::map<std::string, int> texIndexBufByName;
	std::map<int, int> samplerIndexBufBySlot;
	std::map<std::string, int> samplerIndexBufByName;

	SamplerInfo GetSamplerInfoByName(const char * name, int slot)
	{
		if (samplerIndexBufByName.find(name) == samplerIndexBufByName.end())
		{
			// try to find by slot
			if (samplerIndexBufBySlot.find(slot) == samplerIndexBufBySlot.end())
			{
				return samplers[0];
			}
			return samplers[samplerIndexBufBySlot[slot]];
		}
		return samplers[samplerIndexBufByName[name]];
	}
};

