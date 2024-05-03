#include "pch.h"


#include "MyLog.h"
#include "MainConfig.h"
#include "ImGuiCTX.h"
#include "ImGui3D.h"

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

#include "Hack_EnterWindowsWhenRisPressed.h"
#include "Hack_SlowMenacingWalkAndAutowalk.h"
#include "Hack_CycleEquipmentWhenScrollingMousewheel.h"
#include "Hack_ModifyAimingFOV.h"
#include "Hack_DontForceUnsheatheWhenInDisguise.h"
#include "Hack_CrouchFix.h"
#include "Hack_ReworkedTakeCover.h"
#include "Hack_BreakfallAndLedgeGrab.h"
#include "Hack_EasierTurnWhenSwinging.h"
#include "Hack_AimFromPeaks.h"
#include "Hack_NoMoreFailedBombThrows.h"
#include "Hack_MoreResponsiveBombQuickDrop.h"
#include "Hack_ReloadRangedWeaponsWhenRefillAllInShop.h"
#include "Hack_MoreReliableQuickshot.h"
#include "Hack_DontRandomlyTurn180Degrees.h"
#include "Hack_NoWaitForUnsafeEject.h"
#include "Hack_AllowCustomizeEquipment.h"
#include "Hack_GunRange.h"
#include "Hack_UnequipPistol.h"

#include "Hack_LookbehindButton.h"
#include "Hack_WhistleAbility.h"
#include "Hacks_VariousExperiments.h"

#include "Cheat_BatlampOfFranciade.h"
#include "Cheat_PretendYoureInFranciade.h"
#include "Cheat_Health.h"
#include "Cheat_Ammo.h"
#include "Cheat_DisguiseUpgrades.h"

#include "Request_Spindescent.h"
#include "Request_FreezeFOV.h"

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

