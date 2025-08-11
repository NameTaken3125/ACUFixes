#pragma once

#include "ManagedObject.h"

class AbilitySet : public ManagedObject
{
public:
}; //Size: 0x0010
assert_sizeof(AbilitySet, 0x10);

class AssassinAbilitySet : public AbilitySet
{
public:
    enum MaxSpeed : uint32
    {
        Jog = 6,
    };

    struct Flags
    {
        MaxSpeed MaxSpeed_; //0x0010
        char pad_14[4]; //0x0014

        //===============================================
        // 0x0018
        //===============================================

        bool LeapOfFaith : 1;
        bool Push : 1;
        bool Swim : 1;
        bool Dive : 1;
        bool FirstPersonView : 1;
        bool EagleVision : 1;
        bool PickPocket : 1;
        bool DropCarriedObject : 1;

        // 0x0019:

        bool ThrowCarriedObject : 1;
        bool Loot : 1;
        bool Skin : 1;
        bool CarryBhv : 1;
        bool SpringJump : 1;
        bool UseHideSpots : 1;
        bool CanAttractAttentionInHideSpots : 1;
        bool UseBlendTraps : 1;

        // 0x001A:

        bool BlendInCrowd : 1;
        bool ThrowMoney : 1;
        bool FollowMe : 1;
        bool LockAndUse : 1;
        bool CanHire : 1;
        bool UseProstitute : 1;
        bool UseThieves : 1;
        bool UseMercenaries : 1;

        // 0x001B:

        bool Charge : 1;
        bool GroundTackle : 1;
        bool CanUseShops : 1;
        bool CanTargetGenericFocusableEntities : 1;
        bool CanInteractWithObjects : 1;
        bool CanCounterAnimalAttack : 1;
        bool CanAutoCounterAnimalAttack : 1;
        bool CanUseDisguiseSkill : 1;

        // 0x001C:

        bool CanUseGroupHealingSkill : 1;
        bool CanUseRefillPointSkill : 1;
        bool CanUseAvatarBooster : 1;
        bool CanUseEject : 1;
        bool bits_1C_4_8 : 4;

        char pad_1D[3]; //0x001D

        //===============================================
        // 0x0020
        //===============================================

        bool Movement : 1; // allowWASD
        bool Camera : 1;
        bool Jump : 1;
        bool UntargetedJump : 1;
        bool Crouch : 1;
        bool PassOver : 1;
        bool Walling : 1;
        bool Facade : 1;

        // 0x0021:

        bool Climb : 1;
        bool ClimbLeap : 1;
        bool ClimbRelease : 1;
        bool ClimbEjectToVoid : 1;
        bool Ladder : 1;
        bool LookDown : 1;
        bool BalanceAlways : 1;
        bool Rotate : 1;

        // 0x0022:

        bool Parachute : 1;
        bool Stealth : 1;
        bool CanUseRangedWeapons : 1;
        bool DisturbedMove : 1;
        bool SlopeSlideWhenWalking : 1;
        bool Revive : 1;
        bool Cover : 1;
        bool ImmuneToWindExposureSlowdown : 1;

        // 0x0023:

        bool UseIkInteriorGroundLimit : 1;
        bool Beam : 1;
        bool FastFreeRun : 1;
        bool FastWalling : 1;
        bool FastPilotis : 1;
        bool FastClimb : 1;
        bool FastPoles : 1;
        bool FastLadders : 1;

        // 0x0024:

        bool FastBeams : 1;
        bool FastFreeStep : 1;
        bool DeepCornicheEjection : 1;
        bool ClimbOnHorse : 1;
        bool SummonHorse : 1;
        bool Dismount : 1;
        bool GrandGallop : 1;
        bool JumpFromHorse : 1;

        // 0x0025:

        bool Rearing : 1;
        bool HijackGroundToHorse : 1;
        bool HijackHorseToHorse : 1;
        bool HijackAirToHorse : 1;
        bool AssassinateGroundToHorse : 1;
        bool AssassinateHorseToHorse : 1;
        bool AssassinateAirToHorse : 1;
        bool FightOnHorse : 1;

        // 0x0026:

        bool AutoFollow : 1;
        bool PrecisionMode : 1;
        bool ShellshockedLevel1 : 1;
        bool ShellshockedLevel2 : 1;
        bool CanRegenerateHealth : 1;
        bool bits_20_35_38 : 0x38 - 0x35;
        bool bits_20_38_40 : 0x40 - 0x38;


        //===============================================
        // 0x0028
        //===============================================

        bool FistAttack : 1;
        bool FistBlock : 1;
        bool FistCombo : 1;
        bool Grab : 1;
        bool CanPerformHiddenBladeSpecialAttack : 1;
        bool CanPerformNormalSpecialAttack : 1;
        bool CanPerformShortSpecialAttack : 1;
        bool Grasp : 1;

