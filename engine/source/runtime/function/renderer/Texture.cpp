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

std::shared_ptr<Texture> Texture::pDefaultTexture = nullptr;

Texture::Texture(int width, int height):
    Texture(width, height, ETextureFormat::RGBA32, 1, true)
{
}

Texture::Texture(int width, int height, ETextureFormat textureFormat, int mipCount, bool linear):
    Texture(Surface(width, height), textureFormat, mipCount, linear)
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

Texture::Texture(Surface&& surface):
    Texture(std::move(surface), surface.AlphaLoaded() ? ETextureFormat::RGBA32 : ETextureFormat::RGB24, 0, false)
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
    return m_Surface.width * sizeof( Color );
}
void Texture::PutPixel( unsigned int x,unsigned int y,Color c )
{
    
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < m_Surface.width );
	assert( y < m_Surface.height );
	m_Surface.pBuffer[y * m_Surface.width + x] = c;
}
Color Texture::GetPixel( unsigned int x,unsigned int y ) const
{
    assert( x >= 0 );
	assert( y >= 0 );
	assert( x < m_Surface.width );
	assert( y < m_Surface.height );
	return m_Surface.pBuffer[y * m_Surface.width + x];
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
Color * Texture::GetImageData() const
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
    memset( m_Surface.pBuffer.get(), fillValue.dword, m_Surface.width * m_Surface.height * sizeof( Color ));
}

std::shared_ptr<Texture> Texture::LoadFrom(const char* filename)
{
    int width;
	int height;
	int channels;
	unsigned char *image = stbi_load(filename, &width, &height, &channels, STBI_default);
	if (!image) {
		// throw std::runtime_error("Failed to load image: " + filename);
		char error[50] = "Failed to load image: ";
		strcat_s(error, sizeof(error), filename);
		throw EngineException(__LINE__, __FILE__, error);
	}
    
	assert(width > 0 && height > 0 && channels > 0);
	
	std::unique_ptr<Color[]> pBuffer = std::make_unique<Color[]>(width * height);
	
	Color c;
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			auto r = image[(y * width + x) * channels + 0];
			auto g = image[(y * width + x) * channels + 1];
			auto b = image[(y * width + x) * channels + 2];
			auto a = channels == 4 ? image[(y * width + x) * channels + 3] : 255;
			// save the pixel to our buffer
			pBuffer[y * width + x] = Color(r, g, b, a);
		}
	}
	free(image);
    return std::make_shared<Texture>(Texture(Surface(width, height, std::move(pBuffer), channels)));
}
std::shared_ptr<Texture> Texture::LoadFrom(const std::string name)
{
    return LoadFrom(name.c_str());
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

// void Surface::Copy( const Surface & src ) noexcept(!IS_DEBUG)
// {
// 	assert( width == src.width );
// 	assert( height == src.height );
// 	memcpy( pBuffer.get(),src.pBuffer.get(),width * height * sizeof( Color ) );
// }


