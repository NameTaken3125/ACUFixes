#include "pch.h"


#include "MyLog.h"
#include "MainConfig.h"
#include "ImGuiCTX.h"
#include "ImGui3D.h"

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

#include "Hack_GameRawInputHook.h"

#include "Hack_EnterWindowsWhenRisPressed.h"
#include "Hack_SlowMenacingWalkAndAutowalk.h"
#include "Hack_CycleEquipmentWhenScrollingMousewheel.h"
#include "Hack_ModifyAimingFOV.h"
#include "Hack_DontForceUnsheatheWhenInDisguise.h"
#include "Hack_CrouchFix.h"
#include "Hack_ReworkedTakeCover.h"
#include "Hack_BreakfallAndLedgeGrab.h"
#include "Hack_EasierTurnWhenSwinging.h"
#include "Hack_LookbehindButton.h"

#include "Hack_WhistleAbility.h"
#include "Hacks_VariousExperiments.h"

#include "Cheat_BatlampOfFranciade.h"
#include "Cheat_PretendYoureInFranciade.h"
#include "Cheat_Health.h"
#include "Cheat_Ammo.h"
#include "Cheat_DisguiseUpgrades.h"



void DrawSlowMotionControls();
void DrawSlowMotionTrickControls();

extern bool g_showDevExtraOptions;
#include "ImGuiConfigUtils.h"
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
    AutoAssembleWrapper<LookbehindButton> lookbehindButton;
    AutoAssembleWrapper<BreakfallAndLedgeGrab> breakfallAndCatchLedgeByPressingE;
    AutoAssembleWrapper<EasierTurnWhenSwinging> easierTurnWhenSwingingOnAHorizontalBar;

    AutoAssembleWrapper<WhistleAbility> whistleAbility;

    void DrawControls()
    {
        if (ImGui::Button("Save config file"))
        {
            WriteConfig(g_Config);
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
            bool isHotkeyChanged = ImGui::DrawEnumPicker(
                "Enter Window Button",
                g_Config.hacks->enterWindowsByPressingAButton->enterWindowsButton.get(),
                ImGuiComboFlags_HeightLarge);
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
            bool isHotkeyChanged = ImGui::DrawEnumPicker(
                "Autowalk button",
                g_Config.hacks->menacingWalkAndAutowalk->autowalkButton.get(),
                ImGuiComboFlags_HeightLarge);
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
        ImGui::DrawCheckboxForHack(breakfallAndCatchLedgeByPressingE, "More reliable Breakfall and Catch Ledge");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(
                "When in uncontrolled freefall (when Arno is windmilling his arms\n"
                "e.g. after making a long manual jump from a great height,\n"
                "or when being shot down while climbing a wall),\n"
                "hold \"Parkour Down/Interact\" button (default \"E\") to grab\n"
                "the nearest ledge in front of you.\n"
                "If you also hold directional keys, you can catch ledges in a different direction,\n"
                "if there are any close enough.\n"
            );
        }
        ImGui::DrawCheckboxForHack(easierTurnWhenSwingingOnAHorizontalBar, "Easier turn when swinging on a horizontal bar");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(
                "If you hold Sprint+MoveBack when jumping onto a horizontal bar and about to swing on it,\n"
                "Arno will change direction and swing back."
            );
        }

        ImGui::DrawCheckboxForHack(lookbehindButton, "Lookbehind button");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Flip camera around when Middle Mouse Button is pressed.\n"
                "Warning: doesn't work when aiming Guillotine Gun."
            );
        }
        DrawSlowMotionTrickControls();
        if (g_showDevExtraOptions)
        {
            if (ImGuiCTX::TreeNode _header{ "Unfinished and useless hacks" })
            {
                // This is one of the useless experimental hacks, and has some severe stuttering,
                // but only _sometimes_ for some reason.
                //ImGui::DrawCheckboxForHack(fovGames, "Play with FOV");
                ImGui::DrawCheckboxForHack(bombAimExperiments2, "Bomb aim experiments2");
            }
        }
        ImGui::Separator();
        ImGui::Text("Cheats");
        ImGui::Separator();
        DrawCheatsControls();
    }
    void DrawCheatsControls()
    {
        {
            ImGui::DrawCheckboxForHack(batlampOfFranciade, "Allow use the Head of Saint Denis outdoors");
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
            ImGui::DrawCheckboxForHack(dontDecreaseRemainingAmmo, "Infinite ammo");
            if (ImGuiCTX::TreeNode _disguiseUpgradesSection{ "Disguise upgrades" })
            {
                ImGui::DrawCheckboxForHack(disableDisguiseCooldown, "Disable Disguise cooldown");
                ImGui::DrawCheckboxForHack(unbreakableDisguise, "Unbreakable Disguise");
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip(
                        "Being in Disguise makes you \"invisible\" to enemies, and that makes the Unbreakable Disguise\n"
                        "pretty boring for normal gameplay.\n"
                        "If you simply want to use someone else's appearance, you can uncheck the\n"
                        "\"Invisible to enemies\" cheat, and then the Disguise will not prevent detection.\n"
                        "Also, if you'd like to _fight_ someone while wearing Disguise,\n"
                        "you might also want to disable the \"Don't pull out your weapon while you're in Disguise\" fix."
                    );
                }
                ImGui::DrawCheckboxForHack(disguiseDoesntMakeYouInvisible, "If enabled, Disguise will not make you invisible to enemies");
            }
            Cheat_Invisibility_DrawImGui();
            ImGui::DrawCheckboxForHack(pretendYoureInFranciade, "Pretend you're in Franciade");
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "This allows, for example, to bring the Lantern, including the magical\n"
                    "Head of Saint Denis into Paris.\n"
                    "Please consider backing up your savegame before activating.\n"
                    "The side effects are some missing sounds, at the minimum."
                );
            }
            DrawSlowMotionControls();
        }
    }
    void ReadConfig(ConfigTop& cfg)
    {
        auto& hacksSection = cfg.hacks;
        enterWindowsByPressingAButton.Toggle(hacksSection->enterWindowsByPressingAButton->isActive);
        menacingWalkAndAutowalk.Toggle(hacksSection->menacingWalkAndAutowalk->isActive);
        changeZoomLevelsWhenAimingBombs.Toggle(hacksSection->changeZoomLevelsWhenAimingBombs);
        cycleEquipmentUsingMouseWheel.Toggle(hacksSection->cycleEquipmentUsingMouseWheel);
        dontUnsheatheWhenInDisguise.Toggle(hacksSection->dontUnsheatheWhenInDisguise);
        slightlyMoreResponsiveCrouch.Toggle(hacksSection->slightlyMoreResponsiveCrouch);
        takingCoverIsLessSticky.Toggle(hacksSection->takingCoverIsLessSticky);
        breakfallAndCatchLedgeByPressingE.Toggle(hacksSection->breakfallAndCatchLedgeByPressingE);
        easierTurnWhenSwingingOnAHorizontalBar.Toggle(hacksSection->easierTurnWhenSwingingOnAHorizontalBar);
        lookbehindButton.Toggle(hacksSection->lookbehindWhenPressingMiddleMouseButton);
    }
    void WriteConfig(ConfigTop& cfg)
    {
        auto& hacksSection = cfg.hacks;
        hacksSection->enterWindowsByPressingAButton->isActive = enterWindowsByPressingAButton.IsActive();
        hacksSection->menacingWalkAndAutowalk->isActive = menacingWalkAndAutowalk.IsActive();
        hacksSection->changeZoomLevelsWhenAimingBombs = changeZoomLevelsWhenAimingBombs.IsActive();
        hacksSection->cycleEquipmentUsingMouseWheel = cycleEquipmentUsingMouseWheel.IsActive();
        hacksSection->dontUnsheatheWhenInDisguise = dontUnsheatheWhenInDisguise.IsActive();
        hacksSection->slightlyMoreResponsiveCrouch = slightlyMoreResponsiveCrouch.IsActive();
        hacksSection->takingCoverIsLessSticky = takingCoverIsLessSticky.IsActive();
        hacksSection->breakfallAndCatchLedgeByPressingE = breakfallAndCatchLedgeByPressingE.IsActive();
        hacksSection->easierTurnWhenSwingingOnAHorizontalBar = easierTurnWhenSwingingOnAHorizontalBar.IsActive();
        hacksSection->lookbehindWhenPressingMiddleMouseButton = lookbehindButton.IsActive();
    }
    AutoAssembleWrapper<BatlampOfFrancide> batlampOfFranciade;
    AutoAssembleWrapper<AmmoCheat> dontDecreaseRemainingAmmo;
    AutoAssembleWrapper<PretendYoureInFranciade> pretendYoureInFranciade;
    AutoAssembleWrapper<DisableDisguiseCooldown> disableDisguiseCooldown;
    AutoAssembleWrapper<UnbreakableDisguise> unbreakableDisguise;
    AutoAssembleWrapper<DisguiseDoesntMakeYouInvisible> disguiseDoesntMakeYouInvisible;
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
    g_MyHacks->ReadConfig(g_Config);
    g_MyHacks->WriteConfig(g_Config);
    MainConfig::WriteToFile();
}
void MyVariousHacks::MyHacks_OnKeyJustPressed(int keyCode)
{
}
