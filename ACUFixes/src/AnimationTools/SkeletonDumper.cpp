#include "pch.h"

#include "ACU/SkeletonComponent.h"
#include "ACU/Skeleton.h"
#include "ACU/Bone.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"
#include "ACU/ACUGetSingletons.h"

#include "Handles.h"
#include "ImGuiCTX.h"
#include "SimpleJSON/json.hpp"
#include "Raycasting/RaycastPicker.h"

#include "SkeletonDumper.h"


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
                , bone.BoneID
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
            , bone.BoneID
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
                , bone.BoneID
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
    obj["BoneID"] = std::to_string(bone.BoneID);
    obj["parentBoneID"] = json::JSON();
    if (bone.FatherBone)
    {
        obj["parentBoneID"] = std::to_string(bone.FatherBone->BoneID);
    }
    obj["mirrorBoneID"] = json::JSON();
    if (bone.MirrorBone)
    {
        obj["mirrorBoneID"] = std::to_string(bone.MirrorBone->BoneID);
    }
    obj["GlobalPosition"] = Vector4ftoJSON(GlobalPosition);
    obj["GlobalRotation"] = Vector4ftoJSON(GlobalRotation);
    obj["LocalPosition"] = Vector4ftoJSON(LocalPosition);
    obj["LocalRotation"] = Vector4ftoJSON(LocalRotation);
    return obj;
}
json::JSON SkeletonToJSON(SharedPtrNew<Skeleton>& sharedSkel)
{
    json::JSON obj = json::Object();
    uint64 handle = sharedSkel.handle;
    obj["handle"] = std::to_string(handle);
    obj["handleReadable"] = ACU::Handles::HandleToText(handle);
    Skeleton* skel = sharedSkel.GetPtr();
    if (!skel) return obj;
    obj["RootBoneDefaultLocalPosition"] = Vector4ftoJSON(skel->RootBoneDefaultLocalPosition);
    obj["RootBoneDefaultLocalRotation"] = Vector4ftoJSON(skel->RootBoneDefaultLocalRotation);
    json::JSON& bonesArr = obj["Bones"] = json::Array();
    for (Bone* bone : skel->Bones)
    {
        json::JSON boneAsJSON = BoneToJSON(*bone);
        bonesArr.append(boneAsJSON);
    }
    return obj;
}
json::JSON SkeletonsToJSON(SkeletonComponent& skelCpnt)
{
    json::JSON obj = json::Object();
    obj["MainSkeleton"] = SkeletonToJSON(*skelCpnt.MainSkeleton);
    auto& jsAddonSkels = obj["AddonSkeletons"] = json::Array();
    for (SharedPtrNew<Skeleton>* addonSkel : skelCpnt.AddonSkeletons)
    {
        jsAddonSkels.append(SkeletonToJSON(*addonSkel));
    }
    return obj;
}
void ExportSkeletonsAsJSON(SkeletonComponent& skelCpnt)
{
    ImGui::LogToClipboard();
    ImGui::LogText(SkeletonsToJSON(skelCpnt).dump().c_str());
    ImGui::LogFinish();
}
const ImVec4 colorTextRed = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);

