#pragma once
#include <string>
#include "settings.h"
#include <memory>

struct Color;

class Surface
{
public:
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
	// Color* GetBufferPtr() noexcept;
	Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	
	bool AlphaLoaded() const noexcept;
	virtual void Save( const std::string& filename ) const;
	virtual void Copy( const Surface& src ) noexcept(!IS_DEBUG);

	static Surface LoadFromFile( const std::string& name);
	static Surface LoadFromFile( const char * name);
	static void SaveToFile( const Surface& surface, const std::string& filename);
private:
	Surface( unsigned int width,unsigned int height,std::unique_ptr<Color[]> pBufferParam ) noexcept;


	std::unique_ptr<Color[]> pBuffer;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
	bool alphaLoaded = false;
};