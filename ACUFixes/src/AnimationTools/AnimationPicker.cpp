#include "pch.h"

#include "AnimationPicker.h"
int g_currentAnimIdx = 0;
std::vector<MyPlayableAnim> g_myAnimations = {
    {83792228775, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_Bomb_Freeaim_Throw_Deny.Animation"},
    {40835186528, "DataAI_GroupBehavior_Couple_TakeAway/xx_Fight_Bomb_Freeaim_Throw_Drop.Animation"},

    {127014661718, "DataAI_GroupBehavior_Couple_TakeAway/xx_FreeAim_Reflexshot_Reload.Animation"},
    {75882037750, "DataAI_GroupBehavior_Couple_TakeAway/xx_Reflexshot_Pistol_UpperBody_Middle_Front.Animation"},
    {75107198058, "DataAI_GroupBehavior_Couple_TakeAway/xx_ReflexShot_Pistol_UpperBody_Middle_LeftBack.Animation"},

    {95034111564, "DataAI_GroupBehavior_Couple_TakeAway/xx_Assassinate_GroundHigh_HBladeKill_Fd300cm_P1_Wall_Atk.Animation"},
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

    {31096122263, "Game Bootstrap Settings/xx_Invalid_Animation.Animation"},
};

bool AnimationPicker::Draw(const char* label, ACUSharedPtr_Strong<Animation>& inOut)

{
    ImGuiCTX::PushID _id(this);
    if (ImGui::Button(label))
    {
        ImGui::OpenPopup("Animpicker");
    }
    uint64 editedAnimHandle = inOut.GetSharedBlock().handle;
    bool isChanged = false;
    if (ImGuiCTX::Popup _animpicker{ "Animpicker" })
    {
        ImGui::Text("Input handle:"); ImGui::SameLine();
        isChanged |= ImGui::InputUInt64("##manInput", &editedAnimHandle);
        ImGui::Text("Or pick from collection:");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##fromCollection", nullptr))
        {
            for (size_t i = 0; i < g_myAnimations.size(); i++)
            {
                const MyPlayableAnim& anim = g_myAnimations[i];
                if (ImGui::Selectable(anim.name, anim.handle == editedAnimHandle))
                {
                    editedAnimHandle = anim.handle;
                    isChanged = true;
                }
            }
            ImGui::EndCombo();
        }
    }
    ACUSharedPtr_Strong<ManagedObject> foundSharedBlock(editedAnimHandle);
    if (ManagedObject* maybeAnim = foundSharedBlock.GetPtr())
    {
        uint64 vtbl = *(uint64*)maybeAnim;
        if (vtbl == Animation::GetVTBL())
        {
            Animation& anim = *static_cast<Animation*>(maybeAnim);
            ImGui::Text("%s\nDuration: %f", ACU::Handles::HandleToText(foundSharedBlock.GetSharedBlock().handle).c_str(), anim.Length);
            inOut = std::move(reinterpret_cast<ACUSharedPtr_Strong<Animation>&>(foundSharedBlock));
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
    return isChanged;
}
