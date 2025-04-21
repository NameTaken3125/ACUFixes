#pragma once

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct MoreSituationsToDropBomb : AutoAssemblerCodeHolder_Base
{
    MoreSituationsToDropBomb();
    void OnBeforeActivate() override;
    void OnBeforeDeactivate() override;
};
/*
Normally, if the character is sprinting, _holding_ the "Drop Bomb" button (F) does nothing.
With this enabled, Arno will immediately drop a bomb at his feet.
No longer useful, I guess, if the "More situations to Drop Bombs" patch is enabled.
*/
struct LessFinickyBombQuickDrop : AutoAssemblerCodeHolder_Base
{
    LessFinickyBombQuickDrop();
};
