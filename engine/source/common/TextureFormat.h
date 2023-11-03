#pragma once

enum class ETextureFormat
{
    
    //
    // 摘要:
    //     Color texture format, 8-bits per channel.
    RGB24 = 3,
    //
    // 摘要:
    //     Color with alpha texture format, 8-bits per channel.
    RGBA32 = 4,
    //
    // 摘要:
    //     Color with alpha texture format, 8-bits per channel.
    ARGB32 = 5,
    
    //
    // 摘要:
    //     Color and alpha texture format, 4 bit per channel.
    RGBA4444 = 13,
    //
    // 摘要:
    //     Color with alpha texture format, 8-bits per channel.
    BGRA32 = 14,
    
    
    //
    // 摘要:
    //     Scalar (R) texture format, 16 bit floating point.
    RHalf = 0xF,
    //
    // 摘要:
    //     Two color (RG) texture format, 16 bit floating point per channel.
    RGHalf = 0x10,
    //
    // 摘要:
    //     RGB color and alpha texture format, 16 bit floating point per channel.
    RGBAHalf = 17,
    

    /***
     * Float texture formats.
    */
    RFloat = 18,    //     Scalar (R) texture format, 32 bit floating point.
    RGFloat = 19,   //     Two color (RG) texture format, 32 bit floating point per channel.
    RGBFloat = 20,
    RGBAFloat = 21, //     RGB color and alpha texture format, 32-bit floats per channel.
    
};