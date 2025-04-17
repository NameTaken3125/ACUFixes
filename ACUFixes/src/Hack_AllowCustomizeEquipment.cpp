#include "pch.h"

#include "Hack_AllowCustomizeEquipment.h"

AllowCustomizeEquipment::AllowCustomizeEquipment()
{
    DEFINE_ADDR(whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState, 0x141A19B30);
    ALLOC(whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState__cave, 0x80, 0x141A19B30);
    LABEL(whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState__return);
    whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState = {
        db(0xE9), RIP(whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState__cave),
        nop(3),
        PutLabel(whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState__return)
    };
    whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState__cave = {
        // EDX==3 means "Is the player on ground, Low Profile?"
        // To that question return "yes, they are".
        "83 FA 03     "                                                                         // - cmp edx,03
        "0F84 11000000"                                                                         // - je 13F70001A
        "85 D2        "                                                                         // - test edx,edx
        "0F85", RIP(whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState__return),  // - jne ACU.exe+1A19B38
        "41 C6 00 01  "                                                                         // - mov byte ptr [r8],01
        "E9", RIP(whenPlayerOnLedgeOrPeakAndCheckingIfInAParticularNavigationState__return),    // - jmp ACU.exe+1A19B38
        "41 C6 00 01  "                                                                         // - mov byte ptr [r8],01
        "C3           "                                                                         // - ret
    };
}
