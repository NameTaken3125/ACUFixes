#include "pch.h"

#include "ImGuiCTX.h"

#include "AnimationTools/SkeletonDumper.h"

#include "ACU/AtomAnimComponent.h"
#include "ACU/SkeletonComponent.h"
#include "ACU/Entity.h"
#include "ACU/ACUGetSingletons.h"

#include "AnimationTools/MyAnimationPlayer.h"




void ExportPlayerSkeletonAsJSON();
void DrawSkeletonExperimentsControls()
{
    if (ImGui::Button("Dump player skeletons/bones tree"))
    {
        if (SkeletonComponent* skelCpnt = ACU::GetPlayerCpnt_SkeletonComponent())
        {
            SkelDumper().DumpSkeletons(*skelCpnt);
        }
    }
    if (ImGui::Button("Export player skeleton into Blender as JSON"))
    {
        ExportPlayerSkeletonAsJSON();
    }
}

constexpr uint64 handle_skeleton_BaseMale = 28540328525; // = > NEW_SDN_CN_U_ArmorSet_02/ACU_BaseSkeleton_Male.Skeleton
constexpr uint64 handle_skeleton_HiddenBlade = 87332257962; // = > CN_P_LegacyAvatar_ThomasCarneillon_Base/ACU_U_Arno_IconicWeapon.Skeleton
void DrawAnimationEditor();
void DrawAtomGraphDumper();
void DrawAnimationExperiments()
{
    if (ImGuiCTX::TabBar _tabbar{ "animtoolsTB" })
    {
        if (ImGuiCTX::Tab _mainTab{ "Animplayer" })
        {
            if (ImGuiCTX::WindowChild _{ "animtoolsmaintabchild" })
            {
                //g_SkeletonsVisualizer.DrawControls();
                //g_ManualAnimationPlayer.DrawControls();
                g_MyAnimationPlayer.DrawControls();
            }
        }
        if (ImGuiCTX::Tab _mainTab{ "Animedit" })
        {
            if (ImGuiCTX::WindowChild _{ "animeditchild" })
            {
                DrawAnimationEditor();
            }
        }
        if (ImGuiCTX::Tab _mainTab{ "Skeletons" })
        {
            DrawSkeletonExperimentsControls();
        }
        if (ImGuiCTX::Tab _mainTab{ "AtomGraph" })
        {
            DrawAtomGraphDumper();
        }
    }
}