struct HookAnimationSignalsThatHumansReceive : AutoAssemblerCodeHolder_Base
{
    HookAnimationSignalsThatHumansReceive();
};
void WhenRegisteredAnimationSignalHasSentAnUpdate(AllRegisters* params)
{
    uint32 animationSignalReceiverIndex = (uint32&)params->rdx_;
    HumanStatesHolder* humanStates = (HumanStatesHolder*)params->rcx_;
    uint8 isSignalPositiveNow = (uint8&)params->r8_;
}
HookAnimationSignalsThatHumansReceive::HookAnimationSignalsThatHumansReceive()
{
    uintptr_t whenRegisteredAnimationSignalHasSentAnUpdate = 0x141942CB0;
    PresetScript_CCodeInTheMiddle(whenRegisteredAnimationSignalHasSentAnUpdate, 5,
        WhenRegisteredAnimationSignalHasSentAnUpdate, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}

void DrawSlowMotionControls();
void DrawSlowMotionTrickControls();
void DrawHoodControls();

extern bool g_showDevExtraOptions;
#include "ImGuiConfigUtils.h"
class MyHacks
{
public:
    AutoAssembleWrapper<EnterWindowWhenRisPressed> enterWindowsByPressingAButton;
    AutoAssembleWrapper<AllowSlowMenacingWalkAndAutowalk> menacingWalkAndAutowalk;
    AutoAssembleWrapper<ModifyConditionalFOVs> changeZoomLevelsWhenAimingBombs;
    AutoAssembleWrapper<InputInjection_CycleEquipmentWhenScrollingMousewheel> cycleEquipmentUsingMouseWheel;
    AutoAssembleWrapper<DontUnsheatheLikeAnIdiotWhileInDisguise> dontUnsheatheWhenInDisguise;
    AutoAssembleWrapper<CrouchFix> slightlyMoreResponsiveCrouch;
    AutoAssembleWrapper<ReworkedTakeCover> takingCoverIsLessSticky;
    AutoAssembleWrapper<LookbehindButton> lookbehindButton;
    AutoAssembleWrapper<BreakfallAndLedgeGrab> breakfallAndCatchLedgeByPressingE;
    AutoAssembleWrapper<EasierTurnWhenSwinging> easierTurnWhenSwingingOnAHorizontalBar;
    AutoAssembleWrapper<MoreSituationsToDropBomb> moreSituationsToDropBombs;
    AutoAssembleWrapper<AimFromPeaks> aimBombsFromPeaks;
    AutoAssembleWrapper<NoMoreFailedBombThrows> noMoreImaginaryBombThrows;
    AutoAssembleWrapper<DontRandomlyTurn180Degrees> dontRandomlyTurn180degrees;
    AutoAssembleWrapper<ReloadRangedWeaponsWhenRefillAllInShop> automaticallyReloadWeaponsWhenRefillAllInShops;
    AutoAssembleWrapper<MoreReliableQuickshot> moreReliableQuickshot;
    AutoAssembleWrapper<NoWaitForUnsafeEject> noWaitForUnsafeEject;
    AutoAssembleWrapper<AllowCustomizeEquipment> allowCustomizeEquipmentOnLedges;
    AutoAssembleWrapper<GuillotineGunRange> guillotineGunRange;
    AutoAssembleWrapper<UnequipPistol> allowUnequipPistol;

    // Unused and unfinished
    AutoAssembleWrapper<PlayWithFOV> fovGames;
    AutoAssembleWrapper<PlayWithBombAimCameraTracker2> bombAimExperiments2;
    AutoAssembleWrapper<WhistleAbility> whistleAbility;
    AutoAssembleWrapper<BetterAimingFromBehindCover> moreConsistentAimBombFromBehindCover;

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
            ImGui::Checkbox("Also enter nearby Hidespot Closets", &g_Config.hacks->enterWindowsByPressingAButton->alsoEnterNearbyHidespotClosets.get());
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
        ImGui::DrawCheckboxForHack(moreSituationsToDropBombs, "Can drop bombs in more situations, e.g. during a jump. Please read.");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Allows to quickly drop bombs in more situations: during a jump, when hanging onto walls,"
                "\nduring an assassination, when hiding in a closet, swinging on a horizontal bar etc."
                "\nBecause this starts a throw animation, one which involves the left arm at the minimum,"
                "\nthere are cases when animations will look out of place"
                "\n(e.g. if you throw a bomb right in the middle of a stab, or when hanging on one arm),"
                "\nand there probably won't be much I can do to make all the animations seamless."
            );
        }
        if (moreSituationsToDropBombs.IsActive())
        {
            ImGuiCTX::Indent _ind;
            ImGui::Checkbox("Also more responsive in combat", &g_Config.hacks->moreSituationsToDropBombs->alsoMoreResponsiveInCombat.get());
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "When in combat, allows dropping bombs while performing a different action,"
                    "\ne.g. during a parry, during a roll, during a finisher."
                );
            }
        }
        ImGui::DrawCheckboxForHack(moreReliableQuickshot, "More reliable Quickshot. Please read.");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Allows to Quickshoot the wristbow and pistols from from peaks and V-shapes"
                "\nlike flagpoles or trees."
                "\nAlso, fixes the very strange situation: when you try to Quickshoot your pistol"
                "\n_while_ sheathing your melee weapon, Arno will instead attempt to reload,"
                "\neven if the magazine is full."
                "\nAlso, potentially, hopefully fixes the very elusive, sometimes gamebreaking Quickshot bug"
                "\nthat can occur if you Quickshoot and quickly do some parkour:"
                "\nyou are left unable to switch ranged weapons and when on a ledge"
                "\nare unable to walk or turn around."
                "\nConsider also enabling the \"Less Quickshot restrictions\" option."
            );
        }
        if (moreReliableQuickshot.IsActive())
        {
            ImGuiCTX::Indent _indent;
            ImGui::Checkbox("Less Quickshot restrictions. Please read.", &g_Config.hacks->moreReliableQuickshot->lessQuickshotRestrictions.get());
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "Normally, the Quickshot is very easily interrupted:"
                    "\nrunning into a table, stepping off a table, even just running into a wall"
                    "\nwithout climbing it can cancel the animation."
                    "\nThis patch removes _most_ of these interruptions, allowing you to, for example,"
                    "\nshoot while sliding across a table for massive style points."
                    "\nAlso, if the \"More situations to drop bombs\" patch is enabled,"
                    "\nallows you to perform Quickshots in most of those situations, too. For example"
                    "\nduring a jump, during a roll, even while hanging on a wall."
                    "\nThis mostly applies to pistols and the wristbow, as shooting"
                    "\na twohanded ranged weapon like a rifle while hanging on a wall makes no sense."
                    "\nSadly, you're still unable to shoot rifles from ledges."
                    "\nBe warned: unlike the Drop Bomb animation, which mostly just uses"
                    "\nthe left arm, the Quickshot animation involves the character's torso too."
                    "\nThis means that there are more opportunities for the animations"
                    "\nto look strange, for example the lower body can perform a roll"
                    "\nwhile the upper body is standing upright in the Quickshot posture."
                    "\nTry it out and see for yourself what looks good."
                );
            }
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
        ImGui::DrawCheckboxForHack(aimBombsFromPeaks, "Can aim bombs from \"peaks\" or \"perches\" (vertical pipes, spires etc.)");
        ImGui::DrawCheckboxForHack(noMoreImaginaryBombThrows, "No more imaginary bomb throws");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "You know how sometimes after aiming and releasing a bomb"
                "\nthe throw animation plays out, but no bomb is actually thrown?"
                "\nThat shouldn't happen anymore."
            );
        }
        ImGui::DrawCheckboxForHack(dontRandomlyTurn180degrees, "Don't randomly flip 180 degrees");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Did it ever happen to you that you're crouched, being very sneaky,"
                "\nand when trying to make some small movements, the character"
                "\ninstantaneously turns 180 degrees out of absolutely nowhere?"
                "\nThis removes at least one reason for this nonsense."
            );
        }
        ImGui::DrawCheckboxForHack(automaticallyReloadWeaponsWhenRefillAllInShops, "Automatically reload weapons when using \"Refill All\" in shops");
        DrawHoodControls();
        ImGui::DrawCheckboxForHack(noWaitForUnsafeEject, "No waiting period for unsafe wall eject");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "In places where you are able to perform an unsafe backeject from a wall"
                "\nthe game normally waits for 1 second after you stop moving"
                "\nbefore letting you do so. This removes that waiting period."
            );
        }
        ImGui::DrawCheckboxForHack(allowCustomizeEquipmentOnLedges, "Allow to open equipment customization when sitting on ledges");
        ImGui::DrawCheckboxForHack(guillotineGunRange, "Slightly greater Guillotine Gun quickshot range");
        //ImGui::DrawCheckboxForHack(preventAutomaticReequipPistol, "Don't automatically reequip the pistol after switching from a rifle");
        //if (ImGui::IsItemHovered())
        //{
        //    ImGui::SetTooltip(
        //        "If you want to play without the pistol equipped,"
        //        "this is not as good as an \"Unequip\" button, but it's something."
        //    );
        //}
        ImGui::DrawCheckboxForHack(allowUnequipPistol, "Allow to unequip pistol");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Go to Pause Menu -> Character Customization -> Gear Loadout -> Weapons -> Pistols."
                "\nSelect the currently equipped pistol (highlighted yellow), then press Enter/Equip"
                "\nto unequip it."
            );
        }
        ImGui::DrawCheckboxForHack(lookbehindButton, "Lookbehind button");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Flip camera around when a hotkey is pressed.\n"
                "Warning: doesn't work when aiming Guillotine Gun."
            );
        }
        if (lookbehindButton.IsActive())
        {
            ImGuiCTX::Indent _ind;
            ImGui::DrawEnumPicker("Lookbehind hotkey", g_Config.hacks->lookbehindButton->hotkey.get(), ImGuiComboFlags_HeightLarge);
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
                ImGui::DrawCheckboxForHack(moreConsistentAimBombFromBehindCover, "Slightly more permissive Aim Bombs out of cover");
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip(
                        "You know how sometimes you try to throw a bomb around the corner,"
                        "\nbut the character instead steps away from the wall, because you're not hugging"
                        "\nthe _very_ edge of cover? Then you end up aiming sort of _at_ the wall instead of"
                        "\naround the corner? This should make these situations less likely."
                        "\nCurrently, however, there are places where this makes things worse."
                    );
                }
            }
        }
        ImGui::Separator();
        ImGui::Text("Cheats");
        ImGui::Separator();
        DrawCheatsControls();
        ImGui::Separator();
        if (ImGuiCTX::TreeNode _{ "Personal requests" })
        {
            DrawPersonalRequestsControls();
        }
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
    void DrawPersonalRequestsControls()
    {
        ImGui::DrawCheckboxForHack(freezeFOV, "Freeze global FOV");
        if (freezeFOV.IsActive())
        {
            DrawFreezeFOVControls();
        }
        ImGui::DrawCheckboxForHack(parkourHelpers, "Parkour helpers");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "I've been asked to make it easier to perform some of the flashy parkour moves."
                "\nThese features aren't _fixes_ and are not for everyone but they can be fun."
            );
        }
        if (parkourHelpers.IsActive())
        {
            ImGuiCTX::Indent _ind;
            ImGui::Checkbox("Spinning descent helper", &g_Config.personalRequests->parkourHelper->spinningDescentHelper.get());
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "TL;DR: The Spinning Descent move is much more likely to trigger if you do the following:"
                    "\nWhile swinging on a horizontal bar, hold Sprint+Forward+ParkourDown."
                    "\nDetails:"
                    "\nThere is a very flashy, very rare parkour animation that involves Arno"
                    "\nmaking a 270 degrees spin in the air before grabbing a ledge."
                    "\nIt is shown in the AC Unity E3 gameplay trailer, and the easiest place"
                    "\nto trigger this move is to start swinging on the horizontal gargoyles on the sides"
                    "\nof the rear half of Notre Dame, and while the swing is performed, hold Sprint+Forward+ParkourDown."
                    "\nNormally, the conditions for this animation to be used are as follows:"
                    "\n- Player needs to be parkouring from a Swing (a horizontal bar, a clothesline,"
                    "\n  ropes hanging between two buildings etc.) to a Wall Hang"
                    "\n  (both feet planted on the wall, not dangling freely)."
                    "\n- Importantly, the wall the player needs to grab must be roughly perpendicular"
                    "\n  to the bar that the player is Swinging from."
                    "\n- The direction from the Swing to Target needs to be rather sharply downward."
                    "\n- Finally, no other \"Parkour Action\" needs to be chosen by the parkour system."
                    "\nThe last point is probably why the move is so rare: there are usually"
                    "\nmore fitting locations to land on when using the ParkourDown."
                    "\nThis patch makes it so that the spinning descent is _heavily_ prioritized when using"
                    "\nthe ParkourDown while Swinging - all other \"parkour actions\" will be ignored"
                    "\nif this one can be used."
                    "\nThis doesn't mean that you can do this on _every_ Swing, and sometimes the move will look"
                    "\nout of place."
                    "\nPersonally, I think this makes the move too frequent, but if you're making one of those"
                    "\nwonderful parkour videos, you might find this useful."
                    "\nLike everything else, this was not tested in multiplayer."
                );
            }
            ImGui::Checkbox("Dive helper", &g_Config.personalRequests->parkourHelper->diveHelper->isActive.get());
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(
                    "When holding down a new hotkey (default Mouse5), the dive move takes an even higher priority"
                    "\nthan the Spindescent (the \"dive\" or the \"basejump\" is a kind of HighProfile jump"
                    "\nthat transitions you to a state where you're holding onto"
                    "\na swing/a horizontal bar/some hanging ropes/an overhanging roof"
                    "\nwith both hands and your feet dangle freely)."
                    "\nYour other input still matters. If the bar you're targeting is close and below,"
                    "\nyou'll need to press Sprint+ParkourDown+Forward+DiveHotkey."
                    "\nIf it's close and slightly higher than yourself or below but further away,"
                    "\nyou'll need to press Sprint+ParkourUp+Forward+DiveHotkey."
                    "\nPretty much as in unmodded game, you need to be able to judge the distance"
                    "\nand the appropriate \"parkour mode\", but you can now prioritize the dive."
                    "\nI'm not super happy with the new hotkey, but you can come to rely on it,"
                    "\nand this move has been requested twice."
                );
            }
            if (g_Config.personalRequests->parkourHelper->diveHelper->isActive.get())
            {
                ImGuiCTX::Indent _ind2;
                ImGui::DrawEnumPicker("Dive helper hotkey", g_Config.personalRequests->parkourHelper->diveHelper->hotkey.get(), ImGuiComboFlags_HeightLarge);
            }
            ImGui::Checkbox("Can rise on ledge immediately after Ledge Assassination", &g_Config.personalRequests->parkourHelper->canRiseOnLedgeAfterLedgeAssassination.get());
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
        aimBombsFromPeaks.Toggle(hacksSection->aimBombsFromPeaks);
        noMoreImaginaryBombThrows.Toggle(hacksSection->noMoreImaginaryBombThrows);
        dontRandomlyTurn180degrees.Toggle(hacksSection->dontRandomlyTurn180degrees);
        moreSituationsToDropBombs.Toggle(hacksSection->moreSituationsToDropBombs->isActive);
        moreReliableQuickshot.Toggle(hacksSection->moreReliableQuickshot->isActive);
        automaticallyReloadWeaponsWhenRefillAllInShops.Toggle(hacksSection->automaticallyReloadWeaponsWhenRefillAllInShops);
        lookbehindButton.Toggle(hacksSection->lookbehindButton->isActive);
        noWaitForUnsafeEject.Toggle(hacksSection->noWaitForUnsafeEject);
        allowCustomizeEquipmentOnLedges.Toggle(hacksSection->allowCustomizeEquipmentOnLedges);
        guillotineGunRange.Toggle(hacksSection->slightlyGreaterGuillotineGunQuickshotRange);
        allowUnequipPistol.Toggle(hacksSection->allowUnequipPistol);

        auto& cheatsSection = cfg.cheats;
        dontDecreaseRemainingAmmo.Toggle(cheatsSection->infiniteAmmo);

        auto& requestsSection = cfg.personalRequests;
        parkourHelpers.Toggle(requestsSection->parkourHelper->isActive);
        freezeFOV.Toggle(requestsSection->freezeFOV->isActive);
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
        hacksSection->aimBombsFromPeaks = aimBombsFromPeaks.IsActive();
        hacksSection->noMoreImaginaryBombThrows = noMoreImaginaryBombThrows.IsActive();
        hacksSection->dontRandomlyTurn180degrees = dontRandomlyTurn180degrees.IsActive();
        hacksSection->moreSituationsToDropBombs->isActive = moreSituationsToDropBombs.IsActive();
        hacksSection->moreReliableQuickshot->isActive = moreReliableQuickshot.IsActive();
        hacksSection->automaticallyReloadWeaponsWhenRefillAllInShops = automaticallyReloadWeaponsWhenRefillAllInShops.IsActive();
        hacksSection->lookbehindButton->isActive = lookbehindButton.IsActive();
        hacksSection->noWaitForUnsafeEject = noWaitForUnsafeEject.IsActive();
        hacksSection->allowCustomizeEquipmentOnLedges = allowCustomizeEquipmentOnLedges.IsActive();
        hacksSection->slightlyGreaterGuillotineGunQuickshotRange = guillotineGunRange.IsActive();
        hacksSection->allowUnequipPistol = allowUnequipPistol.IsActive();

        auto& cheatsSection = cfg.cheats;
        cheatsSection->infiniteAmmo = dontDecreaseRemainingAmmo.IsActive();

        auto& requestsSection = cfg.personalRequests;
        requestsSection->parkourHelper->isActive = parkourHelpers.IsActive();
        requestsSection->freezeFOV->isActive = freezeFOV.IsActive();
    }
    AutoAssembleWrapper<BatlampOfFrancide> batlampOfFranciade;
    AutoAssembleWrapper<AmmoCheat> dontDecreaseRemainingAmmo;
    AutoAssembleWrapper<PretendYoureInFranciade> pretendYoureInFranciade;
    AutoAssembleWrapper<DisableDisguiseCooldown> disableDisguiseCooldown;
    AutoAssembleWrapper<UnbreakableDisguise> unbreakableDisguise;
    AutoAssembleWrapper<DisguiseDoesntMakeYouInvisible> disguiseDoesntMakeYouInvisible;

    AutoAssembleWrapper<ParkourActionsExtraProcessing> parkourHelpers;
    AutoAssembleWrapper<FreezeFOV> freezeFOV;
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
    g_MyHacks->ReadConfig(g_Config);
    g_MyHacks->WriteConfig(g_Config);
    MainConfig::WriteToFile();
}
