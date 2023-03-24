#include "pch.h"


#include "MyLog.h"
#include "MainConfig.h"
#include "ImGuiCTX.h"
#include "ImGui3D.h"

#include "AutoAssemblerKinda.h"

#include "Hack_GameRawInputHook.h"

#include "Hack_EnterWindowsWhenRisPressed.h"
#include "Hack_SlowMenacingWalkAndAutowalk.h"
#include "Hack_CycleEquipmentWhenScrollingMousewheel.h"
#include "Hack_ModifyAimingFOV.h"
#include "Hack_DontForceUnsheatheWhenInDisguise.h"
#include "Hack_CrouchFix.h"
#include "Hack_ReworkedTakeCover.h"

#include "Hack_WhistleAbility.h"
#include "Hacks_VariousExperiments.h"

#include "Cheat_BatlampOfFranciade.h"
#include "Cheat_Health.h"


extern bool g_showDevExtraOptions;
BindableKeyCode_Keyboard enterWindowsButton = BindableKeyCode_Keyboard::K_R;
BindableKeyCode_Keyboard autowalkButton = BindableKeyCode_Keyboard::K_B;
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/EnumAdapter.h"
#define TO_STRING(x) #x
namespace ImGui {
template<typename EnumType>
bool DrawEnumPicker(const char* label, EnumType& currentValueInOut, ImGuiComboFlags flags)
{
    bool isNewSelection = false;
    auto itemsStrings = enum_reflection<EnumType>::GetAllStrings();
    auto itemsValues = enum_reflection<EnumType>::GetAllValues();
    auto it = std::find(itemsValues.begin(), itemsValues.end(), currentValueInOut);
    int item_current_idx = it - itemsValues.begin();                    // Here our selection data is an index.
    if (ImGui::BeginCombo(label, itemsStrings[item_current_idx], flags))
    {
        for (int n = 0; n < itemsStrings.size(); n++)
        {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(itemsStrings[n], is_selected)) {
                item_current_idx = n;
                currentValueInOut = itemsValues[n];
                isNewSelection = true;
            }
        }
        ImGui::EndCombo();
    }
    return isNewSelection;
}
template<class Hack>
void DrawCheckboxForHack(Hack& hack, const std::string_view& text)
{
    if (auto* instance = &hack)
    {
        bool isActive = instance->IsActive();
        if (ImGui::Checkbox(text.data(), &isActive))
        {
            instance->Toggle();
        }
    }
}
} // namespace ImGui
class MyHacks
{
public:
    AutoAssembleWrapper<GameRawInputHook> gameInputHooks;

    AutoAssembleWrapper<EnterWindowWhenRisPressed> enterWindowsByPressingAButton;
    AutoAssembleWrapper<AllowSlowMenacingWalkAndAutowalk> menacingWalkAndAutowalk;
    AutoAssembleWrapper<ModifyConditionalFOVs> changeZoomLevelsWhenAimingBombs;
    AutoAssembleWrapper<InputInjection_CycleEquipmentWhenScrollingMousewheel> cycleEquipmentUsingMouseWheel;
    AutoAssembleWrapper<PlayWithFOV> fovGames;
    AutoAssembleWrapper<PlayWithBombAimCameraTracker2> bombAimExperiments2;
    AutoAssembleWrapper<DontUnsheatheLikeAnIdiotWhileInDisguise> dontUnsheatheWhenInDisguise;
    AutoAssembleWrapper<CrouchFix> slightlyMoreResponsiveCrouch;
    AutoAssembleWrapper<ReworkedTakeCover> takingCoverIsLessSticky;

    AutoAssembleWrapper<WhistleAbility> whistleAbility;

