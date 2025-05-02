#include "pch.h"

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"

#include "ImGuiConfigUtils.h"

#include "ParkourDebugging/ParkourDebuggingPatch.h"
#include "ParkourDebugging/LoggingTheHumanStates.h"
#include "ParkourDebugging/Hack_PickEntityToShoot.h"
#include "AnimationTools/AnimationsLog/AnimationGraphEvaluationLog.h"


class MyHacksContainer_DevExtras
{
public:
    AutoAssembleWrapper<ParkourDebuggingPatch> parkourDebugging;
    AutoAssembleWrapper<EnterHumanStateHook> humanStatesHook;
    AutoAssembleWrapper<AnimGraphEvaluationLogging> animGraphDebugging;
    AutoAssembleWrapper<PickEntityToShoot> pickEntityToShoot;

    void DrawControls()
    {
        ImGui::DrawCheckboxForHack(animGraphDebugging, "Animation graph debugging");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Whenever the player character starts playing an Animation,\n"
                "a log entry is added to the ImGui console with the address\n"
                "of the corresponding AtomAnimationDataBaseNode, address of Animation, the Animation's handle\n"
                "and filename (if the Dictionary of Handles is loaded\n"
                "and the Handle is recognized - see the Handles tab for more info).\n"
                "This can help when trying to figure out when and how the animation state transitions happen.\n"
                "Animations can be referenced multiple times within the animation graph,\n"
                "but if you use the \"Dump animation graph to text file\" feature in the same game session,\n"
                "and search for the address of AtomAnimationDataBaseNode,\n"
                "you can see exactly which state nodes are involved."
            );
        }
        ImGui::Separator();
        ImGui::DrawCheckboxForHack(humanStatesHook, "Log player's \"HumanStates\"");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Log the tree of the player's \"HumanStates\".\n"
                "Different states contain different callbacks and these\n"
                "held the keys to making many of the features of ACUFixes possible.\n"
                "I think observing how the tree of these states changes in response\n"
                "to player's actions in real time can be insightful."
            );
        }
        if (humanStatesHook.IsActive())
        {
            ImGui::SameLine();
            DrawHumanStatesDebugControls();
        }
        ImGui::Separator();
        ImGui::DrawCheckboxForHack(parkourDebugging, "Parkour Debugging");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Logs some information into ImGui console about which recognized\n"
                "\"Parkour Actions\" were selected.\n"
                "If 3D Markers feature is enabled, also visualizes the target locations\n"
                "for the actions that were considered."
            );
        }
        ImGui::Separator();
        ImGui::DrawCheckboxForHack(pickEntityToShoot, "Manually select quickshot target");
        if (pickEntityToShoot.IsActive())
        {
            RaycastPicker_PickRangedWeaponTarget();
        }
    }
};
std::optional<MyHacksContainer_DevExtras> g_MyHacks_DevExtras;

void DrawHacksControls_DevExtras()
{
    if (g_MyHacks_DevExtras)
    {
        g_MyHacks_DevExtras->DrawControls();
    }
}

void MyVariousHacks_DevExtras_Start()
{
    g_MyHacks_DevExtras.emplace();
}
