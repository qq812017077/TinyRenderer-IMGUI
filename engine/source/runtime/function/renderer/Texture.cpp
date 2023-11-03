#include "Texture.h"
#include "Color.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <core/EngineException.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include "stb/stb_image_write.h"

#include "Color.h"
#include <string>


static int GetPixelByteSize(ETextureFormat textureFormat)
{
    switch (textureFormat)
    {
    case ETextureFormat::RGBA32:
        return 4 * sizeof(uint8_t);
    case ETextureFormat::RGBAFloat:
        return 4 * sizeof(float);
    case ETextureFormat::RGHalf:
        return 2 * sizeof(uint16_t);
    case ETextureFormat::RGBFloat:
        return 3 * sizeof(float);
    }
    THROW_ENGINE_EXCEPTION("unknowned texture format. cannot get channel");
    return 0;
}

std::shared_ptr<Texture> Texture::pDefaultTexture = nullptr;

Texture::Texture(int width, int height, ETextureFormat textureFormat, int mipCount, bool linear):
    Texture(Surface(width, height, GetPixelByteSize(textureFormat)), textureFormat, mipCount, linear)
{

}

Texture::Texture(Texture&& texture) noexcept:
    m_Surface(std::move(texture.m_Surface)), 
    m_TextureFormat(texture.m_TextureFormat), 
    m_FilterMode(texture.m_FilterMode),
    m_WrapMode(texture.m_WrapMode), 
    m_Anisolevel(texture.m_Anisolevel), 
    m_MipLevel(texture.m_MipLevel), 
    m_Linear(texture.m_Linear)
{
    m_MipChain = std::move(texture.m_MipChain);
    texture.m_MipChain.clear();
}

Texture::Texture(Surface&& surface, ETextureFormat textureFormat):
    Texture(std::move(surface), textureFormat, 1, false)
{

}
Texture::Texture(Surface&& surface, ETextureFormat textureFormat, int mipCount, bool linear):
    m_Surface(std::move(surface)), m_TextureFormat(textureFormat), m_Linear(linear)
{
    SetMipMapLevel(mipCount);
    m_WrapMode = EWrapMode::Repeat;
}

Texture::~Texture()
{
    releaseMipChain();
}



void Texture::PutPixel( unsigned int x,unsigned int y,Color c )
{
    
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < m_Surface.width );
	assert( y < m_Surface.height );
	// m_Surface.pBuffer[y * m_Surface.width + x] = c;
    THROW_ENGINE_EXCEPTION("Not implemented");
}
Color Texture::GetPixel( unsigned int x,unsigned int y ) const
{
    assert( x >= 0 );
	assert( y >= 0 );
	assert( x < m_Surface.width );
	assert( y < m_Surface.height );
    
    THROW_ENGINE_EXCEPTION("Not implemented");
	return Color::White();
}

void * Texture::GetImageData() const
{
    return m_Surface.pBuffer.get();
}

void * Texture::GetMipmapImageData(int mipLevel) const
{
    return m_MipChain[mipLevel]->pBuffer.get();
}


void Texture::SetData(const void* data, long size, int mipLevelIndex)
{
    Texture::Surface * pSurface = nullptr;
    if(mipLevelIndex == 0) pSurface = &m_Surface;
    else pSurface = m_MipChain[mipLevelIndex];

    int numBytes = pSurface->width * pSurface->height * GetPixelByteSize(m_TextureFormat);
    if(numBytes != size) THROW_ENGINE_EXCEPTION("data size not match");
    memcpy_s(pSurface->pBuffer.get(), size, data, size);
}


int Texture::GetWidth() const
{
    return m_Surface.width;
}

int Texture::GetHeight() const
{
    return m_Surface.height;
}

int Texture::GetRowPitch() const
{
    return m_Surface.width * GetPixelByteSize(m_TextureFormat);
}


int Texture::GetMipmapRowPitch(int level) const
{
    return m_MipChain[level]->width * GetPixelByteSize(m_TextureFormat);
}

ETextureFormat Texture::GetTextureFormat() const
{
    return m_TextureFormat;
}

EFilterMode Texture::GetFilterMode() const
{
    return m_FilterMode;
}
EWrapMode Texture::GetWrapMode() const
{
    return m_WrapMode;
}
int Texture::GetAnisoLevel() const
{
    return m_Anisolevel;
}
bool Texture::UseMipMap() const
{
    return m_MipLevel > 1 || m_MipLevel == 0;
}
int Texture::GetMipMapLevels() const
{
    return m_MipLevel;
}

bool Texture::IsLinear() const
{
    return m_Linear;
}

bool Texture::HasAlphaChannel() const
{
    return m_TextureFormat == ETextureFormat::RGBA32;
}


void Texture::SetFilterMode(EFilterMode filterMode)
{
    m_FilterMode = filterMode;
}

void Texture::SetWrapMode(EWrapMode wrapMode)
{
    m_WrapMode = wrapMode;
}

void Texture::SetAnisoLevel(int anisoLevel)
{
    m_Anisolevel = anisoLevel;
}