        // 0x0029:

        bool CanSelectWeapon : 1;
        bool WeaponAccess : 1;
        bool Fight : 1;
        bool CanDisarm : 1;
        bool CanWeaponAttack : 1;
        bool CanWeaponBlock : 1;
        bool CanHiddenBladeBlockAndDefect : 1;
        bool CanWeaponCombo : 1;

        // 0x002A:

        bool Targeting : 1;
        bool CounterDodge : 1;
        bool CounterKill : 1;
        bool CanCounterThrow : 1;
        bool CanCounterHurt : 1;
        bool CanCounterTool : 1;
        bool CanDodgeRangeAttacks : 1;
        bool CanCounterCounter : 1;

        // 0x002B:

        bool GrabBreaker : 1;
        bool FightSprintEscape : 1;
        bool CombatSteps : 1;
        bool DefenseBreaker : 1;
        bool CanThrowKnives : 1;
        bool CanThrowSmokeBombs : 1;
        bool CanTaunt : 1;
        bool CanUseShortWeapon : 1;

        // 0x002C:

        bool CanUseNormalWeapon : 1;
        bool CanUseHeavyWeapon : 1;
        bool CanUseLongWeapon : 1;
        bool CanUseArmors : 1;
        bool CanUseHiddenBlade : 1;
        bool CanPerformHiddenBladeCombos : 1;
        bool CanLinkComboKills : 1;
        bool CanPerformComboKill : 1;

        // 0x002D:

        bool CanPerformOpenDefenseAttack : 1;
        bool CanPerformCounterGrab : 1;
        bool FistSpecialAttack : 1;
        bool CanPerformThrowingKnifeSpecialAttack : 1;
        bool CanPerformThrowLongSpecialAttack : 1;
        bool CanPerformThrowBluntSpecialAttack : 1;
        bool CanPerformHeavySlamSpecialAttack : 1;
        bool EnableSwordGunDualWield : 1;

        // 0x002E:

        bool EnableDaggerKnivesDualWield : 1;
        bool CanDropWeapon : 1;
        bool CanPickUpWeapon : 1;
        bool CanUseCrossbow : 1;
        bool CanUseAppleOfEden : 1;
        bool CanUseRopeDart : 1;
        bool CanTripMine : 1;
        bool CanPerformDoubleComboKill : 1;

        // 0x002F:

        bool CanUseBow : 1;
        bool CanUseHorseWhistle : 1;
        bool CanUsePoisonDarts : 1;
        bool CanDoContextualAttacks : 1;
        bool bits_28_3C_40 : 0x40 - 0x3C;

        //===============================================
        // 0x0030
        //===============================================

        bool CanAssassinate : 1;
        bool BlendAssassination : 1;
        bool ObviousAssassination : 1;
        bool StealthAssassination : 1;
        bool AssassinationWithAnyWeapon : 1;
        bool AirAssassination : 1;
        bool AssassinationFromHidingSpot : 1;
        bool AssassinationFromLedge : 1;

        // 0x0031:

        bool CanDoubleAssassinate : 1;
        bool CanCoverAssassinate : 1;
        bool CanPoisonAssassinate : 1;
        bool CanGunAssassinate : 1;
        bool CanPoisonDarts : 1;
        bool CanPoisonUpgrade : 1;
        bool CanSheathHeavyWeapons : 1;
        bool CanSheathMediumWeapons : 1;

        // 0x0032:

        bool CanUseEagleSenseLvl2 : 1;
        bool bits_30_11_18 : 7;

        char pad_33[1]; //0x0033

        //===============================================
        // 0x0034
        //===============================================

        bool HighProfile : 1;
        bool IndoorAbilitySet : 1;
        bool IndoorCameraAbility : 1;
        bool AllActions : 1;
        bool bits_34_4_8 : 8 - 4;

        // 0x0035:

        bool Pause : 1;
        bool Map : 1;
        bool bits_35_2_8 : 8 - 2;

        // 0x0036:

        bool CanSwitchTools : 1;
        bool CanUseSmokeBomb : 1;
        bool CanUseMoneyPouch : 1;
        bool CanUseCherryBomb : 1;
        bool CanUsePoisonGas : 1;
        bool CanUseStunGrenade : 1;
        bool CanUseHealthPotion : 1;
        bool bits_36_7_8 : 8 - 7;

        // 0x0037:

        bool KeepCurrentTarget : 1;
        bool bits_37_1_8 : 8 - 1;
    } flags;
}; //Size: 0x0038
assert_offsetof(AssassinAbilitySet, flags.pad_1D, 0x1D);
assert_offsetof(AssassinAbilitySet, flags.pad_33, 0x33);
assert_sizeof(AssassinAbilitySet, 0x38);