    void DrawControls()
    {
        if (ImGui::Button("Save config file"))
        {
            this->WriteConfig(MainConfig::GetConfigJSON());
            MainConfig::WriteToFile();
        }
        //ImGui::DrawCheckboxForHack(whistleAbility, "Whistle ability");
        //if (ImGui::IsItemHovered())
        //{
        //    ImGui::SetTooltip("Press Y to _try_to_ attract nearby guards' attention. Very unfinished.");
        //}
        //WhistleAbilityAttempt_ImGuiControls(whistleAbility.IsActive());
        ImGui::DrawCheckboxForHack(enterWindowsByPressingAButton, "Enter nearby windows by pressing a button");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "When climbing on a wall, press the specified key (default 'R' like in Syndicate)\n"
                "to enter a nearby window.\n"
                "Additionally, if standing on the ground, enter the nearby Hide Spot\n"
                "(closet with curtains)."
            );
        }
        if (enterWindowsByPressingAButton.IsActive())
        {
            ImGuiCTX::Indent _indent;
            bool isHotkeyChanged = ImGui::DrawEnumPicker("Enter Window Button", enterWindowsButton, ImGuiComboFlags_HeightLarge);
        }
        ImGui::DrawCheckboxForHack(menacingWalkAndAutowalk, "Allow Autowalk and the Slow Menacing Walk");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "CapsLock toggles the slow and menacing walk;\n"
                "When walking in any direction, tap the Autowalk key. Then let go\n"
                "of directional keys, and Arno will keep walking in the same direction.\n"
                "Alternatively:\n"
                " - Stand still\n"
                " - Press and release Autowalk key\n"
                " - Within the next second or so, start walking and let go."
            );
        }
        if (menacingWalkAndAutowalk.IsActive())
        {
            ImGuiCTX::Indent _indent;
            bool isHotkeyChanged = ImGui::DrawEnumPicker("Autowalk button", autowalkButton, ImGuiComboFlags_HeightLarge);
        }
        ImGui::DrawCheckboxForHack(changeZoomLevelsWhenAimingBombs, "Change Zoom Levels when aiming Bombs");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "FOV is increased when aiming bombs and the Guillotine Gun.\n"
                "Press Right Mouse Button when aiming a bomb to zoom in."
            );
        }
        ImGui::DrawCheckboxForHack(cycleEquipmentUsingMouseWheel, "Cycle through equipment using mouse wheel");
        ImGui::DrawCheckboxForHack(dontUnsheatheWhenInDisguise, "Don't pull out your weapon while you're in Disguise");
        ImGui::DrawCheckboxForHack(slightlyMoreResponsiveCrouch, "Slightly more responsive Crouch button");
        ImGui::DrawCheckboxForHack(takingCoverIsLessSticky, "Less sticky Take Cover/Leave Cover");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "By default, Take Cover action is only attempted when _just_pressed_ the Take Cover button (Spacebar).\n"
                "With this activated, Arno will attempt to take cover all the time\n"
                "while Spacebar is pressed (and staying in low profile).\n"
                "Also, it's a little easier to detach from cover by moving in the direction away from it.\n"
                "Also, when behind cover and reached its edge, player leaves cover\n"
                "instead of leaning around the corner."
            );
        }
        if (g_showDevExtraOptions)
        {
            if (ImGuiCTX::TreeNode _header{ "Unfinished and useless hacks" })
            {
                // This is one of the useless experimental hacks, and has some stuttering I either didn't see or notice before.
                ImGui::DrawCheckboxForHack(fovGames, "Play with FOV");
                ImGui::DrawCheckboxForHack(bombAimExperiments2, "Bomb aim experiments2");
            }
        }
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Cheats"))
        {
            ImGui::DrawCheckboxForHack(batlampOfFranciade, "Allow use the Head of Saint Denis outdoors in Franciade");
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "Please read the options after activation."
                );
            }
            if (batlampOfFranciade.IsActive())
            {
                DrawBatlampControls();
            }
            Cheat_Health_DrawImGui();
        }
    }
    void ReadConfig(JSON& cfg)
    {
        {
            JSON& section = cfg[TO_STRING(enterWindowsByPressingAButton)];
            bool isActive = true;
            READ_JSON_VARIABLE(section, isActive, BooleanAdapter);
            READ_JSON_VARIABLE(section, enterWindowsButton, EnumAdapter);
            enterWindowsByPressingAButton.Toggle(isActive);
        }
        {
            JSON& section = cfg[TO_STRING(menacingWalkAndAutowalk)];
            bool isActive = true;
            READ_JSON_VARIABLE(section, isActive, BooleanAdapter);
            READ_JSON_VARIABLE(section, autowalkButton, EnumAdapter);
            menacingWalkAndAutowalk.Toggle(isActive);
        }
        { bool isActive = true; json::TryToReadVariableFromJSONObjectUsingAdapter(cfg, TO_STRING(changeZoomLevelsWhenAimingBombs), BooleanAdapter(isActive)); changeZoomLevelsWhenAimingBombs.Toggle(isActive); }
        { bool isActive = true; json::TryToReadVariableFromJSONObjectUsingAdapter(cfg, TO_STRING(cycleEquipmentUsingMouseWheel), BooleanAdapter(isActive)); cycleEquipmentUsingMouseWheel.Toggle(isActive); }
        { bool isActive = true; json::TryToReadVariableFromJSONObjectUsingAdapter(cfg, TO_STRING(dontUnsheatheWhenInDisguise), BooleanAdapter(isActive)); dontUnsheatheWhenInDisguise.Toggle(isActive); }
        { bool isActive = true; json::TryToReadVariableFromJSONObjectUsingAdapter(cfg, TO_STRING(slightlyMoreResponsiveCrouch), BooleanAdapter(isActive)); slightlyMoreResponsiveCrouch.Toggle(isActive); }
        { bool isActive = true; json::TryToReadVariableFromJSONObjectUsingAdapter(cfg, TO_STRING(takingCoverIsLessSticky), BooleanAdapter(isActive)); takingCoverIsLessSticky.Toggle(isActive); }
    }
    void WriteConfig(JSON& cfg)
    {
        {
            JSON& section = cfg[TO_STRING(enterWindowsByPressingAButton)];
            bool isActive = enterWindowsByPressingAButton.IsActive();
            WRITE_JSON_VARIABLE(section, isActive, BooleanAdapter);
            WRITE_JSON_VARIABLE(section, enterWindowsButton, EnumAdapter);
        }
        {
            JSON& section = cfg[TO_STRING(menacingWalkAndAutowalk)];
            bool isActive = menacingWalkAndAutowalk.IsActive();
            WRITE_JSON_VARIABLE(section, isActive, BooleanAdapter);
            WRITE_JSON_VARIABLE(section, autowalkButton, EnumAdapter);
        }
        { bool isActive = changeZoomLevelsWhenAimingBombs.IsActive(); json::WriteVariableAsJSONObjectMemberUsingAdapter(cfg, TO_STRING(changeZoomLevelsWhenAimingBombs), BooleanAdapter(isActive)); }
        { bool isActive = cycleEquipmentUsingMouseWheel.IsActive(); json::WriteVariableAsJSONObjectMemberUsingAdapter(cfg, TO_STRING(cycleEquipmentUsingMouseWheel), BooleanAdapter(isActive)); }
        { bool isActive = dontUnsheatheWhenInDisguise.IsActive(); json::WriteVariableAsJSONObjectMemberUsingAdapter(cfg, TO_STRING(dontUnsheatheWhenInDisguise), BooleanAdapter(isActive)); }
        { bool isActive = slightlyMoreResponsiveCrouch.IsActive(); json::WriteVariableAsJSONObjectMemberUsingAdapter(cfg, TO_STRING(slightlyMoreResponsiveCrouch), BooleanAdapter(isActive)); }
        { bool isActive = takingCoverIsLessSticky.IsActive(); json::WriteVariableAsJSONObjectMemberUsingAdapter(cfg, TO_STRING(takingCoverIsLessSticky), BooleanAdapter(isActive)); }
    }
    AutoAssembleWrapper<BatlampOfFrancide> batlampOfFranciade;
};
std::optional<MyHacks> g_MyHacks;
void DrawHacksControls()
{
    if (g_MyHacks)
    {
        g_MyHacks->DrawControls();
    }
}

#include "MyVariousHacks.h"


void MyVariousHacks::Start()
{
    g_MyHacks.emplace();
    g_MyHacks->gameInputHooks.Activate();
    JSON& cfg = MainConfig::GetConfigJSON();
    g_MyHacks->ReadConfig(cfg);
    g_MyHacks->WriteConfig(cfg);
    MainConfig::WriteToFile();
}
void MyVariousHacks::MyHacks_OnKeyJustPressed(int keyCode)
{
}
