#pragma once

#include "basic_types.h"

/*
I think these are a sort of reimplementation of Virtual Function Tables
with ability to contain some extra (static, unchanging) data.
You can look up the "virtual function" by index or, I guess, iterate over all
functions in the "fancy vtable" to see which ones are used at all.
These are used in different ways in different sections
of the code.
Many objects such as Human State Nodes (`FunctorBase` and subclasses),
PotentialParkourActions, ParkourActionScanners have a pointer to a regular static VTable
alongside a pointer to static tables (arrays) of these "Fancy Virtual Functions".


Things discovered:
- Tables of these functions are plain C arrays, sizes of tables are hardcoded, tables don't seem to change at runtime.
- Offsets in these tables are hardcoded, meaning that they can be (sometimes are) used just like virtual functions:
    mov rax, [rcx+8]        // if the `fancyVTable` is at offset 8
    call [rax+8B0]
- The `idx` member of the "fancy virtual" is equal to the corresponding index in the table.
- The `fn` member is a function pointer of varying type, can be `nullptr`.
- If `fn` is `nullptr`, the `hash` member seems to be 0.
- The `fn` doesn't have to be a member function of the owner class (see usage of `nestedness` in Human State Nodes).
- Not all "fancy vtables" have the same length.
  - Seems like all subclasses of AvailableParkourAction have a `fancyVTable` at offset 8 containing 0x78 "fancy vfunctions".
    Examples:
        0x143A47FA0         // dive, mb?
        0x143A4E1D0         // swing turn
        0x143A13130         // descent from wall to ground; also sidehop
  - Seems like all "Human State Nodes" (subclasses of `FunctorBase`) have tables at offset 0x18 containing 0x357 "fancy vfunctions"
    Examples:
        0x14496B420         // Functor_Quickshot::FancyVTable




Some examples (uses pseudocode):

In Parkour System:
It seems to me that in that region of code the "fancy vtables" are used just like
the regular vtables would be.
- I have only seen the `fn` member used, none of them seem to ever be `nullptr`.
1)
    EnumParkourAction actionType = potentialParkourAction.fancyVTable[69]();
2)
At the end of Parkour Action Selection process for the current frame,
the array of gathered AvailableParkourActions seems to be sorted by the following predicate:
    float defaultActionWeight = currentParkourSystem.fancyVTable[40](potentialParkourAction);
    float closenessOrAppropriatenessIGuess = potentialParkourAction.fancyVTable[35]();
    return defaultActionWeight * closenessOrAppropriatenessIGuess;
Then the sorted array is iterated over, a couple more "fancy vfunctions" are called,
determining, I guess, if all conditions for the potential action are indeed met
(for example, the "Enter Window From Wall" action checks if the player is fully leaning into the window).
The first action in the sorted array that returns `true` is the one selected.

In the HumanStatesHolder:
The usage of these tables in the player's state machine is very different.
- Of the 0x357 "fancy virtuals" that each subclass of "Human State Node" (`FunctorBase` subclass)
  has, most have empty `fn` members.
- I think their usage here is an optimization:
  There are normally 20+ nodes receiving various "messages", but for any given "message" I have seen
  at most 3 receivers, normally it's 0 or 1.
  When a sort of "message" is dispatched to the nodes, instead of calling 20+
  virtual functions that likely do nothing, the game gathers an array of
  receivers by checking which ones have a non-NULL `fn` member
  at the needed "message idx" in the "fancy vtable".
  This is what the functions of the "one of those" category do.
  - Furthermore, the `nestedness` member of `fancy virtuals` is used sometimes.
    It allows the primary (most-overriding) message receivers to dispatch
    to parent nodes. When `nestedness` is N, N != 0, the `fn` will be called
    with the node's Nth parent node as the first parameter,
    so in this case the `virtual function` actually isn't a member function
    of the class that uses the FancyVTable.
*/
class FancyVFunction
{
public:
    uint32 hash; //0x0000
    uint32 staticIdx; //0x0004
    uint32 nestedness; //0x0008
    char pad_000C[4]; //0x000C
    uint64 fn; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020
assert_sizeof(FancyVFunction, 0x20);
