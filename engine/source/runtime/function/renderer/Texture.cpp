#include "Texture.h"
#include "Color.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <core/EngineException.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
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
    case ETextureFormat::RGBFloat:
        return 3 * sizeof(float);
    }
    THROW_ENGINE_EXCEPTION("unknowned texture format. cannot get channel");
    return 0;
}

std::shared_ptr<Texture> Texture::pDefaultTexture = nullptr;

Texture::Texture(int width, int height):
    Texture(width, height, ETextureFormat::RGBA32, 1, true)
{
}

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
    Texture(std::move(surface), textureFormat, 0, false)
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
    
}

void Texture::LoadImageFrom(const char* path)
{

}

int Texture::GetWidth() const
{
    return m_Surface.width;
}

int Texture::GetHeight() const
{
    return m_Surface.height;
}

int Texture::GetPitch() const
{
    switch (m_TextureFormat)
    {
    case ETextureFormat::RGBA32:
        return m_Surface.width * 4 * sizeof(uint8_t);
    case ETextureFormat::RGB24:
        return m_Surface.width * 3 * sizeof(uint8_t);
    case ETextureFormat::RGBAFloat:
        return m_Surface.width * 4 * sizeof(float);
    case ETextureFormat::RGBFloat:
        return m_Surface.width * 3 * sizeof(float);
    }
    THROW_ENGINE_EXCEPTION("unknowned texture format. cannot get pitch");
    return 0;
    // return m_Surface.width * sizeof( Color );
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
uint8_t * Texture::GetImageData() const
{
    return m_Surface.pBuffer.get();
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
    // releaseMipChain();
    // m_MipChain.resize(m_MipCount);
}

void Texture::Clear(Color fillValue)
{
    switch (m_TextureFormat)
    {
    
    case ETextureFormat::RGB24:
    case ETextureFormat::RGBA32:
        memset(m_Surface.pBuffer.get(), fillValue.dword(), m_Surface.width * m_Surface.height * 4 * sizeof(unsigned char));
        break;
    case ETextureFormat::RGBAFloat:
        THROW_ENGINE_EXCEPTION("Not implemented");
        break;
    case ETextureFormat::RGBFloat:
        THROW_ENGINE_EXCEPTION("Not implemented");
        break;
    }
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
	free(image);
    auto pTex = std::make_shared<Texture>(Texture(Surface(width, height, std::move(buffer)), ETextureFormat::RGBA32));
    return pTex;
}

std::shared_ptr<Texture> Texture::LoadFrom(const std::string name)
{
    return LoadFrom(name.c_str());
}


std::shared_ptr<Texture> Texture::LoadHDRFrom(const std::string filename)
{
    THROW_ENGINE_EXCEPTION("Not implemented");
    return nullptr;
}

Texture * Texture::GetDefaultTexturePtr()
{
    if (pDefaultTexture == nullptr)
    {
        pDefaultTexture = std::make_shared<Texture>(512, 512);
        pDefaultTexture->Clear(Color::DarkGray());
    }
    return pDefaultTexture.get();
}

void Texture::releaseMipChain()
{
    for(int i = 1; i < m_MipLevel; ++i)
    {
        if (m_MipChain[i] != nullptr)
            delete m_MipChain[i];
    }
    m_MipChain.clear();
}


void Texture::generateMipChain()
{
    m_MipChain.resize(m_MipLevel);
    m_MipChain[0] = &m_Surface;
    for(int i = 1; i < m_MipLevel; ++i)
    {
    }
}


