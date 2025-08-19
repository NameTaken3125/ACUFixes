#include "pch.h"

#include "Hack_MoreResponsiveBombQuickDrop.h"


#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "ACU_DefineNativeFunction.h"
#include "Common_Plugins/ACU_InputUtils.h"



#include "MainConfig.h"
bool IsBombQuickdropEnabledInCombat()
{
    return g_Config.hacks->moreSituationsToDropBombs->alsoMoreResponsiveInCombat;
}

bool g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame = false;
bool g_BombQuickthrowEnabler_isDroppingBombAnimationNow = false;
bool g_BombQuickthrowEnabler_isStartedBombDropThisFrame = false;
bool g_BombQuickthrowEnabler_IsCombatRightNow = false;
void SetIsDropping(bool isDroppingNow)
{
    g_BombQuickthrowEnabler_isDroppingBombAnimationNow = isDroppingNow;
}
bool g_BombQuickthrowEnabler_isNeedToFixRightArm = false;
static bool BombQuickthrowEnabler_isNeedToFixRightArm()
{
    return g_BombQuickthrowEnabler_isNeedToFixRightArm;
}
void WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting_inner(AllRegisters* params)
{
    bool doBombDrop = params->r15_ & 0xFF;
    if (doBombDrop)
        return;
    const bool forceBombDrop
        = ACU::Input::IsPressed(ActionKeyCode::BombDrop)
        && ACU::Input::IsPressed(ActionKeyCode::Sprint);
    params->r15_ = forceBombDrop;
}
void WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting(AllRegisters* params)
{
    WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting_inner(params);
    const bool doBombDrop = params->r15_ & 0xFF;
    if (!doBombDrop)
        return;
}
LessFinickyBombQuickDrop::LessFinickyBombQuickDrop()
{
    uintptr_t whenCheckingIfBombShouldBeQuickDropped = 0x142666F8F;
    PresetScript_CCodeInTheMiddle(whenCheckingIfBombShouldBeQuickDropped, 7,
        WhenCheckingIfBombShouldBeQuickDropped_ForceIfSprinting, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}








DEFINE_GAME_FUNCTION(SmallArray_DWORD__AppendIfNotPresent_mb, 0x140B8E8F0, void, __fastcall, (SmallArray<uint32>* arr, uint32* newElement));
void MoveElementToStart(SmallArray<uint32>& arr, uint32 elem)
{
    int initialPos = (int)(std::find(arr.begin(), arr.end(), elem) - arr.begin());
    for (int i = initialPos; i > 0; i--)
    {
        arr[i] = arr[i - 1];
    }
    arr[0] = elem;
}
void AddBombDropStateToListOfToBeUpdated(SmallArray<uint32>& arrayToFill)
{
    uint32 stateEnumThatChecksForBombDrop = 1;
    if (std::find(arrayToFill.begin(), arrayToFill.end(), stateEnumThatChecksForBombDrop) != arrayToFill.end())
    {
        return;
    }
    constexpr uint32 stateEnumThatIsUsedWhenLootingBodies = 14;
    if (std::find(arrayToFill.begin(), arrayToFill.end(), stateEnumThatIsUsedWhenLootingBodies) != arrayToFill.end())
    {
        return;
    }
    SmallArray_DWORD__AppendIfNotPresent_mb(&arrayToFill, &stateEnumThatChecksForBombDrop);
    {
        // UPD: I'm now having doubts regarding what's written below about preserving the original array's order.
        //      State `1` _does_ have to be placed at the start though.
        //
        // This here is extremely important.
        // Without this, when in combat, Arno dropping a bomb during a roll or an attack
        // uses not just the left arm to drop the bomb, but also the right arm and really the whole upper body
        // stays mostly upright. This is especially weird (spine breaking weird) during a roll.
        // I used to just move `stateEnumThatChecksForBombDrop` to the start by swapping with the first element,
        // but that wasn't enough.
        // In combat, the state enums to be updated are normally [0, 2].
        // To allow bomb drops (and fix animations), the resulting array must be
        // [1, 0, 2]. Again, order matters, because for example if array is left as [1, 2, 0], then only [1, 2]
        // will be updated (states updater will short-return after `2` because it returns `true`),
        // and to unbreak the spine I _need_ to run state `0` with `isInHighProfile==true` _in_the_same_frame_.
        MoveElementToStart(arrayToFill, stateEnumThatChecksForBombDrop);
    }
    g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame = true;
}
void WhenFillingArrayOfStateEnumsToUpdate_EnableChecksForBombQuickDrop(AllRegisters* params)
{
    auto& arrayToFill = *(SmallArray<uint32>*)(params->GetRSP() + 0x30);
    if (arrayToFill.size == 2
        && arrayToFill[0] == 0
        && arrayToFill[1] == 2
        )
    {
        // The combat situation. There are some confusing and hard to catch bugs if enabled in combat.
        // _Sometimes_, under undetermined conditions, perhaps after rolling around a lot,
        // dropping bombs and quickshooting a lot, Arno gets stuck in a state where he can't change ranged weapon,
        // and shooting itself feels, um, weird.
        // Exiting and reentering combat (weapon needs to be completely sheathed, I guess)
        // restores normalcy, but this is not really acceptable.
        // Perhaps if I'm able to also implement an always-available quickshot,
        // then "combat version" of quickshot can be removed just as the "combat version"
        // of quickthrow was.

        g_BombQuickthrowEnabler_IsCombatRightNow = true;
        if (!IsBombQuickdropEnabledInCombat())
        {
            return;
        }
    }
    if (arrayToFill.size == 3
        && arrayToFill[0] == 0
        && arrayToFill[1] == 6
        && arrayToFill[2] == 3
        )
    {
        // Sitting behind cover. Not needed, perhaps (and enabling it has a side effect of
        // `Loot` interaction pop up near dead bodies).
        return;
    }
    AddBombDropStateToListOfToBeUpdated(arrayToFill);
}


class HasLanterndlcComponent;
class HumanStatesHolder;
DEFINE_GAME_FUNCTION(UpdateState_ReloadRangedWeapon, 0x142663A40, __int64, __fastcall, (HasLanterndlcComponent* a1, HumanStatesHolder* a2, unsigned __int8 a3));
void WhenUpdateGunReloadChecker_DisableIfQuickdropWasForceEnabled(AllRegisters* params)
{
    if (g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame)
    {
        params->GetRAX() = 0;
        return;
    }
    HasLanterndlcComponent* a1 = (HasLanterndlcComponent*)params->rcx_;
    HumanStatesHolder* a2 = (HumanStatesHolder*)params->rdx_;
    params->GetRAX() = UpdateState_ReloadRangedWeapon(a1, a2, 0);
}
void HasLanternCpnt30SetHighProfile(uint64 hasLantern, bool makeInHighProfile)
{
    *(byte*)(*(uint64*)(hasLantern + 0x30) + 0x20) = makeInHighProfile;
}
void WhenDropBombCheckerFinishes_PretendArnoIsInHighProfileIfItsNeededToFixAnimations(AllRegisters* params)
{
    bool resultOfBombDropChecker_parentCall = (params->GetRAX()) & 0xFF;
    if (!resultOfBombDropChecker_parentCall)
        return;
    if (g_BombQuickthrowEnabler_isStartedBombDropThisFrame && BombQuickthrowEnabler_isNeedToFixRightArm())
    {
        auto* someImportantObject = (HasLanterndlcComponent*)params->rbx_;

        HasLanternCpnt30SetHighProfile(params->rbx_, true);
        params->rsi_ = 1;
        params->GetRAX() = 0;
    }
}
void WhenHighProfileMovementIsDecided_insideGetter_PretendArnoIsInHighProfileIfItsNeededToFixAnimations(AllRegisters* params)
{
    if (BombQuickthrowEnabler_isNeedToFixRightArm())
    {
        *(byte*)(params->rcx_ + 0x20) = 1;
    }
}
void WhenDropBombAnimationEnds_ClearBombDropFlags(AllRegisters* params)
{
    SetIsDropping(false);
    g_BombQuickthrowEnabler_isNeedToFixRightArm = false;
}
void WhenStatesUpdaterFinishes_ClearFlagsForFrame(AllRegisters* params)
{
    g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame = false;
    g_BombQuickthrowEnabler_isStartedBombDropThisFrame = false;
    g_BombQuickthrowEnabler_IsCombatRightNow = false;
}
void WhenCheckingIfShouldDisallowDropBombOnSlopes_DisobeyOrders(AllRegisters* params)
{
    const bool isOnSlope_mb = *(bool*)(params->GetRSP() + 0x20);
    if (isOnSlope_mb)
        g_BombQuickthrowEnabler_isNeedToFixRightArm = true;
}
void WhenCheckingIfShouldDisallowDropBombInWallrunAndTrees_DisobeyOrders(AllRegisters* params)
{
    const bool isBombDropAllowed_falseifWallrunOrStandingInTree = *(bool*)(params->GetRSP() + 0x38);
    const bool needToFixRightArm = !isBombDropAllowed_falseifWallrunOrStandingInTree;
    if (needToFixRightArm)
        g_BombQuickthrowEnabler_isNeedToFixRightArm = true;
    params->rcx_ = params->rbx_;
}
void WhenSuccessfullyStartingToDropBomb_RememberSuccess(AllRegisters* params)
{
    SetIsDropping(true);
    g_BombQuickthrowEnabler_isStartedBombDropThisFrame = true;
    if (g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame)
    {
        g_BombQuickthrowEnabler_isNeedToFixRightArm = true;
    }
}
DEFINE_GAME_FUNCTION(sub_142666AE0, 0x142666AE0, char, __fastcall, (HasLanterndlcComponent* a1));
void WhenCombatActionsAreUpdatedChecksBombDrop_DisableOriginalBombDropHandlingInCombat(AllRegisters* params)
{
    if (IsBombQuickdropEnabledInCombat())
    {
        params->GetRAX() = 0;
        return;
    }
    params->GetRAX() = sub_142666AE0((HasLanterndlcComponent*)params->rcx_);
}
void WhenOnWallCheckingIfAssassinateAttemptTargetAvailable_DisableIfBombJustDropped(AllRegisters* params)
{
    if (g_BombQuickthrowEnabler_isStartedBombDropThisFrame)
    {
        params->rbx_ = 1;
    }
}




#include "ACU/ReactionManager.h"
#include "ACU/ReactionRadiusData.h"


#include "MovabilityUtils.h"
template<typename T>
class AutoRestoredValue_tmplt
{
public:
    std::reference_wrapper<T> valueRef;
    T initialValue;
public:
    NON_MOVABLE(AutoRestoredValue_tmplt);
    AutoRestoredValue_tmplt(T& valueRef) : valueRef(valueRef), initialValue(valueRef) {}
    void Restore() { valueRef.get() = initialValue; }
    ~AutoRestoredValue_tmplt()
    {
        Restore();
    }
};

class ReactionRadiusData_Accessor
{
public:
    ReactionRadiusData_Accessor(ReactionRadiusData& radiusData)
        : m_AutoRestoredValues(radiusData.flts)
    {}
private:
    AutoRestoredValue_tmplt<ReactionRadiusData_floats> m_AutoRestoredValues;
};

class ReactionRadiusData_Database
{
public:
    std::optional<ReactionRadiusData_Accessor> radiusData_hitSomeoneWithWristbow;
    std::optional<ReactionRadiusData_Accessor> radiusData_bombQuickdropped1;
    std::optional<ReactionRadiusData_Accessor> radiusData_bombQuickdropped2;
    void RestoreDefaults()
    {
        radiusData_hitSomeoneWithWristbow.reset();
        radiusData_bombQuickdropped1.reset();
        radiusData_bombQuickdropped2.reset();
    }


    NON_MOVABLE(ReactionRadiusData_Database);
    ReactionRadiusData_Database() = default;
    static ReactionRadiusData_Database& GetSingleton() { static ReactionRadiusData_Database inst; return inst; }
};

constexpr DangerEvent::SubType witnessEventResponseHash_StartAimWristbow = DangerEvent::SubType::SubType_SeePhantomBladeAimAt;
constexpr DangerEvent::SubType witnessEventResponseHash_FirePhantomBladeSpecifically = DangerEvent::SubType::SubType_IconicBladeImpact;
constexpr DangerEvent::SubType witnessEventResponseHash_StartAimGun = DangerEvent::SubType::SubType_SeeGunAimAt;
constexpr DangerEvent::SubType witnessEventResponseHash_PlayerCrouching = DangerEvent::SubType::SubType_SeePlayerSneaking;
constexpr DangerEvent::SubType witnessEventResponseHash_WeaponDrawnMeleeOrGun = DangerEvent::SubType::SubType_SeeArmedAssassin;
constexpr DangerEvent::SubType witnessEventResponseHash_PlayerHitSomeoneWithWristbow_mb = DangerEvent::SubType::SubType_SeePhantomBladeFatal;
constexpr DangerEvent::SubType witnessEventResponseHash_BombQuickdropped1 = DangerEvent::SubType::SubType_ThrowingProjectile;
constexpr DangerEvent::SubType witnessEventResponseHash_BombQuickdropped2 = DangerEvent::SubType::SubType_ThrowingSmokeBomb;
void FixReactionRadiusDatas()
{
    auto& reactionRadiusMap = ReactionManager::GetSingleton()->hashmapReactionRadiusData;
    auto& radiusDatas = ReactionRadiusData_Database::GetSingleton();
    if (!radiusDatas.radiusData_bombQuickdropped1
        || !radiusDatas.radiusData_bombQuickdropped2)
    {
        // When hanging on a wall above a guard, it often makes sense
        // to drop a Smoke Bomb then drop assassinate.
        // Dropping a bomb sends `WitnessEvents` of the player entity
        // with hashes `ReactionHash_ReactToWhat::BombQuickdropped1/BombQuickdropped2`
        // For some reason, guards about 3 meters below and turned away are still able to "witness"
        // this action (that is, as soon as you barely release the bomb, before it is anywhere near ground),
        // resulting in a detection, which in turn forces the assassination to fail.
        // Here's a very simplistic fix: I just reduce the allowed radius for this reaction.
        // Side effects of this change are TBD.
        ReactionRadiusData** radiusData_bombQuickdropped1 = reactionRadiusMap.Get(witnessEventResponseHash_BombQuickdropped1);
        ReactionRadiusData** radiusData_bombQuickdropped2 = reactionRadiusMap.Get(witnessEventResponseHash_BombQuickdropped2);
        if (
            radiusData_bombQuickdropped1
            && *radiusData_bombQuickdropped1
            && radiusData_bombQuickdropped2
            && *radiusData_bombQuickdropped2
            )
        {
            radiusDatas.radiusData_bombQuickdropped1.emplace(**radiusData_bombQuickdropped1);
            //(**radiusData_bombQuickdropped1).flts = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
            radiusDatas.radiusData_bombQuickdropped2.emplace(**radiusData_bombQuickdropped2);
            //(**radiusData_bombQuickdropped2).flts = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
            auto& radData = (**radiusData_bombQuickdropped2);
            // Warning: I used to change both of the "throw bomb"-related reaction datas,
            // setting all 6 floats in each to 0.
            // A strange side effect of this was a bug in one of the Paris Crowd Events,
            // namely the one where two blue guards witness a murder of one civilian by another one
            // (the latter usually wearing a purple vest and a top hat). It seems that this events uses one of the
            // "throw bomb"-related hashes in the Events it emits, and when I reduce all the "radius values" to 1.0f,
            // the blue guards just fail to notice the criminal standing just a couple of meters right in front of them,
            // so he runs away, and the guards start investigating the dead body.
            // With ACViewer and AnvilToolkit having recovered a bunch of class member names
            // I'm able to make a smaller change, editing just the "ZCutoffUp" and "ZCutoffDown" values.
            // This seems to be enough to have the desired effect on the "bomb thrown event"
            // and doesn't have this strange crowd bug.
            radData.flts.ZCutoffDown = radData.flts.ZCutoffUp = 1.0f;
        }
    }
    if (!radiusDatas.radiusData_hitSomeoneWithWristbow)
    {
        // While I'm at it, I change the detection radius for "Player shot somebody with Wristbow".
        // Here's the thing: making a shot from the Wristbow is completely silent. To prove this,
        // stand a meter or so behind a guard with noone around. Fire from behind, missing around his ears.
        // The guard might notice the projectile hitting something in front of him,
        // even see a body drop, but he won't turn around to notice you.
        // However, check this out: "It belongs in the Museum", [46.03 -282.90 0.50]:
        // you can stand some 5 meters, I guess, behind a gunner in the hall, 15 meters in front of a brute
        // (second gunner in the hall, the one facing you, needs to be removed - use the invisibility cheat),
        // in a restricted area, without being noticed.
        // You can point the wristbow at brute's face, be in range of wristbow shot, you can spin around,
        // but you won't get a yellow icon or even a turned head from him.
        // You can even shoot next to the brute, at the ground in front of him, at the column behind him,
        // and at most he will act interested by the impact sound, but won't notice you.
        // Yet as soon as you shoot at the gunner that stands behind a column between you and the brute,
        // you're instantly detected (provided the brute has a clear Line Of Sight to you).
        // So which is it? The sound, the movement? It's established he doesn't hear the sound of the shot,
        // doesn't see the glint of projectile, nor can see me standing with my arm outstretched,
        // nor would rise to high alert if he _didn't_ have a clear Line Of Sight to me.
        // As far as I can tell, it just doesn't make sense and more importantly, is unpredictable
        // and unfairly inconsistent.
        // When standing in the "safe spot" that I described above,
        // unsheathing the melee weapon also results in immediate detection,
        // but you could argue that the weapon glints and unsheathing itself is a more noticeable motion.
        //
        // The actual reason detection happens is because the "ReactionHashes" for aiming with wristbow
        // or shooting with wristbow have a much smaller "reaction radius" than the action of
        // specifically _hitting_ someone while being "sort of in sight".
        // So I make the reaction radius for "player just shot someone with wristbow"
        // the same as "player is aiming with wristbow".
        // You can see me shooting the wristbow <=> you must see me aiming.
        // Note that this doesn't change the following:
        // - Aiming the gun is still more noticeable than aiming the wristbow;
        // - It is still easier for guards to see you if they are looking for you
        //   (e.g. in the described situation shooting the BerserkBlade at gunner would normally trigger detection
        //   from the brute immediately, but now the brute might merely notice something is wrong with the gunner,
        //   and _then_ start looking more closely at you)
        // - Detection can still immediately trigger from the guard if _he_ specifically is shot.
        ReactionRadiusData** radiusData_aimingWristbow = reactionRadiusMap.Get(witnessEventResponseHash_StartAimWristbow);
        ReactionRadiusData** radiusData_hitSomeoneWithWristbow = reactionRadiusMap.Get(witnessEventResponseHash_PlayerHitSomeoneWithWristbow_mb);
        if (
            radiusData_aimingWristbow
            && *radiusData_aimingWristbow
            && radiusData_hitSomeoneWithWristbow
            && *radiusData_hitSomeoneWithWristbow
            )
        {
            radiusDatas.radiusData_hitSomeoneWithWristbow.emplace(**radiusData_hitSomeoneWithWristbow);
            (**radiusData_hitSomeoneWithWristbow).flts = (**radiusData_aimingWristbow).flts;
        }
    }
}
DEFINE_GAME_FUNCTION(onQuickshot_PX, 0x14265DDD0, __int64, __fastcall, (HasLanterndlcComponent* a1, HumanStatesHolder* rdx0, char a3));
void WhenUpdatingOutOfCombatVersionOfQuickshot_DisableWhenInCombat(AllRegisters* params)
{
    if (IsBombQuickdropEnabledInCombat())
    {
        if (g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame
            && g_BombQuickthrowEnabler_IsCombatRightNow)
        {
            params->GetRAX() = 0;
            return;
        }
    }
    HasLanterndlcComponent* a1 = (HasLanterndlcComponent*)params->rcx_;
    HumanStatesHolder* a2 = (HumanStatesHolder*)params->rdx_;
    params->GetRAX() = onQuickshot_PX(a1, a2, 1);
}
void WhenCheckingIfLeaveCombatBySprint_MakeSureIsReallyTryingToSprint(AllRegisters* params)
{
    if (!IsBombQuickdropEnabledInCombat())
    {
        return;
    }
    const bool isReallySprinting = ACU::Input::IsPressed(ActionKeyCode::Sprint);
    if (!isReallySprinting)
    {
        byte* pIsShouldLeaveCombat = (byte*)(params->rbp_ + 0x420);
        *pIsShouldLeaveCombat = 0;
    }
}

DEFINE_GAME_FUNCTION(DisableThisToMagicallyUnbreakAssassinationsAndDropBombs, 0x14265DDA0, uint32, __fastcall, (HasLanterndlcComponent* a1, HumanStatesHolder* a2, unsigned __int8 a3));
void WhenMakingSomeStrangeCallInNoncombatUpdates(AllRegisters* params)
{
    if (g_BombQuickthrowEnabler_bombDropCheckerHasBeenForceEnabledThisFrame)
    {
        params->GetRAX() = 0;
        return;
    }
    params->GetRAX() = DisableThisToMagicallyUnbreakAssassinationsAndDropBombs(
        (HasLanterndlcComponent*)params->rcx_,
        (HumanStatesHolder*)params->rdx_,
        params->r8_ & 0xFF
    );
}
#include "ACU/HumanStatesHolder.h"
bool IsInHaystack(HumanStatesHolder& humanStates)
{
    const uint64 haystackState_Enter = 0x1419DECC0;
    for (auto& primaryReceiver : humanStates.primaryCallbackReceivers)
    {
        const uint64 currNode_Enter = (uint64)primaryReceiver.pNode->Enter;
        if (currNode_Enter == haystackState_Enter)
        {
            return true;
        }
    }
    return false;
}
#include "ACU/Enum_EquipmentType.h"
DEFINE_GAME_FUNCTION(oneOfThoseFns_whenTryToStartAimBomb, 0x14198F020, int, __fastcall, (HumanStatesHolder* p_humanStates, __int64 a2, EquipmentType p_bombEquipType, __int64 a4));
void WhenLongPressedToAimBomb_DontTryIfInHaystack(AllRegisters* params)
{
    auto* humanStates = (HumanStatesHolder*)params->rcx_;
    int callResult;

    const bool isAllowedToTryAimBomb = !IsInHaystack(*humanStates);
    if (isAllowedToTryAimBomb)
    {
        callResult = oneOfThoseFns_whenTryToStartAimBomb(
            humanStates,
            params->rdx_,
            (EquipmentType&)params->r8_,
            params->r9_);
    }
    else
    {
        callResult = 1;
    }
    params->GetRAX() = callResult;
}
MoreSituationsToDropBomb::MoreSituationsToDropBomb()
{
    auto WhenBuildingArrayOfStatesToBeUpdated_AddQuickdropChecker = [&]() {
        uintptr_t whenFillingArrayOfStateEnumsToUpdate = 0x142656F47;
        PresetScript_CCodeInTheMiddle(whenFillingArrayOfStateEnumsToUpdate, 5,
            WhenFillingArrayOfStateEnumsToUpdate_EnableChecksForBombQuickDrop, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };
    auto DisableGunReloadIfQuickdropWasForceEnabled = [&]() {
        uintptr_t whenUpdateGunReloadChecker = 0x14265D18E;
        uintptr_t whenUpdateGunReloadChecker_return = 0x14265D18E;
        PresetScript_CCodeInTheMiddle(whenUpdateGunReloadChecker, 5,
            WhenUpdateGunReloadChecker_DisableIfQuickdropWasForceEnabled, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };

    auto StrangeFixToUnbreakCombatAndAssassinations_v2 = [&]()
    {
        // When I enable the "Bomb drop updater", I also enable plenty of other things
        // like the calls that check whether quickshot needs to be started, the looting state, interaction,
        // but also this one strange call.
        // If this call is enabled, the "extended bomb dropping ability" will work fine
        // until you try to assassinate or start combat, in which case the player's state
        // will glitch out, repeatedly and rapidly "reset".
        // Interestingly, skipping this strange call entirely changes pretty much nothing in the game,
        // it seems, until you reload the level and see Arno standing in an A-pose. What is it doing?
        // Why is it called every frame? I dunno.
        // This strange fix consists of _not_ making that strange call if the "bomb drop updater"
        // was force enabled for the current frame.
        uintptr_t whenMakingSomeStrangeCallInTheNoncombatUpdates = 0x14265D137;
        PresetScript_CCodeInTheMiddle(whenMakingSomeStrangeCallInTheNoncombatUpdates, 5,
            WhenMakingSomeStrangeCallInNoncombatUpdates, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };

    WhenBuildingArrayOfStatesToBeUpdated_AddQuickdropChecker();
    StrangeFixToUnbreakCombatAndAssassinations_v2();
    DisableGunReloadIfQuickdropWasForceEnabled();

    // Also allow during a wallrun and when standing in the V-shape of a tree or flagpole.
    uintptr_t whenCheckingIfShouldDisallowDropBombInWallrunAndTrees = 0x141AA7C51;
    PresetScript_CCodeInTheMiddle(whenCheckingIfShouldDisallowDropBombInWallrunAndTrees, 5,
        WhenCheckingIfShouldDisallowDropBombInWallrunAndTrees_DisobeyOrders, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

    // Also allow during movement on steep slopes (like Notre Dame's).
    // This is actually independent from the above hacks.
    uintptr_t whenCheckingIfShouldDisallowDropBombOnSlopes = 0x142667042;
    PresetScript_CCodeInTheMiddle(whenCheckingIfShouldDisallowDropBombOnSlopes, 6,
        WhenCheckingIfShouldDisallowDropBombOnSlopes_DisobeyOrders, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);

    // Also allow during social stealth interaction like sitting on a bench, leaning on a wall.
    uintptr_t whenCheckingIfDropbombIsAvailableInAbilitySets = 0x142664422;
    PresetScript_NOP(whenCheckingIfDropbombIsAvailableInAbilitySets, 6);


    uintptr_t whenSuccessfullyStartingToDropBomb = 0x141A2C040;
    PresetScript_CCodeInTheMiddle(whenSuccessfullyStartingToDropBomb, 6,
        WhenSuccessfullyStartingToDropBomb_RememberSuccess, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenDropBombAnimationEnds = 0x141AA7DF0;
    PresetScript_CCodeInTheMiddle(whenDropBombAnimationEnds, 5,
        WhenDropBombAnimationEnds_ClearBombDropFlags, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    uintptr_t whenDropBombCheckerFinishes = 0x14265764C;
    uintptr_t whenDropBombCheckerFinishes_returnTo = 0x1426577BD;
    PresetScript_CCodeInTheMiddle(whenDropBombCheckerFinishes, 5,
        WhenDropBombCheckerFinishes_PretendArnoIsInHighProfileIfItsNeededToFixAnimations, whenDropBombCheckerFinishes_returnTo, false);

    uintptr_t whenHighProfileMovementIsDecided_insideGetter = 0x1409D9700;
    PresetScript_CCodeInTheMiddle(whenHighProfileMovementIsDecided_insideGetter, 5,
        WhenHighProfileMovementIsDecided_insideGetter_PretendArnoIsInHighProfileIfItsNeededToFixAnimations, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    auto AlsoAllowBombDropWhenHangingOnWallAndTheresAssassinationTargetAvailableBelow = [&]()
    {
        uintptr_t whenOnWallCheckingIfAssassinateAttemptTargetAvailable = 0x142651B03;
        PresetScript_CCodeInTheMiddle(whenOnWallCheckingIfAssassinateAttemptTargetAvailable, 7,
            WhenOnWallCheckingIfAssassinateAttemptTargetAvailable_DisableIfBombJustDropped, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };
    AlsoAllowBombDropWhenHangingOnWallAndTheresAssassinationTargetAvailableBelow();

    uintptr_t whenStatesUpdaterFinishes = 0x1426572B1;
    PresetScript_CCodeInTheMiddle(whenStatesUpdaterFinishes, 7,
        WhenStatesUpdaterFinishes_ClearFlagsForFrame, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);

    auto PreventDoubleBombDropInCombat = [&]()
    {
        uintptr_t whenCombatActionsAreUpdatedChecksBombDrop = 0x14265C72B;
        PresetScript_CCodeInTheMiddle(whenCombatActionsAreUpdatedChecksBombDrop, 5,
            WhenCombatActionsAreUpdatedChecksBombDrop_DisableOriginalBombDropHandlingInCombat, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };
    auto DisableOutOfCombatVersionOfQuickshotWhenInCombat = [&]()
    {
        const uintptr_t whenUpdatingOutOfCombatVersionOfQuickshot = 0x14265D11F;
        PresetScript_CCodeInTheMiddle(whenUpdatingOutOfCombatVersionOfQuickshot, 5,
            WhenUpdatingOutOfCombatVersionOfQuickshot_DisableWhenInCombat, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };
    auto PreventAccidentalSprintWhenDropBombInCombat = [&]()
    {
        /*
        Because I put the player into High Profile while dropping bombs,
        doing so in combat might result in undesired breaking of combat by sprint.
        */
        const uintptr_t whenCheckingIfLeaveCombatBySprint = 0x1426593BF;
        PresetScript_CCodeInTheMiddle(whenCheckingIfLeaveCombatBySprint, 7,
            WhenCheckingIfLeaveCombatBySprint_MakeSureIsReallyTryingToSprint, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    };

    PreventDoubleBombDropInCombat();
    DisableOutOfCombatVersionOfQuickshotWhenInCombat();
    PreventAccidentalSprintWhenDropBombInCombat();



    auto DisableBombAimInHaystack = [&]()
    {
        uintptr_t whenLongPressedToAimBomb = 0x1426647AF;
        PresetScript_CCodeInTheMiddle(whenLongPressedToAimBomb, 5,
            WhenLongPressedToAimBomb_DontTryIfInHaystack, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
    };
    DisableBombAimInHaystack();
}
void MoreSituationsToDropBomb::OnBeforeActivate()
{
    FixReactionRadiusDatas();
}
void MoreSituationsToDropBomb::OnBeforeDeactivate()
{
    ReactionRadiusData_Database::GetSingleton().RestoreDefaults();
}
