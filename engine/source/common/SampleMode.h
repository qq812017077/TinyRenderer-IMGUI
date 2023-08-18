#pragma once

//
//Filtering mode for textures. Corresponds to the settings in a.
enum EFilterMode
{
    //
    // 摘要:
    //     Point filtering - texture pixels become blocky up close.
    Point,
    //
    // 摘要:
    //     Bilinear filtering - texture samples are averaged.
    Bilinear,
    //
    // 摘要:
    //     Trilinear filtering - texture samples are averaged and also blended between mipmap
    //     levels.
    Trilinear,
    //
    // 摘要:
    //     Anisotropic filtering - texture samples are chosen depending on the angle of the
    //     surface.
    Anisotropic
};

enum EWrapMode
{
    //
    // 摘要:
    //     Tiles the texture, creating a repeating pattern.
    Repeat,
    //
    // 摘要:
    //     Clamps the texture to the last pixel at the edge.
    Clamp,
    //
    // 摘要:
    //     Tiles the texture, creating a repeating pattern by mirroring it at every integer
    //     boundary.
    Mirror,
    //
    // 摘要:
    //     Mirrors the texture once, then clamps to edge pixels.
    MirrorOnce,
    //
    // 摘要:
    //     Clamps the texture to the border color.
    Border
};