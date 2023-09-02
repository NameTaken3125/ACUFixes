#pragma once

#include "SoundSet.h"
#include "SoundEvent.h"
#include "SmallArray.h"

class ACUGlobalSoundSet : public SoundSet
{
public:
    enum SoundsEnum : int
    {
        k_YellowIconAppears_mb_16 = 16,
        k_ConflictStarts = 17,
        k_ConflictFinished_mb_18 = 18,
        k_PlayerLastKnownLocationGhostAppears = 19,
        k_ChaseResumed_mb = 22,
        k_ConflictFinished_mb_23 = 23,
        k_YellowIconAppears_mb_24 = 24,
        k_OnStartDisguise = 25,
        k_LikeStartDisguiseButSharper = 26,
        k_LikeDisguiseBrokenWithSlightDelay = 27,
        k_ClothSoundSharp = 28,
        k_OnDisguiseBroken = 29,
        k_onFailedDisguiseOrMedicineHealthFull = 31,
        k_EagleVisionStart_44 = 44,
        k_EagleVisionEnd_45 = 45,
        k_EagleVisionStart_46 = 46,
        k_EagleVisionEnd_47 = 47,
        k_EagleVisionMarksATarget = 48,
        k_NostradamusMarkNearby = 51,
        k_NostradamusMarkEnd = 52,
        k_AlarmBellRinged = 54,
        k_AlarmBellRingFinish = 55,
        k_AssassinationPromptLikeInNotreDame_mb = 58,
        k_SomeTicking_59 = 59,
        k_SomeTicking_60 = 60,
        k_SomeTicking_61 = 61,
        k_SomeTicking_62 = 62,
        k_SomeTickingEnd = 63,
        k_Gunshot = 91,
        k_GunTriggerClick = 93,

        k_NUM_ITEMS = 111,
    };
public:
    // @members
    SmallArray<SoundInstance> arrSoundEvents_10;
    char pad_1C[0x1C];
    SoundInstance arrSoundEvents_38[SoundsEnum::k_NUM_ITEMS];
    char pad_1508[8];

    // @helper_functions
    static ACUGlobalSoundSet* GetSingleton();
}; //Size: 0x1510
assert_offsetof(ACUGlobalSoundSet, arrSoundEvents_38, 0x38);
assert_offsetof(ACUGlobalSoundSet, arrSoundEvents_38[ACUGlobalSoundSet::k_OnDisguiseBroken], 0x5A8);
assert_offsetof(ACUGlobalSoundSet, arrSoundEvents_38[ACUGlobalSoundSet::k_onFailedDisguiseOrMedicineHealthFull], 0x608);
assert_sizeof(ACUGlobalSoundSet, 0x1510);
