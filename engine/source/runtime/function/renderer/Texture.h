#pragma once
#include <string>
#include <memory>
#include <vector>
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
    Texture(Texture&& texture) noexcept;
    ~Texture();
    void LoadImageFrom(const char* path);
    int GetWidth() const;
    int GetHeight() const;
    int GetPitch() const;
    void PutPixel( unsigned int x,unsigned int y,Color c );
	Color GetPixel( unsigned int x,unsigned int y ) const;
    ETextureFormat GetTextureFormat() const;
    int GetMipMapLevels() const;
    bool UseMipMap() const;
    bool IsLinear() const;
    bool HasAlphaChannel() const;
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
    class Surface
    {
    public:
        Surface( unsigned int width, unsigned int height, int channels=3):
            Surface(width, height, std::make_unique<Color[]>(width * height), channels)
        {}
        Surface(Surface && source ) noexcept : Surface( source.width,source.height,std::move( source.pBuffer ), source.channels )
        {}

        Surface( unsigned int width,unsigned int height,std::unique_ptr<Color[]> pBufferParam, int channels) noexcept:
            width( width ), height( height ), pBuffer( std::move( pBufferParam)), channels(channels){}
        Surface::~Surface() {}
        Surface( Surface& ) = delete;
        Surface& operator=( Surface&& donor ) noexcept = default;
        Surface& operator=( const Surface& ) = delete;

        bool AlphaLoaded() const noexcept {return channels == 4;}
        // virtual void Copy( const Surface& src ) noexcept(!IS_DEBUG);

        std::string path;
        std::unique_ptr<Color[]> pBuffer;
        unsigned int width;
        unsigned int height;
        unsigned int pitch;
        int channels;
    };
    Texture(Surface&& surface);
    Texture(Surface&& surface, ETextureFormat textureFormat, int mipCount, bool linear);

    void releaseMipChain();
    void generateMipChain();
private:
    

    std::string name;
    Surface m_Surface;
    std::vector<Surface *> m_MipChain;
    ETextureFormat m_TextureFormat;
    EFilterMode m_FilterMode;
    EWrapMode m_WrapMode;
    int m_Anisolevel;
    int m_MipLevel;
    bool m_Linear;
    static std::shared_ptr<Texture> pDefaultTexture;
};

class CubeTexture
{
public:
    CubeTexture() = default;
// in directx :
/***
 *  In directx:
 *      0: right
 *      1: left
 *      2: top
 *      3: bottom
 *      4: front
 *      5: back
*/
    std::shared_ptr<Texture> front() const { return textures[4]; }
    std::shared_ptr<Texture> back() const { return textures[5]; }
    std::shared_ptr<Texture> left() const { return textures[1]; }
    std::shared_ptr<Texture> right() const { return textures[0]; }
    std::shared_ptr<Texture> top() const { return textures[2]; }
    std::shared_ptr<Texture> bottom() const { return textures[3]; }
    
    std::shared_ptr<Texture> textures[6];
    
};