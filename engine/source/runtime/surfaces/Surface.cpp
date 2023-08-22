#include "Surface.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <core/EngineException.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "Color.h"
#include <string>
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

Color Surface::GetPixel( unsigned int x,unsigned int y ) const noexcept(!IS_DEBUG)
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
unsigned int Surface::GetBytePitch() const noexcept
{
	return width * sizeof( Color );
}

// Color* Surface::GetBufferPtr() noexcept
// {
// 	return pBuffer.get();
// }

Color* Surface::GetBufferPtr() const noexcept
{
	return pBuffer.get();
}

const Color* Surface::GetBufferPtrConst() const noexcept
{
	return pBuffer.get();
}

bool Surface::AlphaLoaded() const noexcept
{
	return alphaLoaded;
}

/**
 * @brief save surface to target file
 * 
 * @param filename 
 */
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
Surface Surface::LoadFromFile(const std::string& filename)
{
	return LoadFromFile(filename.c_str());
}

Surface Surface::LoadFromFile(const char* filename)
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
	return Surface(width, height, std::move(pBuffer));
}

// private methods
Surface::Surface( unsigned int width,unsigned int height,std::unique_ptr<Color[]> pBufferParam ) noexcept
	:width( width ),
	height( height ),
	pBuffer( std::move( pBufferParam ) )
{}