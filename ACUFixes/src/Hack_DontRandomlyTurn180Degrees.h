#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

/*
When you're crouched and take exactly 1 step with the right foot first,
if you release movement keys in the little window while the right foot
is partially planted on the ground, there is a good chance
that the character will turn around what seems to be exactly 180 degrees.
*/
struct DontRandomlyTurn180Degrees : AutoAssemblerCodeHolder_Base
{
    DontRandomlyTurn180Degrees();
};
