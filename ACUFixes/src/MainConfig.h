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
        ACM(cycleEquipmentUsingMouseWheel, bool, BooleanAdapter, true);
        ACM(dontUnsheatheWhenInDisguise, bool, BooleanAdapter, true);
        ACM(slightlyMoreResponsiveCrouch, bool, BooleanAdapter, true);
        ACM(takingCoverIsLessSticky, bool, BooleanAdapter, true);
        ACM(breakfallAndCatchLedgeByPressingE, bool, BooleanAdapter, true);
        ACM(easierTurnWhenSwingingOnAHorizontalBar, bool, BooleanAdapter, true);
        ACM(slightlyMoreResponsiveBombQuickDrop, bool, BooleanAdapter, true);
        ACM(moreSituationsToDropBombs, bool, BooleanAdapter, false);
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
    };
    ACM(cheats, Cheats, YAConfigSectionAdapter, );
};
extern ConfigTop g_Config;
