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
    static Color White() noexcept { return Color( 1.0f, 1.0f, 1.0f ); }
    static Color Black() noexcept { return Color( 0.0f, 0.0f, 0.0f ); }
    static Color Red() noexcept { return Color( 1.0f, 0.0f, 0.0f ); }
    static Color Green() noexcept { return Color( 0.0f, 1.0f, 0.0f ); }
    static Color Blue() noexcept { return Color( 0.0f, 0.0f, 1.0f ); }
    static Color Yellow() noexcept { return Color( 1.0f, 1.0f, 0.0f ); }
    static Color Cyan() noexcept { return Color( 0.0f, 1.0f, 1.0f ); }
    static Color Magenta() noexcept { return Color( 1.0f, 0.0f, 1.0f ); }
    static Color Gray() noexcept { return Color( 0.5f, 0.5f, 0.5f ); }
    static Color LightGray() noexcept { return Color( 0.75f, 0.75f, 0.75f ); }
    static Color DarkGray() noexcept { return Color( 0.25f, 0.25f, 0.25f ); }
    static Color Orange() noexcept { return Color( 1.0f, 0.65f, 0.0f ); }
    static Color Brown() noexcept { return Color( 0.5f, 0.16f, 0.16f ); }
    static Color Pink() noexcept { return Color( 1.0f, 0.75f, 0.79f ); }
    static Color Transparent() noexcept { return Color( 0.0f, 0.0f, 0.0f, 0.0f ); }
    
    float r, g, b, a;
    Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
    Color(unsigned int color)
    {
        r = (float)((color >> 24) & 0xFF) / 255.0f;
        g = (float)((color >> 16) & 0xFF) / 255.0f;
        b = (float)((color >> 8) & 0xFF) / 255.0f;
        a = (float)(color & 0xFF) / 255.0f;
    }
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        this->r = (float)r / 255.0f;
        this->g = (float)g / 255.0f;
        this->b = (float)b / 255.0f;
        this->a = (float)a / 255.0f;
    }
    Color(unsigned char r, unsigned char g, unsigned char b)
    {
        this->r = (float)r / 255.0f;
        this->g = (float)g / 255.0f;
        this->b = (float)b / 255.0f;
        this->a = 1.0f;
    }
    Color(const Color& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
    }

    Color& operator=(const Color& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    float GetRf() const noexcept { return r; }
    float GetGf() const noexcept { return g; }
    float GetBf() const noexcept { return b; }
    float GetAf() const noexcept { return a; }

    unsigned int dword()
    {
        unsigned int color = 0;
        color |= ((unsigned int)(r * 255.0f));
        color |= ((unsigned int)(g * 255.0f) << 8);
        color |= ((unsigned int)(b * 255.0f) << 16);
        color |= ((unsigned int)(a * 255.0f) << 24);
        return color;
    
    } 
};

// struct Color
// {
//     static Color White() noexcept { return Color( 255,255,255 ); }
//     static Color Black() noexcept { return Color( 0,0,0 ); }
//     static Color Red() noexcept { return Color( 255,0,0 ); }
//     static Color Green() noexcept { return Color( 0,255,0 ); }
//     static Color Blue() noexcept { return Color( 0,0,255 ); }
//     static Color Yellow() noexcept { return Color( 255,255,0 ); }
//     static Color Cyan() noexcept { return Color( 0,255,255 ); }
//     static Color Magenta() noexcept { return Color( 255,0,255 ); }
//     static Color Gray() noexcept { return Color( 128,128,128 ); }
//     static Color LightGray() noexcept { return Color( 192,192,192 ); }
//     static Color DarkGray() noexcept { return Color( 64,64,64 ); }
//     static Color Orange() noexcept { return Color( 255,165,0 ); }
//     static Color Brown() noexcept { return Color( 128,42,42 ); }
//     static Color Pink() noexcept { return Color( 255,192,203 ); }
//     static Color Transparent() noexcept { return Color( 0,0,0,0 ); }

//     unsigned int dword;
//     constexpr Color() noexcept : dword() {}
//     constexpr Color( const Color& col ) noexcept : dword( col.dword ) {}
//     constexpr Color( unsigned int dw ) noexcept : dword( dw ) {}

//     constexpr Color( unsigned char r,unsigned char g,unsigned char b, unsigned char x) noexcept : 
//         dword( r | (g << 8u) | (b << 16u) | (x << 24u) ) {} 
//     constexpr Color( unsigned char r,unsigned char g,unsigned char b ) noexcept : 
//         dword( r | (g << 8u) | (b << 16u) | (255u << 24u) ) {}
//     constexpr Color( Color col,unsigned char x ) noexcept : 
//         Color( col.GetR(),col.GetG(),col.GetB(),x ) {}
//     constexpr Color(float * rgba):
//         Color( static_cast<unsigned char>(rgba[0] * 255.0f), 
//                static_cast<unsigned char>(rgba[1] * 255.0f), 
//                static_cast<unsigned char>(rgba[2] * 255.0f), 
//                static_cast<unsigned char>(rgba[3] * 255.0f) ) {}

//     Color& operator =( Color color ) noexcept { dword = color.dword; return *this; }
    
//     constexpr unsigned char GetR() const noexcept { return (dword & 0xFFu); }
//     constexpr unsigned char GetG() const noexcept { return (dword >> 8u) & 0xFFu; }
//     constexpr unsigned char GetB() const noexcept { return (dword >> 16u) & 0xFFu; }
//     constexpr unsigned char GetX() const noexcept { return (dword >> 24u) & 0xFFu; }
//     constexpr unsigned char GetA() const noexcept { return GetX(); }
    

//     float GetRf() const noexcept { return GetR() / 255.0f; }
//     float GetGf() const noexcept { return GetG() / 255.0f; }
//     float GetBf() const noexcept { return GetB() / 255.0f; }
//     float GetAf() const noexcept { return GetA() / 255.0f; }

//     void SetX( unsigned char x ) noexcept { dword = (dword & 0xFFFFFFu) | (x << 24u); }
//     void SetA( unsigned char a ) noexcept { SetX( a ); }
//     void SetB( unsigned char b ) noexcept { dword = (dword & 0xFF00FFFFu) | (b << 16u); }
//     void SetG( unsigned char g ) noexcept { dword = (dword & 0xFFFF00FFu) | (g << 8u); }
//     void SetR( unsigned char r ) noexcept { dword = (dword & 0xFFFFFF00u) | r; }
//     void SetRGB( unsigned char r,unsigned char g,unsigned char b ) noexcept { SetR( r ); SetG( g ); SetB( b ); }
//     void SetRGBA(unsigned char r,unsigned char g,unsigned char b, unsigned char a) noexcept { SetR( r ); SetG( g ); SetB( b ); SetA( a ); }
// };