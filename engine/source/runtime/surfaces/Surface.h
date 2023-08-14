#pragma once
#include <string>
#include "settings.h"
#include <memory>

class Surface
{
public:
	#pragma pack(push,1)
	enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };
	struct SurfaceHeader {
		std::uint8_t  idlength = 0;
		std::uint8_t  colormaptype = 0;
		std::uint8_t  datatypecode = 0;
		std::uint16_t colormaporigin = 0;
		std::uint16_t colormaplength = 0;
		std::uint8_t  colormapdepth = 0;
		std::uint16_t x_origin = 0;
		std::uint16_t y_origin = 0;
		std::uint16_t width = 0;
		std::uint16_t height = 0;
		std::uint8_t  bitsperpixel = 0;
		std::uint8_t  imagedescriptor = 0;
	};
	#pragma pack(pop)
    struct Color
    {
        unsigned int dword;
		constexpr Color() noexcept : dword() {}
		constexpr Color( const Color& col ) noexcept : dword( col.dword ) {}
		constexpr Color( unsigned int dw ) noexcept : dword( dw ) {}

		constexpr Color( unsigned char x,unsigned char r,unsigned char g,unsigned char b ) noexcept : dword( (x << 24u) | (r << 16u) | (g << 8u) | b ){}
		constexpr Color( unsigned char r,unsigned char g,unsigned char b ) noexcept : dword( (255u << 24u) | (r << 16u) | (g << 8u) | b ) {}
		constexpr Color( Color col,unsigned char x ) noexcept : Color( (x << 24u) | col.dword ) {}
		
		Color& operator =( Color color ) noexcept { dword = color.dword; return *this; }
		
		constexpr unsigned char GetR() const noexcept { return (dword >> 16u) & 0xFFu; }
		constexpr unsigned char GetG() const noexcept { return (dword >> 8u) & 0xFFu; }
		constexpr unsigned char GetB() const noexcept { return dword & 0xFFu; }
		constexpr unsigned char GetX() const noexcept { return dword >> 24u; }
		constexpr unsigned char GetA() const noexcept { return GetX(); }

		void SetX( unsigned char x ) noexcept { dword = (dword & 0xFFFFFFu) | (x << 24u); }
		void SetA( unsigned char a ) noexcept { SetX( a ); }
		void SetR( unsigned char r ) noexcept { dword = (dword & 0xFF00FFFFu) | (r << 16u); }
		void SetG( unsigned char g ) noexcept { dword = (dword & 0xFFFF00FFu) | (g << 8u); }
		void SetB( unsigned char b ) noexcept { dword = (dword & 0xFFFFFF00u) | b; }
    };

    Surface( unsigned int width,unsigned int height );
	Surface( Surface&& source ) noexcept;
	Surface( Surface& ) = delete;
	Surface& operator=( Surface&& donor ) noexcept = default;
	Surface& operator=( const Surface& ) = delete;
	~Surface();

	// Pixel operations
	void Clear( Color fillValue ) noexcept;
	void PutPixel( unsigned int x,unsigned int y,Color c );
	Color GetPixel( unsigned int x,unsigned int y ) const;
	
	// Image operations
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	unsigned int GetBytePitch() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	
	bool AlphaLoaded() const noexcept;
	virtual void Save( const std::string& filename ) const;
	virtual void Copy( const Surface& src ) noexcept(!IS_DEBUG);

	static Surface LoadFromFile( const std::string& name);
	static void SaveToFile( const Surface& surface, const std::string& filename);
private:
	Surface( unsigned int width,unsigned int height,std::unique_ptr<Color[]> pBufferParam ) noexcept;


	std::unique_ptr<Color[]> pBuffer;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
	bool alphaLoaded = false;
};