#pragma once

#include "basic_types.h"
#include "vmath/vmath.h"

#include "HasInputContainers.h"
#include "ActionKeyStates.h"


class InputContainerBig;

class InputContainer
{
public:
    // @members
    char pad_0000[16]; //0x0000
    KeyStates keyStates_prevFrame; //0x0010
    KeyStates keyStates_thisFrame; //0x0030
    char pad_0050[548]; //0x0050
    float fltIsPressed_LMB; //0x0274
    char pad_0278[36]; //0x0278
    float fltIsPressed_RMB; //0x029C
    char pad_02A0[8]; //0x02A0
    float fltIsPressed_reload; //0x02A8
    char pad_02AC[68]; //0x02AC
    Vector2f wasdVector; //0x02F0
    char pad_02F8[8]; //0x02F8
    Vector2f mouseMovementVector; //0x0300
    char pad_0308[1320]; //0x0308
    InputContainerBig* pendingFrame_mb; //0x0830
    char pad_0838[56]; //0x0838

    // @helper_functions

    // Not sure if this really never fails.
    static InputContainer& GetMainSingleton() { return *HasInputContainers::GetSingleton()->p_10->inputContainer; }
    InputContainerBig* GetInputContainerBig() { return pendingFrame_mb; }
}; //Size: 0x0870
assert_offsetof(InputContainer, keyStates_thisFrame, 0x30);
assert_offsetof(InputContainer, wasdVector, 0x02F0);
assert_offsetof(InputContainer, pendingFrame_mb, 0x830);
assert_sizeof(InputContainer, 0x0870);

class InputContainerBig : public InputContainer
{
public:
    // @members
    char pad_0870[80]; //0x0870
    uint8 pad8C0_21ff0[137008]; //0x08C0
    char pad_21FF0[188]; //0x21FF0
    int32 mouseDeltaInt_X; //0x220AC
    int32 mouseDeltaInt_Y; //0x220B0
    int32 mouseWheelDeltaInt; //0x220B4
    char pad_220B8[8]; //0x220B8
    uint8 isPressed_byScancode[256]; //0x220C0
    char pad_221C0[256]; //0x221C0
    uint8 isNewInputAvailable_mb; //0x222C0
    char pad_222C1[15]; //0x222C1
    uint32 displayWithoutLetterBoxWidth; //0x222D0
    uint32 displayWithoutLetterBoxHeight; //0x222D4
    char pad_222D8[8]; //0x222D8

    // @members
    bool IsMouseWheelScrollingUp() { return mouseWheelDeltaInt > 0; }
    bool IsMouseWheelScrollingDown() { return mouseWheelDeltaInt < 0; }
}; //Size: 0x222E0
assert_offsetof(InputContainerBig, isPressed_byScancode, 0x220C0);
assert_offsetof(InputContainerBig, mouseWheelDeltaInt, 0x220B4);
assert_sizeof(InputContainerBig, 0x222E0);
