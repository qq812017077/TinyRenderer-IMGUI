#pragma once

enum ETextureFormat
    {
        //
        // 摘要:
        //     Alpha-only texture format, 8 bit integer.
        Alpha8 = 1,
        //
        // 摘要:
        //     A 16 bits/pixel texture format. Texture stores color with an alpha channel.
        ARGB4444 = 2,
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
        //     A 16 bit color texture format.
        RGB565 = 7,
        //
        // 摘要:
        //     Single channel (R) texture format, 16 bit integer.
        R16 = 9,
        //
        // 摘要:
        //     Compressed color texture format.
        DXT1 = 10,
        //
        // 摘要:
        //     Compressed color with alpha channel texture format.
        DXT5 = 12,
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
        //
        // 摘要:
        //     Scalar (R) texture format, 32 bit floating point.
        RFloat = 18,
        //
        // 摘要:
        //     Two color (RG) texture format, 32 bit floating point per channel.
        RGFloat = 19,
        //
        // 摘要:
        //     RGB color and alpha texture format, 32-bit floats per channel.
        RGBAFloat = 20,
        //
        // 摘要:
        //     A format that uses the YUV color space and is often used for video encoding or
        //     playback.
        YUY2 = 21,
        //
        // 摘要:
        //     RGB HDR format, with 9 bit mantissa per channel and a 5 bit shared exponent.
        RGB9e5Float = 22,
        //
        // 摘要:
        //     Compressed one channel (R) texture format.
        BC4 = 26,
        //
        // 摘要:
        //     Compressed two-channel (RG) texture format.
        BC5 = 27,
        //
        // 摘要:
        //     HDR compressed color texture format.
        BC6H = 24,
        //
        // 摘要:
        //     High quality compressed color texture format.
        BC7 = 25,
        //
        // 摘要:
        //     Compressed color texture format with Crunch compression for smaller storage sizes.
        DXT1Crunched = 28,
        //
        // 摘要:
        //     Compressed color with alpha channel texture format with Crunch compression for
        //     smaller storage sizes.
        DXT5Crunched = 29,
        //
        // 摘要:
        //     PowerVR (iOS) 2 bits/pixel compressed color texture format.
        PVRTC_RGB2 = 30,
        //
        // 摘要:
        //     PowerVR (iOS) 2 bits/pixel compressed with alpha channel texture format.
        PVRTC_RGBA2 = 0x1F,
        //
        // 摘要:
        //     PowerVR (iOS) 4 bits/pixel compressed color texture format.
        PVRTC_RGB4 = 0x20,
        //
        // 摘要:
        //     PowerVR (iOS) 4 bits/pixel compressed with alpha channel texture format.
        PVRTC_RGBA4 = 33,
        //
        // 摘要:
        //     ETC (GLES2.0) 4 bits/pixel compressed RGB texture format.
        ETC_RGB4 = 34,
        //
        // 摘要:
        //     ETC2 EAC (GL ES 3.0) 4 bitspixel compressed unsigned single-channel texture format.
        EAC_R = 41,
        //
        // 摘要:
        //     ETC2 EAC (GL ES 3.0) 4 bitspixel compressed signed single-channel texture format.
        EAC_R_SIGNED = 42,
        //
        // 摘要:
        //     ETC2 EAC (GL ES 3.0) 8 bitspixel compressed unsigned dual-channel (RG) texture
        //     format.
        EAC_RG = 43,
        //
        // 摘要:
        //     ETC2 EAC (GL ES 3.0) 8 bitspixel compressed signed dual-channel (RG) texture
        //     format.
        EAC_RG_SIGNED = 44,
        //
        // 摘要:
        //     ETC2 (GL ES 3.0) 4 bits/pixel compressed RGB texture format.
        ETC2_RGB = 45,
        //
        // 摘要:
        //     ETC2 (GL ES 3.0) 4 bits/pixel RGB+1-bit alpha texture format.
        ETC2_RGBA1 = 46,
        //
        // 摘要:
        //     ETC2 (GL ES 3.0) 8 bits/pixel compressed RGBA texture format.
        ETC2_RGBA8 = 47,
        //
        // 摘要:
        //     ASTC (4x4 pixel block in 128 bits) compressed RGB(A) texture format.
        ASTC_4x4 = 48,
        //
        // 摘要:
        //     ASTC (5x5 pixel block in 128 bits) compressed RGB(A) texture format.
        ASTC_5x5 = 49,
        //
        // 摘要:
        //     ASTC (6x6 pixel block in 128 bits) compressed RGB(A) texture format.
        ASTC_6x6 = 50,
        //
        // 摘要:
        //     ASTC (8x8 pixel block in 128 bits) compressed RGB(A) texture format.
        ASTC_8x8 = 51,
        //
        // 摘要:
        //     ASTC (10x10 pixel block in 128 bits) compressed RGB(A) texture format.
        ASTC_10x10 = 52,
        //
        // 摘要:
        //     ASTC (12x12 pixel block in 128 bits) compressed RGB(A) texture format.
        ASTC_12x12 = 53,
        //
        // 摘要:
        //     Two color (RG) texture format, 8-bits per channel.
        RG16 = 62,
        //
        // 摘要:
        //     Single channel (R) texture format, 8 bit integer.
        R8 = 0x3F,
        //
        // 摘要:
        //     Compressed color texture format with Crunch compression for smaller storage sizes.
        ETC_RGB4Crunched = 0x40,
        //
        // 摘要:
        //     Compressed color with alpha channel texture format using Crunch compression for
        //     smaller storage sizes.
        ETC2_RGBA8Crunched = 65,
        //
        // 摘要:
        //     ASTC (4x4 pixel block in 128 bits) compressed RGB(A) HDR texture format.
        ASTC_HDR_4x4 = 66,
        //
        // 摘要:
        //     ASTC (5x5 pixel block in 128 bits) compressed RGB(A) HDR texture format.
        ASTC_HDR_5x5 = 67,
        //
        // 摘要:
        //     ASTC (6x6 pixel block in 128 bits) compressed RGB(A) HDR texture format.
        ASTC_HDR_6x6 = 68,
        //
        // 摘要:
        //     ASTC (8x8 pixel block in 128 bits) compressed RGB(A) texture format.
        ASTC_HDR_8x8 = 69,
        //
        // 摘要:
        //     ASTC (10x10 pixel block in 128 bits) compressed RGB(A) HDR texture format.
        ASTC_HDR_10x10 = 70,
        //
        // 摘要:
        //     ASTC (12x12 pixel block in 128 bits) compressed RGB(A) HDR texture format.
        ASTC_HDR_12x12 = 71,
        //
        // 摘要:
        //     Two channel (RG) texture format, 16 bit integer per channel.
        RG32 = 72,
        //
        // 摘要:
        //     Three channel (RGB) texture format, 16 bit integer per channel.
        RGB48 = 73,
        //
        // 摘要:
        //     Four channel (RGBA) texture format, 16 bit integer per channel.
        RGBA64 = 74,
    };