#pragma once

#include "ACU/SkeletonComponent.h"
#include "ACU/Skeleton.h"
#include "ACU/Bone.h"

#include "Handles.h"

#include "SimpleJSON/json.hpp"

class SkelDumper
{
public:
    class Indent
    {
    public:
        Indent(SkelDumper& parent) : parent(parent) { parent.m_currentIndent += "  "; }
        ~Indent() { parent.m_currentIndent = parent.m_currentIndent.substr(2); }
    private:
        SkelDumper& parent;
    };
    std::string m_currentIndent;
    Indent IndentScoped() { return Indent{ *this }; }
public:

    void DumpBoneSubtree(Skeleton& skel, Bone& parent)
    {
        auto _ind = IndentScoped();
        for (uint16 i = 0; i < skel.Bones.size; i++)
        {
            Bone* child_mb = skel.Bones[i];
            if (child_mb->FatherBone != &parent)
            {
                continue;
            }
            Bone& bone = *child_mb;
            ImGui::LogText(
                "%s%d. BoneID: %10u; GlobalPosition: [%f, %f, %f]; LocalPosition: [%f, %f, %f]; GlobalRotation: [%f, %f, %f, %f]; LocalRotation: [%f, %f, %f, %f]\n"
                , m_currentIndent.c_str()
                , i
                , bone.BoneID_boneNameHash
                , bone.GlobalPosition.x
                , bone.GlobalPosition.y
                , bone.GlobalPosition.z
                , bone.LocalPosition.x
                , bone.LocalPosition.y
                , bone.LocalPosition.z
                , bone.GlobalRotation.x
                , bone.GlobalRotation.y
                , bone.GlobalRotation.z
                , bone.GlobalRotation.w
                , bone.LocalRotation.x
                , bone.LocalRotation.y
                , bone.LocalRotation.z
                , bone.LocalRotation.w
            );
            DumpBoneSubtree(skel, *child_mb);
        }
    }
    void DumpSkeletonAsTree(Skeleton& skel)
    {
        auto _ind = IndentScoped();
        Bone& bone = *skel.RootBone;
        ImGui::LogText(
            "%s%d. BoneID: %10u; GlobalPosition: [%f, %f, %f]; LocalPosition: [%f, %f, %f]; GlobalRotation: [%f, %f, %f, %f]; LocalRotation: [%f, %f, %f, %f]\n"
            , m_currentIndent.c_str()
            , 0
            , bone.BoneID_boneNameHash
            , bone.GlobalPosition.x
            , bone.GlobalPosition.y
            , bone.GlobalPosition.z
            , bone.LocalPosition.x
            , bone.LocalPosition.y
            , bone.LocalPosition.z
            , bone.GlobalRotation.x
            , bone.GlobalRotation.y
            , bone.GlobalRotation.z
            , bone.GlobalRotation.w
            , bone.LocalRotation.x
            , bone.LocalRotation.y
            , bone.LocalRotation.z
            , bone.LocalRotation.w
        );
        DumpBoneSubtree(skel, *skel.RootBone);
    }
    void DumpSkeletonPlain(Skeleton& skel)
    {
        auto _ind = IndentScoped();
        for (uint16 i = 0; i < skel.Bones.size; i++)
        {
            Bone& bone = *skel.Bones[i];
            ImGui::LogText(
                "%s%d. BoneID: %10u; GlobalPosition: [%f, %f, %f]; LocalPosition: [%f, %f, %f]; GlobalRotation: [%f, %f, %f, %f]; LocalRotation: [%f, %f, %f, %f]\n"
                , m_currentIndent.c_str()
                , i
                , bone.BoneID_boneNameHash
                , bone.GlobalPosition.x
                , bone.GlobalPosition.y
                , bone.GlobalPosition.z
                , bone.LocalPosition.x
                , bone.LocalPosition.y
                , bone.LocalPosition.z
                , bone.GlobalRotation.x
                , bone.GlobalRotation.y
                , bone.GlobalRotation.z
                , bone.GlobalRotation.w
                , bone.LocalRotation.x
                , bone.LocalRotation.y
                , bone.LocalRotation.z
                , bone.LocalRotation.w
            );
        }
    }
    void DumpSkeleton(Skeleton& skel) { DumpSkeletonAsTree(skel); }
    void DumpSkeletons(SkeletonComponent& skelCpnt)
    {
        ImGui::LogToClipboard();
        ImGui::LogText("MainSkeleton:\n");
        {
            auto _ind = IndentScoped();
            ImGui::LogText(
                "%s%llu => %s:\n"
                , m_currentIndent.c_str()
                , skelCpnt.MainSkeleton->handle
                , ACU::Handles::HandleToText(skelCpnt.MainSkeleton->handle));
            if (Skeleton* mainSkel = skelCpnt.MainSkeleton->GetPtr())
            {
                DumpSkeleton(*mainSkel);
            }
        }
        ImGui::LogText("AddonSkeletons:\n");
        {
            auto _ind = IndentScoped();
            for (SharedPtrNew<Skeleton>* sharedSkel : skelCpnt.AddonSkeletons)
            {
                ImGui::LogText(
                    "%s%llu => %s:\n"
                    , m_currentIndent.c_str()
                    , sharedSkel->handle
                    , ACU::Handles::HandleToText(sharedSkel->handle));
                if (Skeleton* skel = sharedSkel->GetPtr())
                {
                    DumpSkeleton(*skel);
                }
            }
        }
        ImGui::LogFinish();
    }

};

