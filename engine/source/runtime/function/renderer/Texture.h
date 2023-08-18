#pragma once
#include <string>
#include <memory>
#include "surfaces/Surface.h"
#include "Color.h"
#include "TextureFormat.h"
#include "SampleMode.h"
class Texture
{
public:
    enum EType
    {
        TEXTURE1D,
        TEXTURE2D,
        TEXTURE3D
    };
    enum ETextureCreationFlags
    {
        None = 0x0,
        MipChain = 0x1,
        DontInitializePixels = 0x4,
        Crunch = 0x40,
        DontUploadUponCreate = 0x400,
        IgnoreMipmapLimit = 0x800
    };
    
public:
    Texture(int width, int height);
    Texture(int width, int height, ETextureFormat textureFormat, int mipCount, bool linear);
    Texture(Surface&& surface);
    Texture(Surface&& surface, ETextureFormat textureFormat, int mipCount, bool linear);
    Texture(Surface& surface) = delete;
    ~Texture();
    
    void LoadImageFrom(const char* path);

    int GetWidth() const;
    int GetHeight() const;
    int GetPitch() const;
    void PutPixel( unsigned int x,unsigned int y,Color c );
	Color GetPixel( unsigned int x,unsigned int y ) const;
    ETextureFormat GetTextureFormat() const;
    int GetMipMapLevels() const;
    bool IsLinear() const;
    Color * GetImageData() const;

    EFilterMode GetFilterMode() const;
    EWrapMode GetWrapMode() const;
    int GetAnisoLevel() const;
    
    void SetFilterMode(EFilterMode filterMode);
    void SetWrapMode(EWrapMode wrapMode);
    void SetAnisoLevel(int anisoLevel);
    void SetMipMapLevel(int mipMapLevel);

    void Clear(Color c);
    static std::shared_ptr<Texture> LoadFrom(const std::string name);
    static std::shared_ptr<Texture> LoadFrom(const char* name);
    static Texture* GetDefaultTexturePtr();
private:
    std::string name;
    Surface m_Surface;
    ETextureFormat m_TextureFormat;
    EFilterMode m_FilterMode;
    EWrapMode m_WrapMode;
    int m_Anisolevel;
    int m_MipCount;
    bool m_Linear;
    static std::shared_ptr<Texture> pDefaultTexture;
};
