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
        pDefaultTexture->Clear(Color::White());
    }
    return pDefaultTexture.get();
}