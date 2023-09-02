#include "pch.h"

#include "ACU/HumanStatesHolder.h"
#include "ACU/AtomAnimComponent.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
#include "ACU/Human.h"

static AtomAnimComponent* GetPlayerAtomAnimComponent()
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return nullptr; }
    int cpntIdx_atomAnimCpnt = player->cpntIndices_157.atomAnimCpnt;
    return static_cast<AtomAnimComponent*>(player->cpnts_mb[cpntIdx_atomAnimCpnt]);
}
HumanStatesHolder* HumanStatesHolder::GetForPlayer()
{
    AtomAnimComponent* atomAnimCpnt = GetPlayerAtomAnimComponent();
    if (!atomAnimCpnt) { return nullptr; }
    auto* human = atomAnimCpnt->human_c58; if (!human) { return nullptr; }
    return human->humanStates;
}
