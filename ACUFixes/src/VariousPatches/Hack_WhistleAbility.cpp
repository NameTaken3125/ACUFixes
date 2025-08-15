#include "pch.h"

#include "Hack_WhistleAbility.h"

#include "ACU/SoundManager.h"
#include "ACU/AssassinSoundSettings.h"
#include "ACU/ACUGlobalSoundSet.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/UISounds.h"
#include "ACU/WitnessEvent.h"
#include "ACU/Entity.h"
#include "ACU/BhvAssassin.h"
#include "ACU/CharacterAI.h"
#include "ACU/ReactionHandler.h"
#include "ACU/WhenHighlightedNPCChanges.h"

#include "ACU_DefineNativeFunction.h"
#include "Common_Plugins/ACU_InputUtils.h"

#include "ACU/Sound/ACU_SoundUtils.h"

UISounds::UISoundsEnum g_WhistleChosenSoundIdx = UISounds::k_SomeInterestingSound_14;

// Successfully played the "Disguise broken" sound from the player's entity, yay.
void PlayWhistleSound_Placeholder()
{
    //SoundInstance& sound = ACUGlobalSoundSet::GetSingleton()->arrSoundEvents_38[g_WhistleChosenSoundIdx];
    SoundInstance& sound = UISounds::GetSingleton()->arrSounds[g_WhistleChosenSoundIdx];

    Entity* player = ACU::GetPlayer(); if (!player) { return; }
    ACU::Sound::PlaySoundFromEntity(sound, *player);
}

bool g_IsNoticeMeRequested = false;

BindableKeyCode_Keyboard g_WhistleButton = BindableKeyCode_Keyboard::K_Y;
bool IsWhistlePressed()
{
    return ACU::Input::IsPressed(g_WhistleButton);
}
bool IsWhistleHeardNow()
{
    return IsWhistlePressed();
}

// This hook seems to stop triggering if the player is just several meters above the guards.
void WhenCheckingIfLineOfSightIsNotToNPCsOcciput_AllowNPCsSeeingFromOutTheBackOfTheHeadAndThroughWalls(AllRegisters* params)
{
    const bool isNoticeMeRequested = g_IsNoticeMeRequested || IsWhistleHeardNow();
    g_IsNoticeMeRequested = false;
    if (isNoticeMeRequested)
    {
        PlayWhistleSound_Placeholder();
        params->rdi_ |= 1;
        *(byte*)(params->r15_ + 0x30) &= (~1);
    }
}
constexpr DangerEvent::SubType witnessEventResponseHash_asIfYouUnsheathedMeleeWeapon = DangerEvent::SubType::SubType_SeeArmedAssassin;
constexpr DangerEvent::SubType witnessEventResponseHash_triggersMostGuardsIntoCombat = DangerEvent::SubType::SubType_SeeArmedAssassinClose;
constexpr DangerEvent::SubType witnessEventResponseHash_triggersEveryoneIntoCombat_mb = DangerEvent::SubType::SubType_SeePlayerInAwareState;
constexpr DangerEvent::SubType witnessEventResponseHash_turnsGuardsHeads = DangerEvent::SubType::SubType_SeePlayer;
constexpr DangerEvent::SubType witnessEventResponseHash_seenSmokeBomb = DangerEvent::SubType::SubType_SmokeBombExplosion;
constexpr DangerEvent::SubType witnessEventResponseHash_seenSomethingSpooky = DangerEvent::SubType::SubType_SmokeBombArround;
constexpr DangerEvent::SubType witnessEventResponseHash_likeWhenYouSprintAtSomeoneAndTheyTurnAround = DangerEvent::SubType::SubType_SeePlayerInWater;
constexpr DangerEvent::SubType witnessEventResponseHash_seenSomethingAmusing = DangerEvent::SubType::SubType_MoneyGrabbed;
constexpr DangerEvent::SubType witnessEventResponseHash_seenSomeoneToScoldAndWaveArmsAt = DangerEvent::SubType::SubType_SeePlayerFallingInWater;

void OnReactionHandlerRespondsToWitnessEvent_ChangeWitnessEventToAlertGuards(AllRegisters* params)
{
    if (!IsWhistleHeardNow()) { return; }
    WitnessEvent* evn = (WitnessEvent*)params->rdx_;

    /*
    This "response hash" seems to determine the response to the "Witness Event":
    - The guards even say something like "put that away"
    - Of a group of guards, only one seems to start following and scolding me
    - If in restricted area, the guard immediately jumps to conflict instead of growing the Red Meter

    The latter is a problem, because a policeman (which gets Yellow-Icon-Alerted)
    reacts to such an event differently from a guard in a restricted area (which starts conflict immediately).
    Even if makes sense in theory, it feels inconsistent and gets out of hand. When a guard is in conflict,
    he can run all over the place and alert everyone, while ideally the Whistle would merely interest him enough
    to go investigate.
    */
    evn->responseHash = witnessEventResponseHash_asIfYouUnsheathedMeleeWeapon;
}

