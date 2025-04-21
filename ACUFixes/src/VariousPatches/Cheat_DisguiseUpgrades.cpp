#include "pch.h"

#include "Cheat_DisguiseUpgrades.h"

#include "ACU/BhvAssassin.h"
#include "ACU/ACUGetSingletons.h"

UnbreakableDisguise::UnbreakableDisguise()
{
    DEFINE_ADDR(whenCheckingIfDisguiseShouldBeBrokenByCombat, 0x141AC2480);
    whenCheckingIfDisguiseShouldBeBrokenByCombat = {
        "C7 02 00000000"        // - mov [rdx],00000000
        "C3"                    // - ret
    };
    DEFINE_ADDR(whenCheckingIfDisguiseShouldBeBrokenByParkour, 0x141AC71C0);
    whenCheckingIfDisguiseShouldBeBrokenByParkour = {
        "C7 02 00000000"        // - mov [rdx],00000000
        "C3"                    // - ret
    };
    DEFINE_ADDR(whenCheckingIfDisguiseShouldBeBrokenByBeingSeenToCloselyForTooLong, 0x141AB2190);
    whenCheckingIfDisguiseShouldBeBrokenByBeingSeenToCloselyForTooLong = {
        "48 B8 0000000000000000"    // - mov rax,0000000000000000
        "C3"                        // - ret
        , nop(2)
    };
}
DisableDisguiseCooldown::DisableDisguiseCooldown()
{
    DEFINE_ADDR(whenCheckingIfDisguiseAvailable_part1, 0x1426618EA);
    whenCheckingIfDisguiseAvailable_part1 = {
        "C6 45 E8 01"          // - mov byte ptr [rbp-18],01
        "66 0F1F 44 00 00"     // - nop 6
    };
    DEFINE_ADDR(whenCheckingIfDisguiseAvailable_part2, 0x1426619F4);
    whenCheckingIfDisguiseAvailable_part2 = {
        "66 0F1F 44 00 00"     // - nop 6
    };
}
DisguiseDoesntMakeYouInvisible::DisguiseDoesntMakeYouInvisible()
{
    uintptr_t whenDisguiseActivated_MakeActorInvisible = 0x141AD6A40;
    PresetScript_NOP(whenDisguiseActivated_MakeActorInvisible, 5);
}
void Cheat_Invisibility_DrawImGui()
{
    BhvAssassin* bhv = ACU::GetPlayerBhvAssassin();
    if (!bhv)
    {
        return;
    }
    bool isInvisible = bhv->bInvisible & 1;
    if (ImGui::Checkbox("Invisible to enemies", &isInvisible))
    {
        bhv->bInvisible = isInvisible;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Makes you just as invisible as if you were using Disguise."
        );
    }
}
