#include "Texture.h"
#include "Color.h"

std::shared_ptr<Texture> Texture::pDefaultTexture = nullptr;

Texture::Texture(int width, int height):
    Texture(width, height, ETextureFormat::RGBA32, 1, true)
{
}

Texture::Texture(int width, int height, ETextureFormat textureFormat, int mipCount, bool linear):
    m_Surface(width, height), m_TextureFormat(textureFormat), m_MipCount(mipCount), m_Linear(linear)
{
    m_FilterMode = EFilterMode::Bilinear;
    m_WrapMode = EWrapMode::Repeat;
    m_Anisolevel = 1;
    m_MipChain.resize(m_MipCount);
    m_MipChain[0] = &m_Surface;
}
Texture::Texture(Surface&& surface):
    Texture(std::move(surface), ETextureFormat::RGBA32, 1, true)
{
}

Texture::Texture(Surface&& surface, ETextureFormat textureFormat, int mipCount, bool linear):
    m_Surface(std::move(surface)), m_TextureFormat(textureFormat), m_MipCount(mipCount), m_Linear(linear)
{
}

Texture::~Texture()
{
    
}

void Texture::LoadImageFrom(const char* path)
{
    m_Surface = Surface::LoadFromFile(path);
}

int Texture::GetWidth() const
{
    return m_Surface.GetWidth();
}

int Texture::GetHeight() const
{
    return m_Surface.GetHeight();
}

int Texture::GetPitch() const
{
    return m_Surface.GetBytePitch();
}
void Texture::PutPixel( unsigned int x,unsigned int y,Color c )
{
    m_Surface.PutPixel(x, y, c);
}
Color Texture::GetPixel( unsigned int x,unsigned int y ) const
{
    return m_Surface.GetPixel(x, y);
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
    return m_MipCount > 1 || m_MipCount == 0;
}
int Texture::GetMipMapLevels() const
{
    return m_MipCount;
}

bool Texture::IsLinear() const
{
    return m_Linear;
}

Color * Texture::GetImageData() const
{
    return m_Surface.GetBufferPtr();
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
    m_MipCount = mipMapLevel;
    // releaseMipChain();
    // m_MipChain.resize(m_MipCount);
}

void Texture::Clear(Color c)
{
    m_Surface.Clear(c);
}

std::shared_ptr<Texture> Texture::LoadFrom(const char* name)
{
    return std::make_shared<Texture>(Surface::LoadFromFile(name));
}
std::shared_ptr<Texture> Texture::LoadFrom(const std::string name)
{
    return std::make_shared<Texture>(Surface::LoadFromFile(name));
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
    for(int i = 1; i < m_MipCount; ++i)
    {
        if (m_MipChain[i] != nullptr)
            delete m_MipChain[i];
    }
    m_MipChain.clear();
}


void Texture::generateMipChain()
{
    m_MipChain.resize(m_MipCount);
    m_MipChain[0] = &m_Surface;
    for(int i = 1; i < m_MipCount; ++i)
    {
        m_MipChain[i] = new Surface(m_MipChain[i-1]->GetWidth()/2, m_MipChain[i-1]->GetHeight()/2);
        for(int y = 0; y < m_MipChain[i]->GetHeight(); ++y)
        {
            for(int x = 0; x < m_MipChain[i]->GetWidth(); ++x)
            {
                Color c = m_MipChain[i-1]->GetPixel(x*2, y*2);
                Color c1 = m_MipChain[i-1]->GetPixel(x*2+1, y*2);
                Color c2 = m_MipChain[i-1]->GetPixel(x*2, y*2+1);
                Color c3 = m_MipChain[i-1]->GetPixel(x*2+1, y*2+1);
                m_MipChain[i]->PutPixel(x, y, 
                    Color(
                        (c.GetR() + c1.GetR() + c2.GetR() + c3.GetR()) / 4,
                        (c.GetG() + c1.GetG() + c2.GetG() + c3.GetG()) / 4,
                        (c.GetB() + c1.GetB() + c2.GetB() + c3.GetB()) / 4,
                        (c.GetA() + c1.GetA() + c2.GetA() + c3.GetA()) / 4
                    )
                );
            }
        }
    }
}