/*
Very unfinished.
When Y is pressed, the nearby guards are forced to see
the player even out of the back of their heads and through walls. A placeholder sound is played,
and the guards' response is modified so that they act as if the player is seen
with his melee weapon out.
*/
WhistleAbility::WhistleAbility()
{
    uintptr_t whenCheckingIfLineOfSightIsNotToNPCsOcciput = 0x1412234F5;
    PresetScript_CCodeInTheMiddle(
        whenCheckingIfLineOfSightIsNotToNPCsOcciput, 8,
        WhenCheckingIfLineOfSightIsNotToNPCsOcciput_AllowNPCsSeeingFromOutTheBackOfTheHeadAndThroughWalls, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t onReactionHandlerRespondsToWitnessEvent = 0x140ED2450;
    PresetScript_CCodeInTheMiddle(
        onReactionHandlerRespondsToWitnessEvent, 5,
        OnReactionHandlerRespondsToWitnessEvent_ChangeWitnessEventToAlertGuards, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}
Entity* FindHighlightedNPC()
{
    BhvAssassin* bhvAssassin = ACU::GetPlayerBhvAssassin(); if (!bhvAssassin) { return nullptr; }
    auto* highlightedNPCContainer = bhvAssassin->toHighlightedNPC; if (!highlightedNPCContainer) { return nullptr; }
    return highlightedNPCContainer->highlightedNPC->GetPtr();
}

class ReactionHandler;
ReactionHandler* GetReactionHandler(Entity& highlightedNPC)
{
    if (highlightedNPC.cpntIndices.cpntIdx_BhvAssassin_or_AIComponent == 0xFF) { return nullptr; }
    CharacterBehavior* bhv = static_cast<CharacterBehavior*>(highlightedNPC.cpnts_mb[highlightedNPC.cpntIndices.cpntIdx_BhvAssassin_or_AIComponent]); if (!bhv) { return nullptr; }
    return bhv->characterAI_toHealthAndInventory->reactionHandler;
}

//DEFINE_GAME_FUNCTION(Statechart__TransitionToState_mb_StatechartLoop_PP, 0x14275DD60, void, __fastcall, (Statechart* a1, __int64 p_stateID_mb, __int64 a3, __int64 a4));
//DEFINE_GAME_FUNCTION(ReactionHandler__BeforeYellowIconAppears, 0x140ED6070, void, __fastcall, (ReactionHandler* a1));
//DEFINE_GAME_FUNCTION(WitnessEvent__ctor, 0x140BBE160, WitnessEvent*, __fastcall, (WitnessEvent* placeAt));
//DEFINE_GAME_FUNCTION(AlertedParams__createsCLAlertedBeforeYellowIconAppears, 0x1401AB7E0, char, __fastcall, (AlertedParams* p_alertParams, Entity* p_npc, WitnessEvent* p_witnessEvn));

#include "ACU/SharedPtr.h"
#include "ACU/Entity.h"
#include "ACU/WitnessEvent.h"
struct HasNPCEntity
{
    char pad0[0x28];
    Entity* npc;
};
assert_offsetof(HasNPCEntity, npc, 0x28);
DEFINE_GAME_FUNCTION(createsWitnessEventUnsheatheHash_d_whenUnsheatheBeforeGuard, 0x14122D420, char, __fastcall, (HasNPCEntity* a1, SharedPtrNew<Entity>** rdx0));
DEFINE_GAME_FUNCTION(SharedPtr_copy_mb, 0x140152950, SharedPtrAndSmth*, __fastcall, (SharedPtrAndSmth* p_out, SharedPtrNew<Entity>* p_shared));

// This can make the guard react as if you're having your melee weapon out,
// but not if they can't see you. Even with the "see through walls" hack continuously active
// no reaction is produced if they can't see you, no response hash of "0x8315F5D1"
// reaches the ReactionHandler at 0x140ED2450.
void TryDispatchWitnessEvent_UnsheathedMelee()
{
    BhvAssassin* bhv = ACU::GetPlayerBhvAssassin();
    if (!bhv) { return; }
    auto* toHiglightedNPC = bhv->toHighlightedNPC;
    if (!toHiglightedNPC) { return; }
    Entity* highlighted = toHiglightedNPC->highlightedNPC->GetPtr();
    if (!highlighted) { return; }

    HasNPCEntity hnpc;
    hnpc.npc = highlighted;
    createsWitnessEventUnsheatheHash_d_whenUnsheatheBeforeGuard(&hnpc, &ACU::GetPlayer()->selfSharedPtr);
}
#include "ImGuiCTX.h"
void WhistleAbilityAttempt_ImGuiControls(bool isHackActive)
{
    if (isHackActive)
    {
        ImGuiCTX::Indent _indent;
        if (ImGui::Button("Try dispatch witness event"))
        {
            TryDispatchWitnessEvent_UnsheathedMelee();
        }
        using TheEnum = decltype(g_WhistleChosenSoundIdx);
        if (ImGui::Button("-")) { g_WhistleChosenSoundIdx = (TheEnum)std::max(0, g_WhistleChosenSoundIdx - 1); }
        ImGui::SameLine();
        if (ImGui::Button("+")) { g_WhistleChosenSoundIdx = (TheEnum)std::min(TheEnum::k_NUM_ITEMS - 1, g_WhistleChosenSoundIdx + 1); }
        ImGui::SameLine();
        ImGui::SliderInt("Chosen UI sound idx", (int*)&g_WhistleChosenSoundIdx, 0, TheEnum::k_NUM_ITEMS - 1);
        ImGui::Checkbox("Request notice player using sixth sense", &g_IsNoticeMeRequested);
    }
}
