#include "pch.h"

#include "AutoAssemblerKinda.h"
#include "Hack_EnterWindowsWhenRisPressed.h"
#include "Hack_SlowMenacingWalkAndAutowalk.h"
#include "Hack_CycleEquipmentWhenScrollingMousewheel.h"
#include "Hack_ModifyAimingFOV.h"
#include "Hack_DontForceUnsheatheWhenInDisguise.h"
#include "Hack_CrouchFix.h"
#include "Hack_ReworkedTakeCover.h"
#include "MyLog.h"
#include "MainConfig.h"
#include "ImGuiCTX.h"
#include "ImGui3D.h"

#include "Cheat_BatlampOfFranciade.h"

template<typename floatlike>
floatlike simple_interp(floatlike mn, floatlike mx)
{
    auto now = GetTickCount64();
    float speed = 0.001f * 1.5f;
    float interp = sin(now * speed);
    interp = (interp + 1) / 2;
    return mn + (mx - mn) * interp;
}
void FOVGames(AllRegisters* params)
{
    params->XMM1.f0 = simple_interp(0.5f, 1.0f);
}
struct PlayWithFOV : AutoAssemblerCodeHolder_Base
{
    PlayWithFOV()
    {
        PresetScript_CCodeInTheMiddle(
            0x141F3FE3B, 6
            , FOVGames
            , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
            , true);
    }
};

#include "ACU/ACUGetSingletons.h"
#include "ACU/Entity.h"
#include "ACU/ThrowTargetPrecision.h"
void OverrideThrowPredictorBeamPosition(AllRegisters* params)
{
    // At this injection point (0x14055B4BB) RBX == ThrowTargetPrecision* and takes two values:
    // two systems that receive messages about the predictor beam's results.
    // One regulates camera rotation around Z, the other one - rotation around camera's left-right axis.
    Entity* player = ACU::GetPlayer();
    static Vector4f farthestResult;
    ThrowTargetPrecision* thr = (ThrowTargetPrecision*)params->rbx_;
    ImGui3D::DrawLocationNamed((Vector3f&)thr->predictionBeamOrigin, "Prediction beam origin");
    ImGui3D::DrawLocationNamed((Vector3f&)thr->trackerCrawlsTowardPredictorBeamEnd, "Prediction beam terminus");
    thr->trackerCrawlsTowardPredictorBeamEnd.z = player->GetPosition().z + 1;
    //(Vector3f&)thr->cameraTrackerMovingTowardPredictionBeamEnd = player->GetPosition() + Vector3f{ 0, 3, 1 };
}
struct PlayWithBombAimCameraTracker2 : AutoAssemblerCodeHolder_Base
{
    PlayWithBombAimCameraTracker2()
    {
        PresetScript_CCodeInTheMiddle(
            0x14055B4BB, 8
            , OverrideThrowPredictorBeamPosition
            , RETURN_TO_RIGHT_AFTER_STOLEN_BYTES
            , true);
    }
};
extern bool g_showDevExtraOptions;
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
#include "Serialization/EnumAdapter.h"
#define TO_STRING(x) #x
BindableKeyCode_Keyboard enterWindowsButton = BindableKeyCode_Keyboard::K_R;
BindableKeyCode_Keyboard autowalkButton = BindableKeyCode_Keyboard::K_B;
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
} // namespace ImGui
class MyHacks
{
public:
    AutoAssembleWrapper<EnterWindowWhenRisPressed> enterWindowsByPressingAButton;
    AutoAssembleWrapper<AllowSlowMenacingWalkAndAutowalk> menacingWalkAndAutowalk;
    AutoAssembleWrapper<ModifyConditionalFOVs> changeZoomLevelsWhenAimingBombs;
    AutoAssembleWrapper<InputInjection_CycleEquipmentWhenScrollingMousewheel> cycleEquipmentUsingMouseWheel;
    AutoAssembleWrapper<PlayWithFOV> fovGames;
    AutoAssembleWrapper<PlayWithBombAimCameraTracker2> bombAimExperiments2;
    AutoAssembleWrapper<DontUnsheatheLikeAnIdiotWhileInDisguise> dontUnsheatheWhenInDisguise;
    AutoAssembleWrapper<CrouchFix> slightlyMoreResponsiveCrouch;
    AutoAssembleWrapper<ReworkedTakeCover> takingCoverIsLessSticky;

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
    void ToggleDefaultHacks()
    {
        if (!enterWindowsByPressingAButton.IsActive())
        {
            enterWindowsByPressingAButton.Activate();
            menacingWalkAndAutowalk.Activate();
            changeZoomLevelsWhenAimingBombs.Activate();
            cycleEquipmentUsingMouseWheel.Activate();
        }
        else
        {
            enterWindowsByPressingAButton.Deactivate();
            menacingWalkAndAutowalk.Deactivate();
            changeZoomLevelsWhenAimingBombs.Deactivate();
            cycleEquipmentUsingMouseWheel.Deactivate();
        }
    }
    void OnKeyJustPressed(int keyCode)
    {
        switch (keyCode)
        {
        case VK_NUMPAD7:
            //ToggleDefaultHacks();
            break;
        default:
            break;
        }
    }
    void DrawControls()
    {
        if (ImGui::Button("Save config file"))
        {
            this->WriteConfig(MainConfig::GetConfigJSON());
            MainConfig::WriteToFile();
        }
        DrawCheckboxForHack(enterWindowsByPressingAButton, "Enter nearby windows by pressing a button");
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
        DrawCheckboxForHack(menacingWalkAndAutowalk, "Allow Autowalk and the Slow Menacing Walk");
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
        DrawCheckboxForHack(changeZoomLevelsWhenAimingBombs, "Change Zoom Levels when aiming Bombs");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "FOV is increased when aiming bombs and the Guillotine Gun.\n"
                "Press Right Mouse Button when aiming a bomb to zoom in."
            );
        }
        DrawCheckboxForHack(cycleEquipmentUsingMouseWheel, "Cycle through equipment using mouse wheel");
        DrawCheckboxForHack(dontUnsheatheWhenInDisguise, "Don't pull out your weapon while you're in Disguise");
        DrawCheckboxForHack(slightlyMoreResponsiveCrouch, "Slightly more responsive Crouch button");
        DrawCheckboxForHack(takingCoverIsLessSticky, "Less sticky Take Cover/Leave Cover");
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
            //// This is one of the useless experimental hacks, and has some stuttering I either didn't see or notice before.
            //DrawCheckboxForHack(fovGames, "Play with FOV");
            DrawCheckboxForHack(bombAimExperiments2, "Bomb aim experiments2");
        }
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Cheats"))
        {
            DrawCheckboxForHack(batlampOfFranciade, "Allow use the Head of Saint Denis outdoors in Franciade");
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
    JSON& cfg = MainConfig::GetConfigJSON();
    g_MyHacks->ReadConfig(cfg);
    g_MyHacks->WriteConfig(cfg);
    MainConfig::WriteToFile();
}
void MyVariousHacks::MyHacks_OnKeyJustPressed(int keyCode)
{
    if (g_MyHacks)
    {
        g_MyHacks->OnKeyJustPressed(keyCode);
    }
}
