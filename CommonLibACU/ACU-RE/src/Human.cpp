#include "pch.h"

#include "ACU/Human.h"
#include "ACU/AtomAnimComponent.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"

static AtomAnimComponent* GetPlayerAtomAnimComponent()
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return nullptr; }
    int cpntIdx_atomAnimCpnt = player->cpntIndices_157.atomAnimCpnt;
    return static_cast<AtomAnimComponent*>(player->cpnts_mb[cpntIdx_atomAnimCpnt]);
}

Human* Human::GetForPlayer()
{
    AtomAnimComponent* atomAnimCpnt = GetPlayerAtomAnimComponent();
    if (!atomAnimCpnt) { return nullptr; }
    return atomAnimCpnt->human_c58;
}
