#pragma once

#include "Enum_BindableKeyCode.h"
#include "Enum_VirtualKeys.h"

namespace MainConfig {

void FindAndLoadConfigFileOrCreateDefault();
void WriteToFile();

} // namespace MainConfig

#include "OLYAFSer/OLYAFSer.h"
#include "Serialization/EnumAdapter.h"

#define ACM(varName, VarType, AdapterType, optionalDefaultValue) ADD_CONFIG_MEMBER(varName, VarType, AdapterType, optionalDefaultValue)
#define YACSTOR(SubclsName) YACONFIGSECTION_SUBCLASS_CTOR(SubclsName)
struct ConfigTop : YAConfigSection {
    YACSTOR(ConfigTop);
    ACM(imgui_useImGui, bool, BooleanAdapter, true);
    ACM(imgui_showSuccessfulInjectionIndicator, bool, BooleanAdapter, true);
    ACM(hotkey_ToggleMenu, VirtualKeys, EnumAdapter_template<VirtualKeys>, VirtualKeys::KEYBOARD_INSERT);
    ACM(hotkey_UnloadMod, VirtualKeys, EnumAdapter_template<VirtualKeys>, VirtualKeys::KEYBOARD_END);
    struct Hacks : YAConfigSection {
        YACSTOR(Hacks);
        struct EnterWindows : YAConfigSection {
            YACSTOR(EnterWindows);
            ACM(isActive, bool, BooleanAdapter, true);
            ACM(alsoEnterNearbyHidespotClosets, bool, BooleanAdapter, true);
            ACM(enterWindowsButton, BindableKeyCode, EnumAdapter_template<BindableKeyCode>, BindableKeyCode::KEYBOARD_R);
        };
        ACM(enterWindowsByPressingAButton, EnterWindows, YAConfigSectionAdapter, );
        struct MenacingWalk : YAConfigSection {
            YACSTOR(MenacingWalk);
            ACM(isActive, bool, BooleanAdapter, true);
            ACM(autowalkButton, BindableKeyCode, EnumAdapter_template<BindableKeyCode>, BindableKeyCode::KEYBOARD_B);
        };
        ACM(menacingWalkAndAutowalk, MenacingWalk, YAConfigSectionAdapter, );
        ACM(changeZoomLevelsWhenAimingBombs, bool, BooleanAdapter, true);
        struct YACS_moreSituationsToDropBombs : YAConfigSection {
            YACSTOR(YACS_moreSituationsToDropBombs);
            ACM(isActive, bool, BooleanAdapter, true);
            ACM(alsoMoreResponsiveInCombat, bool, BooleanAdapter, true);
        };
        ACM(moreSituationsToDropBombs, YACS_moreSituationsToDropBombs, YAConfigSectionAdapter, );
        struct YACS_MoreReliableQuickshot : YAConfigSection {
            YACSTOR(YACS_MoreReliableQuickshot);
            ACM(isActive, bool, BooleanAdapter, true);
            ACM(lessQuickshotRestrictions, bool, BooleanAdapter, true);
        };
        ACM(moreReliableQuickshot, YACS_MoreReliableQuickshot, YAConfigSectionAdapter, );
        ACM(cycleEquipmentUsingMouseWheel, bool, BooleanAdapter, true);
        ACM(dontUnsheatheWhenInDisguise, bool, BooleanAdapter, true);
        ACM(slightlyMoreResponsiveCrouch, bool, BooleanAdapter, true);
        ACM(takingCoverIsLessSticky, bool, BooleanAdapter, true);
        ACM(breakfallAndCatchLedgeByPressingE, bool, BooleanAdapter, true);
        ACM(easierTurnWhenSwingingOnAHorizontalBar, bool, BooleanAdapter, true);
        ACM(aimBombsFromPeaks, bool, BooleanAdapter, true);
        ACM(noMoreImaginaryBombThrows, bool, BooleanAdapter, true);
        ACM(dontRandomlyTurn180degrees, bool, BooleanAdapter, true);
        ACM(automaticallyReloadWeaponsWhenRefillAllInShops, bool, BooleanAdapter, true);
        struct HoodControls : YAConfigSection {
            YACSTOR(HoodControls);
            ACM(isActive, bool, BooleanAdapter, true);
            ACM(hoodToggleButton, BindableKeyCode, EnumAdapter_template<BindableKeyCode>, BindableKeyCode::KEYBOARD_H);
        };
        ACM(hoodControls, HoodControls, YAConfigSectionAdapter, );
        ACM(allowCustomizeEquipmentOnLedges, bool, BooleanAdapter, true);
        ACM(slightlyGreaterGuillotineGunQuickshotRange, bool, BooleanAdapter, true);
        ACM(noWaitForUnsafeEject, bool, BooleanAdapter, false);
        struct YACS_LookbehindButton : YAConfigSection {
            YACSTOR(YACS_LookbehindButton);
            ACM(isActive, bool, BooleanAdapter, false);
            ACM(hotkey, BindableKeyCode, EnumAdapter_template<BindableKeyCode>, BindableKeyCode::MOUSE_MMB);
        };
        ACM(lookbehindButton, YACS_LookbehindButton, YAConfigSectionAdapter, );
        struct YACS_SlowmotionTrick : YAConfigSection {
            YACSTOR(YACS_SlowmotionTrick);
            ACM(isActive, bool, BooleanAdapter, false);
            ACM(hotkey, BindableKeyCode, EnumAdapter_template<BindableKeyCode>, BindableKeyCode::MOUSE_X4);
        };
        ACM(slowmotionTrick, YACS_SlowmotionTrick, YAConfigSectionAdapter, );
    };
    ACM(hacks, Hacks, YAConfigSectionAdapter, );
    struct Cheats : YAConfigSection {
        YACSTOR(Cheats);
        struct BatlampOfFranciadeCfg : YAConfigSection {
            YACSTOR(BatlampOfFranciadeCfg);
            ACM(isActive, bool, BooleanAdapter, false);
            ACM(doReplaceNormalLampWithMagical, bool, BooleanAdapter, false);
            ACM(doUnlockMovementWithTheBatlamp, bool, BooleanAdapter, true);
            ACM(batlampChargeModeButton, BindableKeyCode, EnumAdapter_template<BindableKeyCode>, BindableKeyCode::KEYBOARD_N);
        };
        ACM(batlampOfFranciadeManipulations, BatlampOfFranciadeCfg, YAConfigSectionAdapter, );
        ACM(infiniteAmmo, bool, BooleanAdapter, false);
    };
    ACM(cheats, Cheats, YAConfigSectionAdapter, );
    struct PersonalRequests : YAConfigSection {
        YACSTOR(PersonalRequests);
        struct YACS_ParkourHelper : YAConfigSection {
            YACSTOR(YACS_ParkourHelper);
            ACM(isActive, bool, BooleanAdapter, false);
            ACM(spinningDescentHelper, bool, BooleanAdapter, true);
            struct YACS_DiveHelper : YAConfigSection {
                YACSTOR(YACS_DiveHelper);
                ACM(isActive, bool, BooleanAdapter, true);
                ACM(hotkey, BindableKeyCode, EnumAdapter_template<BindableKeyCode>, BindableKeyCode::MOUSE_X5);
            };
            ACM(diveHelper, YACS_DiveHelper, YAConfigSectionAdapter, );
        };
        ACM(parkourHelper, YACS_ParkourHelper, YAConfigSectionAdapter, );
    };
    ACM(personalRequests, PersonalRequests, YAConfigSectionAdapter, );
};
extern ConfigTop g_Config;