json::JSON Vector4ftoJSON(const Vector4f& v)
{
    json::JSON arr = json::Array(v.x, v.y, v.z, v.w);
    return arr;
}
json::JSON BoneToJSON(Bone& bone)
{
    json::JSON obj = json::Object();
    auto& GlobalPosition = bone.GlobalPosition;
    auto& GlobalRotation = bone.GlobalRotation;
    auto& LocalPosition = bone.LocalPosition;
    auto& LocalRotation = bone.LocalRotation;
    obj["Index"] = bone.Index;
    obj["BoneID"] = std::to_string(bone.BoneID_boneNameHash);
    obj["parentBoneID"] = json::JSON();
    if (bone.FatherBone)
    {
        obj["parentBoneID"] = std::to_string(bone.FatherBone->BoneID_boneNameHash);
    }
    obj["GlobalPosition"] = Vector4ftoJSON(GlobalPosition);
    obj["GlobalRotation"] = Vector4ftoJSON(GlobalRotation);
    obj["LocalPosition"] = Vector4ftoJSON(LocalPosition);
    obj["LocalRotation"] = Vector4ftoJSON(LocalRotation);
    return obj;
}
json::JSON SkeletonToJSON(Skeleton& skel)
{
    json::JSON obj = json::Object();
    obj["RootBoneDefaultLocalPosition"] = Vector4ftoJSON(skel.RootBoneDefaultLocalPosition);
    obj["RootBoneDefaultLocalRotation"] = Vector4ftoJSON(skel.RootBoneDefaultLocalRotation);
    json::JSON& bonesArr = obj["Bones"] = json::Array();
    for (Bone* bone : skel.Bones)
    {
        json::JSON boneAsJSON = BoneToJSON(*bone);
        bonesArr.append(boneAsJSON);
    }
    return obj;
}

#include "ACU/ManagedPtrs/ManagedPtrs.h"

void ExportPlayerSkeletonAsJSON()
{
    constexpr uint64 handle_skeleton_BaseMale = 28540328525; // = > NEW_SDN_CN_U_ArmorSet_02/ACU_BaseSkeleton_Male.Skeleton
    ACU::StrongRef<Skeleton> sharedSkel_baseMale(handle_skeleton_BaseMale);
    if (Skeleton* skel = sharedSkel_baseMale.GetPtr())
    {
        ImGui::LogToClipboard();
        ImGui::LogText(SkeletonToJSON(*skel).dump().c_str());
        ImGui::LogFinish();
    }
}
