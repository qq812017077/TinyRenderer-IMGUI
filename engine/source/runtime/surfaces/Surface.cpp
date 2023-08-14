#include "Surface.h"
#include <cassert>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Surface::Surface( unsigned int width, unsigned int height):
	width(width),
	height(height),
    pBuffer(std::make_unique<Color[]>(width * height))
{
	
}

Surface::Surface( Surface && source ) noexcept
	:pBuffer( std::move( source.pBuffer ) ),
	width( source.width ),
	height( source.height )
{}

Surface::~Surface()
{
}

void Surface::Clear( Color fillValue ) noexcept
{
	memset( pBuffer.get(),fillValue.dword,width * height * sizeof( Color ) );
}

void Surface::PutPixel( unsigned int x,unsigned int y,Color c ) noexcept(!IS_DEBUG)
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < width );
	assert( y < height );
	pBuffer[y * width + x] = c;
}

Surface::Color Surface::GetPixel( unsigned int x,unsigned int y ) const noexcept(!IS_DEBUG)
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < width );
	assert( y < height );
	return pBuffer[y * width + x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return pBuffer.get();
}

void Surface::Save(const std::string& filename) const
{

}
void Surface::Copy( const Surface & src ) noexcept(!IS_DEBUG)
{
	assert( width == src.width );
	assert( height == src.height );
	memcpy( pBuffer.get(),src.pBuffer.get(),width * height * sizeof( Color ) );
}

/*  static methods*/
Surface Surface::LoadFromFile(const std::string& name)
{
	// unsigned char *img = stbi_load(name, &width, &height, &channels, 0);
	// read file
	std::ifstream in;
	in.open(name, std::ios::binary);
	if (!in.is_open()) {
		throw std::exception("Can't open file");
    }

	SurfaceHeader header;
    in.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (!in.good()) {
        throw std::exception("Can't read file header");
    }

	unsigned int width = header.width;
	unsigned int height = header.height;
	unsigned int bpp = header.bitsperpixel>>3;
	if (width<=0 || height<=0 || (bpp!=GRAYSCALE && bpp!=RGB && bpp!=RGBA))
	{
		throw std::exception("Invalid file format");
	}

	std::unique_ptr<Color[]> pBuffer = std::make_unique<Color[]>(width * height);
	// for( unsigned int y = 0; y < height; y++ )
	// {
	// 	for( unsigned int x = 0; x < width; x++ )
	// 	{
	// 		Gdiplus::Color c;
	// 		bitmap.GetPixel( x,y,&c );
	// 		pBuffer[y * width + x] = c.GetValue();
	// 	}
	// }
	return Surface(width, height, std::move(pBuffer));
}


// private methods
Surface::Surface( unsigned int width,unsigned int height,std::unique_ptr<Color[]> pBufferParam ) noexcept
	:width( width ),
	height( height ),
	pBuffer( std::move( pBufferParam ) )
{}