void Texture::SetMipMapLevel(int mipMapLevel)
{
    m_MipLevel = mipMapLevel;
    if(m_MipLevel != 1)
    {
        m_FilterMode = EFilterMode::Anisotropic;
        m_Anisolevel = -1;
    }else
    {
        m_FilterMode = EFilterMode::Bilinear;
        m_Anisolevel = 1;
    }
}

void Texture::Clear(Color fillValue)
{
    memset(m_Surface.pBuffer.get(), fillValue.dword(), m_Surface.width * m_Surface.height * GetPixelByteSize(m_TextureFormat));
}


void Texture::SavePNG(const std::string& path, int miplevel)
{
    Texture::Surface * pSurface = nullptr;
    if(miplevel == 0) pSurface = &m_Surface;
    else pSurface = m_MipChain[miplevel];
    
    int width = pSurface->width;
    int height = pSurface->height;
    int rowPitch = width * GetPixelByteSize(m_TextureFormat);
    stbi_write_png(path.c_str(), width, height, 4, pSurface->pBuffer.get(), rowPitch);
}

std::shared_ptr<Texture> Texture::LoadFrom(const char* filename)
{
    int width;
	int height;
	int channels;


	unsigned char *image = stbi_load(filename, &width, &height, &channels, STBI_default); // force channels = 4
	if (!image) {
		// throw std::runtime_error("Failed to load image: " + filename);
		char error[50] = "Failed to load image: ";
		strcat_s(error, sizeof(error), filename);
		throw EngineException(__LINE__, __FILE__, error);
	}
    
	assert(width > 0 && height > 0 && channels > 0);

    long numBytes = height * width * 4 * sizeof(uint8_t);
    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(numBytes);

    /**
     * we need to expand the image to 4 channels, because dx11 has no support for R8G8B8, only R8G8B8A8
     */
    for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
            buffer[(y * width + x) * 4 + 0] = image[(y * width + x) * channels + 0];
            buffer[(y * width + x) * 4 + 1] = image[(y * width + x) * channels + 1];
            buffer[(y * width + x) * 4 + 2] = image[(y * width + x) * channels + 2];
            buffer[(y * width + x) * 4 + 3] = channels == 4 ? image[(y * width + x) * channels + 3] : 255;
		}
	}
	stbi_image_free(image);
    return std::make_shared<Texture>(Texture(Surface(width, height, std::move(buffer)), ETextureFormat::RGBA32));
}

std::shared_ptr<Texture> Texture::LoadFrom(const std::string filename)
{
    return LoadFrom(filename.c_str());
}


std::shared_ptr<Texture> Texture::LoadHDRFrom(const std::string filename)
{
    int width;
	int height;
	int channels;
    
    // flip image vertically
    stbi_set_flip_vertically_on_load(true);
    
	float *image = stbi_loadf(filename.c_str(), &width, &height, &channels, STBI_default); // force channels = 4
	if (!image) {
		throw EngineException(__LINE__, __FILE__, "Failed to load image: " + filename);
	}
    
	assert(width > 0 && height > 0 && channels > 0);
    long numBytes = height * width * channels * sizeof(float);
    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(numBytes);
    
    memcpy_s(buffer.get(), numBytes, image, numBytes);
	stbi_image_free(image);
    ETextureFormat textureFormat = channels == 3? ETextureFormat::RGBFloat : ETextureFormat::RGBAFloat;
    return std::make_shared<Texture>(Texture(Surface(width, height, std::move(buffer)), textureFormat, 1, true));
    // THROW_ENGINE_EXCEPTION("Not implemented");
    // return nullptr;
}

Texture * Texture::GetDefaultTexturePtr()
{
    if (pDefaultTexture == nullptr)
    {
        pDefaultTexture = std::make_shared<Texture>(32, 32);
        pDefaultTexture->Clear(Color::White());
    }
    return pDefaultTexture.get();
}


void Texture::releaseMipChain()
{
    for(unsigned int i = 1; i < m_MipChain.size(); ++i)
    {
        if (m_MipChain[i] != nullptr)
            delete m_MipChain[i];
    }
    m_MipChain.clear();
}

void Texture::generateMipChain()
{
    releaseMipChain();
    m_MipChain.resize(m_MipLevel);
    m_MipChain[0] = &m_Surface;
    for(int i = 1; i < m_MipLevel; ++i)
    {
        int width = m_MipChain[i-1]->width / 2;
        int height = m_MipChain[i-1]->height / 2;
        m_MipChain[i] = new Surface(width, height, GetPixelByteSize(m_TextureFormat));
    }
}


CubeTexture::CubeTexture(TextureResDesc desc)
{
    for(int i = 0; i < 6; ++i)
    {
        textures[i] = std::make_shared<Texture>(desc.width, desc.height, desc.format, desc.mipLevels, desc.linear);
    }
}


void CubeTexture::SavePNG(const std::string& path)
{
    for(int i = 0; i < 6; ++i)
    {
        std::string filename = path + "/" + std::to_string(i) + ".png";
        textures[i]->SavePNG(filename);
    }
}
