#include "pch.h"

#include "Hack_AimFromPeaks.h"
#include "ACU/basic_types.h"


AimFromPeaks::AimFromPeaks()
{
    // Crashes if tried on slopes and in V-Shapes such as trees, flagpoles.
    //DEFINE_ADDR(whenTryToStartAimingFromPeak, 0x1419158E0);
    //DEFINE_ADDR(whenTryToStartAimingFromLedge, 0x141A262A0);
    //DEFINE_ADDR(whenTryToStartAimingFromGround, 0x141A0DAB0);
    //whenTryToStartAimingFromPeak = {
    //    0xE9, RIP(whenTryToStartAimingFromLedge)
    //};


    /*
    When trying to aim bombs from a peak (normally not allowed), I call a function that is used
    when trying to aim from a ledge (normally allowed).
    When trying to aim from a peak: 0x1419158E0 ("Empty callback")
    When trying to aim from a ledge: 0x141A262A0 ("Working callback")
    The callback that is used on peaks returns almost immediately, and doesn't start aiming.
    The hack constists of jumping from the "Empty callback" immediately into the "Working callback".
    The tricky thing is that the "Empty callback" is also triggered when trying to aim from a slope
    or from a V-shape such as a tree, but in these cases jumping into the "Working callback" results in a crash.
    I need to distinguish peaks from slopes and V-shapes, and my chosen method
    is by checking the value stored at [RCX+0xE0].
    */

    /*
    BUG:
    If starting to aim from a peak after _just_ climbed on top of it,
    the perching animation will look strange: Arno will either look as if he's crouched on ground
    or as if he's standing on ground.
    Two ways to fix this strangeness:
    a) Wait for like a second (for the perching animation to "settle");
    b) Move around on the perch just a little bit.
    */


    DEFINE_ADDR(whenTryToStartAimingFromPeak, 0x1419158E0);
    DEFINE_ADDR(whenTryToStartAimingFromLedge, 0x141A262A0);
    ALLOC(whenTryToStartAimingFromPeak_cave, 0x80, 0x1419158E0);
    constexpr uint64 whileOnPeak_0xE0 = 0x141A5DC00;
    LABEL(whileOnPeak_0xE0__storage);

    whenTryToStartAimingFromPeak = {
        db(0xE9), RIP(whenTryToStartAimingFromPeak_cave)
    };
    whenTryToStartAimingFromPeak_cave = {
        "48 8B 81 E0000000"                             //- mov rax,[rcx+000000E0]
        "48 3B 05 0B000000"                             //- cmp rax,[whileOnPeak_0xE0__storage]
        "0F84", RIP(whenTryToStartAimingFromLedge),     //- je `whenTryToStartAimingFromLedge`
        "41 C6 01 00"                                   //- mov byte ptr [r9],00
        "C3"                                            //- ret
        , PutLabel(whileOnPeak_0xE0__storage)
        , dq(whileOnPeak_0xE0)
    };
}
