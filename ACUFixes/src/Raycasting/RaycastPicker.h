#pragma once

#include "vmath/vmath.h"
#include "ACU/Entity.h"
#include "ACU_SharedPtrs.h"

#include "Raycasting.h"
#include "ImGuiCTX.h"
#include "ImGui3D.h"

class RaycastPickerModal
{
public:
    void Pick(const char* buttonLabel
        , std::invocable<const MyRaycastSuccessfulHit&> auto&& callableOnConfirmedHit
        , std::invocable<const MyRaycastSuccessfulHit&> auto&& callableOnEveryHit = DefaultOnEveryHit
        , std::invocable<> auto&& onNoHit = DefaultOnNoHit
    );
    static void DefaultOnEveryHit(const MyRaycastSuccessfulHit& hit)
    {
        Entity* ent = hit.m_Entity.GetPtr();
        ImGuiTextBuffer buf;
        buf.appendf("Entity at %llX, Handle: %llX\n", ent, hit.m_Entity.GetHandle());
        buf.appendf(
            "[%8.2f,%8.2f,%8.2f]"
            , hit.m_HitLocation.x
            , hit.m_HitLocation.y
            , hit.m_HitLocation.z
        );
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
    static void DefaultOnNoHit()
    {
        ImGui::Text("Select target (max range is ~30m)");
    }
};

void RaycastPickerModal::Pick(const char* buttonLabel
    , std::invocable<const MyRaycastSuccessfulHit&> auto&& callableOnConfirmedHit
    , std::invocable<const MyRaycastSuccessfulHit&> auto&& callableOnEveryHit
    , std::invocable<> auto&& onNoHit
) {
    ImGuiCTX::PushID _id{ this };
    std::optional<MyRaycastSuccessfulHit> result;

    if (ImGui::Button(buttonLabel))
    {
        ImGui::OpenPopup("dummy_popup_for_raycast");
    }
    ImVec2 window_pos = ImGui::GetMousePos();
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.60f); // Transparent background

    ImGuiWindowFlags window_flags = 0;
    //window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoResize;
    ImGuiCTX::PushStyleColor _transparentModalBackground{ ImGuiCol_ModalWindowDimBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f } };

    if (ImGuiCTX::PopupModal _dummyModalPopup{ "dummy_popup_for_raycast", 0, window_flags })
    {
        result = Raycasting::CastRayToMouseOnScreen(40.0f);
        if (result)
            callableOnEveryHit(*result);
        else
            onNoHit();
        if (ImGui::IsMouseClicked(0))
        {
            // Left Mouse Button just clicked, finish picking.
            ImGui::CloseCurrentPopup();
            if (result)
                callableOnConfirmedHit(*result);
            return;
        }
        else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::CloseCurrentPopup();
            return;
        }
    }
}