constexpr uint64 vtbl_SkeletonComponent = 0x142E76630;
SkeletonComponent* GetEntityCpnt_SkeletonComponent(Entity& entity)
{
    return static_cast<SkeletonComponent*>(entity.FindComponentByVTBL(vtbl_SkeletonComponent));
}
void DrawSkeletonExportControls()
{
    static ACU::WeakRef<Entity> entToDump;
    if (ImGui::Button("Pick player's skeletons"))
    {
        auto SetPlayerAsTarget = [](){
            if (Entity* player = ACU::GetPlayer())
                entToDump = (ACU::WeakRef<Entity>&)player->selfSharedPtr;
            };
        SetPlayerAsTarget();
    }
    ImGui::SameLine();
    ImGui::Text("or");
    ImGui::SameLine();
    {
        static RaycastPickerModal picker;
        picker.Pick(
            "Pick Entity with a SkeletonComponent"
            , [&](const MyRaycastSuccessfulHit& hit) // On confirmed pick
            {
                entToDump = hit.m_Entity;
            }
            , [&](const MyRaycastSuccessfulHit& hit) // On every hit
            {
                Entity* ent = hit.m_Entity.GetPtr();
                static ImGuiTextBuffer buf; buf.resize(0);
                buf.appendf("Entity at %llX, Handle: %llX\n", ent, hit.m_Entity.GetHandle());
                buf.appendf(
                    "[%8.2f,%8.2f,%8.2f]\n"
                    , hit.m_HitLocation.x
                    , hit.m_HitLocation.y
                    , hit.m_HitLocation.z
                );
                auto* skelCpnt = GetEntityCpnt_SkeletonComponent(*ent);
                if (!skelCpnt)
                {
                    buf.append("[-] Has no SkeletonComponent");
                }
                else
                {
                    buf.appendf(
                        "SkeletonComponent: %llX\n"
                        , skelCpnt
                    );
                    SharedPtrNew<Skeleton>* sharedMainSkel = skelCpnt->MainSkeleton;
                    Skeleton* mainSkel = sharedMainSkel->GetPtr();
                    buf.appendf(
                        "MainSkeleton: %llu => %s\n"
                        "%hu AddonSkeletons\n"
                        , sharedMainSkel->handle
                        , ACU::Handles::HandleToText(sharedMainSkel->handle)
                        , skelCpnt->AddonSkeletons.size
                    );
                }
                ImGui::Text(
                    buf.c_str()
                );
                ImGui3D::DrawLocationOnce(hit.m_HitLocation, 0.2f);
                Vector3f entityPos = ent->GetPosition();
                ImGui3D::DrawLocationOnce(entityPos, 0.2f);
                BoundingVolume& bbox = ent->BoundingVolume_;
                Matrix4f bboxScale = Matrix4f::createScale(
                    bbox.Max.x - bbox.Min.x,
                    bbox.Max.y - bbox.Min.y,
                    bbox.Max.z - bbox.Min.z
                );
                Matrix4f bboxPos = Matrix4f::createTranslation(bbox.Min);
                Matrix4f bboxTransform = ent->GetTransform() * bboxPos * bboxScale;
                ImGui3D::DrawWireModelOnce(ImGui3D::GetModel_Cube01(), bboxTransform);
            }
            , RaycastPickerModal::DefaultOnNoHit
        );
        Entity* ent = entToDump.GetSharedBlock().GetPtr();
        if (!ent)
        {
            ImGui::Text("No Entity selected");
        }
        else
        {
            auto* skelCpnt = GetEntityCpnt_SkeletonComponent(*ent);
            if (!skelCpnt)
            {
                ImGuiCTX::PushStyleColor _coloredText(ImGuiCol_Text, colorTextRed);
                ImGui::Text("[-] Has no SkeletonComponent");
            }
            else
            {
                static ImGuiTextBuffer buf; buf.resize(0);
                buf.appendf(
                    "SkeletonComponent: %llX\n"
                    , skelCpnt
                );
                SharedPtrNew<Skeleton>* sharedMainSkel = skelCpnt->MainSkeleton;
                Skeleton* mainSkel = sharedMainSkel->GetPtr();
                buf.appendf(
                    "MainSkeleton: %llu => %s\n"
                    "%hu AddonSkeletons\n"
                    , sharedMainSkel->handle
                    , ACU::Handles::HandleToText(sharedMainSkel->handle)
                    , skelCpnt->AddonSkeletons.size
                );
                ImGui::Text(
                    buf.c_str()
                );
                if (ImGui::Button("Export skeletons as JSON to clipboard"))
                {
                    ExportSkeletonsAsJSON(*skelCpnt);
                }
            }
        }
    }
}
void DrawSkeletonExperimentsControls()
{
    if (ImGui::Button("Dump player skeletons/bones tree"))
    {
        if (SkeletonComponent* skelCpnt = ACU::GetPlayerCpnt_SkeletonComponent())
        {
            SkelDumper().DumpSkeletons(*skelCpnt);
        }
    }
    DrawSkeletonExportControls();
}
