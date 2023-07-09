#pragma once

#include "Enum_BindableKeyCode_Keyboard.h"

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
    struct Hacks : YAConfigSection {
        YACSTOR(Hacks);
        struct EnterWindows : YAConfigSection {
            YACSTOR(EnterWindows);
            ACM(isActive, bool, BooleanAdapter, true);
            ACM(alsoEnterNearbyHidespotClosets, bool, BooleanAdapter, true);
            ACM(enterWindowsButton, BindableKeyCode_Keyboard, EnumAdapter_template<BindableKeyCode_Keyboard>, BindableKeyCode_Keyboard::K_R);
        };
        ACM(enterWindowsByPressingAButton, EnterWindows, YAConfigSectionAdapter, );
        struct MenacingWalk : YAConfigSection {
            YACSTOR(MenacingWalk);
            ACM(isActive, bool, BooleanAdapter, true);
            ACM(autowalkButton, BindableKeyCode_Keyboard, EnumAdapter_template<BindableKeyCode_Keyboard>, BindableKeyCode_Keyboard::K_B);
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
            ACM(hoodToggleButton, BindableKeyCode_Keyboard, EnumAdapter_template<BindableKeyCode_Keyboard>, BindableKeyCode_Keyboard::K_H);
        };
        ACM(hoodControls, HoodControls, YAConfigSectionAdapter, );
        ACM(lookbehindWhenPressingMiddleMouseButton, bool, BooleanAdapter, false);
        ACM(slowmotionTrick, bool, BooleanAdapter, false);
    };
    ACM(hacks, Hacks, YAConfigSectionAdapter, );
    struct Cheats : YAConfigSection {
        YACSTOR(Cheats);
        struct BatlampOfFranciadeCfg : YAConfigSection {
            YACSTOR(BatlampOfFranciadeCfg);
            ACM(isActive, bool, BooleanAdapter, false);
            ACM(doReplaceNormalLampWithMagical, bool, BooleanAdapter, false);
            ACM(doUnlockMovementWithTheBatlamp, bool, BooleanAdapter, true);
            ACM(batlampChargeModeButton, BindableKeyCode_Keyboard, EnumAdapter_template<BindableKeyCode_Keyboard>, BindableKeyCode_Keyboard::K_N);
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
            ACM(diveHelper, bool, BooleanAdapter, true);
        };
        ACM(parkourHelper, YACS_ParkourHelper, YAConfigSectionAdapter, );
    };
    ACM(personalRequests, PersonalRequests, YAConfigSectionAdapter, );
};
extern ConfigTop g_Config;
