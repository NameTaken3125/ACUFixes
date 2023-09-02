#pragma once

#include "SoundSet.h"
#include "SoundEvent.h"

class UISounds : public SoundSet
{
public:
    enum UISoundsEnum : int
    {
        k_unk0 = 0,
        k_SlotBerserkBlades = 2,
        k_SlotCherryBomb = 3,
        k_SomeInterestingSound_4 = 4,
        k_SlotMoneyPouch = 5,
        k_SlotFirearm = 6,
        k_SlotPoisonGas = 7,
        k_SlotSmokeBomb = 11,
        k_SlotStunGrenade = 12,
        k_SlotPhantomBlades = 13,
        k_SomeInterestingSound_14 = 14,
        k_EquipmentLocked = 15,
        k_MenuScroll = 20,
        k_ItemAdded_mb_22 = 22,
        k_ItemAdded_mb_25 = 25,
        k_ItemAdded_mb_26 = 26,
        k_DisguiseFailed = 30,
        k_MainMenuIntro = 31,
        k_MainMenuAfterPressAnyButtonToEndIntro = 32,
        k_SomeInterestingSound_34 = 34,
        k_SomeInterestingSound_35 = 35,
        k_SomeInterestingSound_36 = 36,
        k_MainMenuTheme = 37,
        k_MainMenuEndTheme = 38,
        k_MissionCompleted_mb = 42,
        k_SomeInterestingSound_67 = 67,
        k_EquipmentAdded_mb = 89,
        k_TimerAlmostOutOfTime = 109,
        k_TimerLastSecondsCountdown = 110,
        k_SomeInterestingSound_112 = 112,


        k_NUM_ITEMS = 115,
    };
public:
    // @members
    SoundInstance arrSounds[UISoundsEnum::k_NUM_ITEMS];

    // @helper_functions
    static UISounds* GetSingleton();
}; //Size: 0x15A0
assert_sizeof(UISounds, 0x15A0);
