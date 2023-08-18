#pragma once

// #define SETA(x) (x << 24u)
// #define SETB(x) (x << 16u)
// #define SETG(x) (x << 8u)
// #define SETR(x) (x)

// #define GETA(x) ((x >> 24u) & 0xFFu)
// #define GETB(x) ((x >> 16u) & 0xFFu)
// #define GETG(x) ((x >> 8u) & 0xFFu)
// #define GETR(x) (x & 0xFFu)

// R8G8B8A8 color
struct Color
{
    static Color White() noexcept { return Color( 255,255,255 ); }
    static Color Black() noexcept { return Color( 0,0,0 ); }
    static Color Red() noexcept { return Color( 255,0,0 ); }
    static Color Green() noexcept { return Color( 0,255,0 ); }
    static Color Blue() noexcept { return Color( 0,0,255 ); }
    static Color Yellow() noexcept { return Color( 255,255,0 ); }
    static Color Cyan() noexcept { return Color( 0,255,255 ); }
    static Color Magenta() noexcept { return Color( 255,0,255 ); }

    unsigned int dword;
    constexpr Color() noexcept : dword() {}
    constexpr Color( const Color& col ) noexcept : dword( col.dword ) {}
    constexpr Color( unsigned int dw ) noexcept : dword( dw ) {}

    constexpr Color( unsigned char r,unsigned char g,unsigned char b, unsigned char x) noexcept : 
        dword( r | (g << 8u) | (b << 16u) | (x << 24u) ) {} 
    constexpr Color( unsigned char r,unsigned char g,unsigned char b ) noexcept : 
        dword( r | (g << 8u) | (b << 16u) | (255u << 24u) ) {}
    constexpr Color( Color col,unsigned char x ) noexcept : 
        Color( col.GetR(),col.GetG(),col.GetB(),x ) {}
    
    Color& operator =( Color color ) noexcept { dword = color.dword; return *this; }
    
    constexpr unsigned char GetR() const noexcept { return (dword & 0xFFu); }
    constexpr unsigned char GetG() const noexcept { return (dword >> 8u) & 0xFFu; }
    constexpr unsigned char GetB() const noexcept { return (dword >> 16u) & 0xFFu; }
    constexpr unsigned char GetX() const noexcept { return (dword >> 24u) & 0xFFu; }
    constexpr unsigned char GetA() const noexcept { return GetX(); }
    
    void SetX( unsigned char x ) noexcept { dword = (dword & 0xFFFFFFu) | (x << 24u); }
    void SetA( unsigned char a ) noexcept { SetX( a ); }
    void SetB( unsigned char b ) noexcept { dword = (dword & 0xFF00FFFFu) | (b << 16u); }
    void SetG( unsigned char g ) noexcept { dword = (dword & 0xFFFF00FFu) | (g << 8u); }
    void SetR( unsigned char r ) noexcept { dword = (dword & 0xFFFFFF00u) | r; }
    void SetRGB( unsigned char r,unsigned char g,unsigned char b ) noexcept { SetR( r ); SetG( g ); SetB( b ); }
    void SetRGBA(unsigned char r,unsigned char g,unsigned char b, unsigned char a) noexcept { SetR( r ); SetG( g ); SetB( b ); SetA( a ); }
};