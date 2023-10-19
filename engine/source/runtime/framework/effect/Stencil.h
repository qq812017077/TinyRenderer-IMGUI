#pragma once

namespace TinyEngine::Rendering
{
    enum class EStencilMode
    {
        Off = 0,
        WriteMask = 1,
        ReadMask = 2
    };
    enum class EStencilOp
    {
        //
        // 摘要:
        //     Keeps the current stencil value.
        Keep,
        //
        // 摘要:
        //     Sets the stencil buffer value to zero.
        Zero,
        //
        // 摘要:
        //     Replace the stencil buffer value with reference value (specified in the shader).
        Replace,
        //
        // 摘要:
        //     Increments the current stencil buffer value. Clamps to the maximum representable
        //     unsigned value.
        IncrementSaturate,
        //
        // 摘要:
        //     Decrements the current stencil buffer value. Clamps to 0.
        DecrementSaturate,
        //
        // 摘要:
        //     Bitwise inverts the current stencil buffer value.
        Invert,
        //
        // 摘要:
        //     Increments the current stencil buffer value. Wraps stencil buffer value to zero
        //     when incrementing the maximum representable unsigned value.
        IncrementWrap,
        //
        // 摘要:
        //     Decrements the current stencil buffer value. Wraps stencil buffer value to the
        //     maximum representable unsigned value when decrementing a stencil buffer value
        //     of zero.
        DecrementWrap
    };
}