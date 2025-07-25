#include "pch.h"

#include "AnimationPicker.h"
int g_currentAnimIdx = 0;
std::vector<MyPlayableAnim> g_myAnimations = {
    {0x0000000FDA0491DD, "xx_Parkour_Run_PreWallEntryFront_Fd050cm.Animation"},
    {0x0000000FDA0491E8, "xx_Parkour_Run_PreWallEntryFront_Fd100cm.Animation"},
    {0x0000000FDA0491F6, "xx_Parkour_Run_PreWallEntryFront_Fd150cm.Animation"},
    {0x0000000FDA049201, "xx_Parkour_Run_PreWallEntryFront_Fd170cm.Animation"},
    {0x0000000FDA049206, "xx_Parkour_Run_PreWallEntryFront_Fd200cm.Animation"},

    // After initiating failed wallrun, then sidehop right:
    {0x146A9AC136, "xx_Nav_IdleHigh_PreWallEntry_Fd0Deg040cm.Animation"},
    {0xDA2D2DC7E, "xx_Nav_IdleHigh_PreWallEntry_Fd0Deg100cm.Animation"},
    {0xFDA0491DD, "xx_Parkour_Run_PreWallEntryFront_Fd050cm.Animation"},
    {0x1081724057, "xx_Parkour_WallEntry_Fail_Fd0Deg.Animation"},
    {0x785672016, "xx_ScrambleFrontFootl_55deg_tr_GroundEntryFootr.Animation"},
    {0x2082FC8577, "xx_Parkour_ClimbPassExitRight_Fd400cm-Up100cm.Animation"},
    // After initiating targeted wallrun:
    {0x296F0C0C1, "xx_Climbing_Climb_Idle.Animation"},
    {0x146A9AC136, "xx_Nav_IdleHigh_PreWallEntry_Fd0Deg040cm.Animation"},
    {0xDA2D2DC7E, "xx_Nav_IdleHigh_PreWallEntry_Fd0Deg100cm.Animation"},
    {0xFDA0491DD, "xx_Parkour_Run_PreWallEntryFront_Fd050cm.Animation"},
    {0xFDA0491C0, "xx_Parkour_WallEntryFwd_tr_Climb_Up325cm.Animation"},
    {0xFFAC7D0D4, "xx_Parkour_WallEntryFwd30Deg_tr_Climb_Rt125cm-Up325cm.Animation"},

    {83792228775, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_Bomb_Freeaim_Throw_Deny.Animation"},
    {40835186528, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_Bomb_Freeaim_Throw_Drop.Animation"},

    {127014661718, "DataAI_GroupBehavior_Couple_TakeAway/xx_FreeAim_Reflexshot_Reload.Animation"},
    {75882037750, "DataAI_GroupBehavior_Couple_TakeAway/xx_Reflexshot_Pistol_UpperBody_Middle_Front.Animation"},
    {75107198058, "DataAI_GroupBehavior_Couple_TakeAway/xx_ReflexShot_Pistol_UpperBody_Middle_LeftBack.Animation"},

    {95034111564, "DataAI_GroupBehavior_Couple_TakeAway/xx_Assassinate_GroundHigh_HBladeKill_Fd300cm_P1_Wall_Atk.Animation"},
    {95034111566, "DataAI_GroupBehavior_Couple_TakeAway/xx_Assassinate_GroundHigh_HBladeKill_Fd300cm_P1_Wall_Dfn.Animation"},
    {81260373316, "DataAI_GroupBehavior_Couple_TakeAway/xx_Assassinate_Ground_Lo_Front_1m_01_Atk.Animation"},
    {81844209515, "DataAI_GroupBehavior_Couple_TakeAway/xx_Assassinate_Ground_Lo_Front_1m_02_Atk.Animation"},
    {99861340708, "DataAI_GroupBehavior_Couple_TakeAway/xx_Assassinate_Ground_Lo_Front_1m_03_Atk.Animation"},
    {32322693439, "DataAI_GroupBehavior_Couple_TakeAway/xx_Assassinate_GroundHigh_HBladeKill_Fd100cm_P1_Atk.Animation"},
    {81410192477, "DataAI_GroupBehavior_Couple_TakeAway/xx_Assassinate_GroundHigh_HBladeKill_Fd100cm_P1_Atk_V05.Animation"},

    {26166083397, "DataAI_GroupBehavior_Couple_TakeAway/xx_Parkour_FallCycle.Animation"},
    {139636540715, "DataAI_GroupBehavior_Couple_TakeAway/xx_Parkour_ClimbPassExitRight_Fd300cm - Up100cm.Animation"},
    {27178795240, "DataAI_GroupBehavior_Couple_TakeAway/xx_Parkour_LandingFwd_LandRoll_tr_Run_Impultion.Animation"},

    {140551329342, "DataAI_GroupBehavior_Couple_TakeAway/xx_Rift_Reaction_Wind_Idle_L_02.Animation"},
    {140551329340, "DataAI_GroupBehavior_Couple_TakeAway/xx_Rift_Reaction_Wind_Idle_R_02.Animation"},

    {106205824533, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_Sword_Roll_Back.Animation"},
    {34660455056, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_Sword_Flourish_Lo.Animation"},

    {233400564761, "DataBlock_SQ04_M02_MOD2/ARNAUDC_Body_SQ04_M020_SC077C_Streamed.Animation"},

    {87707686893, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_ThrowBlade_Action_OpenedPose.Animation"},
    {87707686889, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_ThrowBlade_Action_Fire.Animation"},
    {96422756980, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_ThrowBlade_Action_Reload.Animation"},
    {87707686886, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_ThrowBlade_Action_Open.Animation"},
    {87707686891, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_ThrowBlade_Action_Close.Animation"},


    {223334587827, "DataAI_GroupBehavior_Couple_TakeAway/xx_FreeAim_Pistol_Fire_03.Animation"},
    {223334587825, "DataAI_GroupBehavior_Couple_TakeAway/xx_FreeAim_Pistol_Fire_02.Animation"},
    {223334587823, "DataAI_GroupBehavior_Couple_TakeAway/xx_FreeAim_Pistol_Fire_01.Animation"},
    {84093474035, "DataAI_GroupBehavior_Couple_TakeAway/xx_FreeAim_ThrowBlade_Fire_Fd0.Animation"},
    {151418108049, "DataAI_GroupBehavior_Couple_TakeAway/xx_FreeAim_Pistol_Chambering.Animation"},

    {31096122263, "Game Bootstrap Settings/xx_Invalid_Animation.Animation"},
    {195022897882, "xx_HiddenBlade_MeduIdle/xx_HiddenBlade_MeduIdle.Animation"},
    {2999967198, "Game Bootstrap Settings/Ghost_mode_skiprope_sideways.Animation"},
    {2999967798, "Game Bootstrap Settings/Ghost_mode_skiprope_cross_start.Animation"},
    {2999967803, "Game Bootstrap Settings/Ghost_mode_skiprope_cross_cylce.Animation"},
    {2999967806, "Game Bootstrap Settings/Ghost_mode_skiprope_cross_end.Animation"},
    {2999967104, "Game Bootstrap Settings/Ghost_mode_skiprope_doublejump.Animation"},
    {2192390943, "Game Bootstrap Settings/Ghost_mode_skiprope_swing_sides.Animation"},
    {2192390948, "Game Bootstrap Settings/Ghost_mode_skiprope_walk.Animation"},
    {2999965819, "Game Bootstrap Settings/Ghost_mode_skiprope_cycle_straight.Animation"},
    {15776838, "Game Bootstrap Settings/Ghost_mode01.Animation"},
    {15776578, "Game Bootstrap Settings/Ghost_mode02.Animation"},
    {79351389917, "Game Bootstrap Settings/ACU_CrowdWalla_Happy.Animation"},
    {31011512729, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd100cm-Up150cm.Animation"},
    {31011512737, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd300cm-Up150cm.Animation"},
    {31011512745, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd600cm-Up150cm.Animation"},
    {31011512725, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd100cm-Dn000cm.Animation"},
    {31011512733, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd300cm-Dn000cm.Animation"},
    {31011512741, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd600cm-Dn000cm.Animation"},
    {31011512727, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd100cm-Dn300cm.Animation"},
    {31011512735, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd300cm-Dn300cm.Animation"},
    {31011512743, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd600cm-Dn300cm.Animation"},
    {31011512731, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd100cm-Up300cm.Animation"},
    {31011512739, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd300cm-Up300cm.Animation"},
    {31011512747, "DataAI_GroupBehavior_Couple_TakeAway/xx_Climbing_Climb_tr_WallEjectSwing_Lt90Deg-Fd600cm-Up300cm.Animation"},
};

bool AnimationPicker::Draw(const char* label, ACU::StrongRef<Animation>& inOut)

{
    ImGuiCTX::PushID _id(this);
    if (ImGui::Button(label))
    {
        ImGui::OpenPopup("Animpicker");
    }
    uint64 editedAnimHandle = inOut.GetSharedBlock().handle;
    bool isChanged = false;
    bool isNewValidPick = false;
    if (ImGuiCTX::Popup _animpicker{ "Animpicker" })
    {
        ImGui::Text("Input handle:"); ImGui::SameLine();
        isChanged |= ImGui::InputUInt64("##manInput", &editedAnimHandle);
        ImGui::Text("Or pick from collection:");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##fromCollection", nullptr, ImGuiComboFlags_HeightLargest))
        {
            ImGuiTextBuffer buf;
            for (size_t i = 0; i < g_myAnimations.size(); i++)
            {
                const MyPlayableAnim& anim = g_myAnimations[i];
                buf.clear();
                buf.appendf("%s##%d", anim.name, i);
                if (ImGui::Selectable(buf.c_str(), anim.handle == editedAnimHandle))
                {
                    editedAnimHandle = anim.handle;
                    isChanged = true;
                }
            }
            ImGui::EndCombo();
        }
    }
    ACU::StrongRef<ManagedObject> foundSharedBlock(editedAnimHandle);
    if (ManagedObject* maybeAnim = foundSharedBlock.GetPtr())
    {
        uint64 vtbl = *(uint64*)maybeAnim;
        if (vtbl == Animation::GetVTBL())
        {
            if (isChanged)
                isNewValidPick = true;
            Animation& anim = *static_cast<Animation*>(maybeAnim);
            ImGui::Text(
                "%s"
                , ACU::Handles::HandleToText(foundSharedBlock.GetSharedBlock().handle)
            );
            ImGui::Text(
                "Addr: %llX"
                , foundSharedBlock.GetPtr()
            );
            if (ImGui::IsItemClicked(0))
            {
                ImGuiTextBuffer buf;
                buf.appendf("%llX", foundSharedBlock.GetPtr());
                ImGui::SetClipboardText(buf.c_str());
            }
            ImGui::SameLine();
            ImGui::Text(
                "SharedBlock: %llX"
                , &foundSharedBlock.GetSharedBlock()
            );
            if (ImGui::IsItemClicked(0))
            {
                ImGuiTextBuffer buf;
                buf.appendf("%llX", &foundSharedBlock.GetSharedBlock());
                ImGui::SetClipboardText(buf.c_str());
            }
            ImGui::Text(
                "Duration: %f\nAnimationKey: 0x%lX"
                , anim.Length
                , anim.AnimationKey
            );
            inOut = std::move(reinterpret_cast<ACU::StrongRef<Animation>&>(foundSharedBlock));
        }
        else
        {
            ImGui::Text("Not an Animation.");
        }
    }
    else
    {
        ImGui::Text("Object doesn't exist.");
    }
    return isNewValidPick;
}
