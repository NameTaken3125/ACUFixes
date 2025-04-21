#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

/*
Thanks to SAD_DE https://www.youtube.com/watch?v=sGp-TGJ7Fuw
and Jcers https://www.youtube.com/watch?v=dpGcKExT9us
for the discovery and explanation of the original mechanic.
*/
struct EasierTurnWhenSwinging : AutoAssemblerCodeHolder_Base
{
    EasierTurnWhenSwinging();
};
EasierTurnWhenSwinging::EasierTurnWhenSwinging()
{
    DEFINE_ADDR(whenChoosingAvailableParkourActionsReadPriorityForTheSwingTurn, 0x14014E40E);

    // Instead of reading `2.0f` from 142E77F80, I read `4.0f` from 142E77F98
    whenChoosingAvailableParkourActionsReadPriorityForTheSwingTurn = {
        "F3 0F10 35 829BD202"   // - movss xmm6,[ACU.exe+2E77F98]
    };
}
