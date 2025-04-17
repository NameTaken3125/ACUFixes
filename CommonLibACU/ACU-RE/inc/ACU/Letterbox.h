#pragma once

#include "ACU/basic_types.h"
#include "vmath/vmath.h"

class HasRenderTargetSizeAccountingForLetterbox
{
public:
    // @members
    char pad_0000[40]; //0x0000
    Vector2f displaySizeWithoutLetterbox; //0x0028

    // @helper_functions
    static HasRenderTargetSizeAccountingForLetterbox* GetSingleton() {
        return *(HasRenderTargetSizeAccountingForLetterbox**)0x14524CFD8;
    }
}; //Size: 0x0030
assert_offsetof(HasRenderTargetSizeAccountingForLetterbox, displaySizeWithoutLetterbox, 0x28);
assert_sizeof(HasRenderTargetSizeAccountingForLetterbox, 0x30);
