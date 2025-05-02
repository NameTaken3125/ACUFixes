#include "pch.h"

#include "Hack_PickEntityToShoot.h"

#include "ACU/HumanStatesHolder.h"
#include "ACU/Entity.h"
#include "Experimental_StrongPtr.h"

ACU::WeakPtr<Entity> g_PickedEntityToShoot;
ACUSharedPtr_Strong<Entity> GetRangedWeaponTargetEntityFromRaycastPicker()
{
	return g_PickedEntityToShoot.Lock();
}

void WhenGettingRangedTargetEntity_OverrideWithPickedOne(AllRegisters* params)
{
	auto* humanStates = (HumanStatesHolder*)params->rcx_;
	ACUSharedPtr_Strong<Entity> pickedEntity = GetRangedWeaponTargetEntityFromRaycastPicker();
	if (!pickedEntity.GetPtr()) { return; }
	SharedPtrNew<Entity>* prevTarget = humanStates->shared_quickshotTarget_mb;
	pickedEntity.GetSharedBlock().IncrementWeakRefcount();
	humanStates->shared_quickshotTarget_mb = &pickedEntity.GetSharedBlock();
	prevTarget->DecrementWeakRefcount();
	int noop = 0;
}

PickEntityToShoot::PickEntityToShoot()
{
	uintptr_t whenGettingRangedTargetEntity = 0x141ACDB00;
	PresetScript_CCodeInTheMiddle(whenGettingRangedTargetEntity, 7,
		WhenGettingRangedTargetEntity_OverrideWithPickedOne, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}


#include "ImGui3D/ImGui3DCustomDraw.h"
#include "Raycasting/RaycastPicker.h"
extern ACU::WeakPtr<Entity> g_PickedEntityToShoot;
class OverrideRangedTarget_3DDrawer : public ImGui3D::CustomDraw::CustomDrawer
{
public:
    OverrideRangedTarget_3DDrawer() { ImGui3D::CustomDraw::CustomDraw_Subscribe(*this); }
    ~OverrideRangedTarget_3DDrawer() { ImGui3D::CustomDraw::CustomDraw_Unsubscribe(*this); }
    virtual void DoDraw() override
    {
        ACUSharedPtr_Strong<Entity> locked = g_PickedEntityToShoot.Lock();
        if (Entity* ent = locked.GetPtr())
        {
            BoundingVolume& bbox = ent->BoundingVolume_;
            Matrix4f bboxScale = Matrix4f::createScale(
                bbox.Max.x - bbox.Min.x,
                bbox.Max.y - bbox.Min.y,
                bbox.Max.z - bbox.Min.z
            );
            Matrix4f bboxPos = Matrix4f::createTranslation(bbox.Min);
            Matrix4f bboxTransform = ent->GetTransform() * bboxPos * bboxScale;
            ImGui3D::DrawWireModelTransform(ImGui3D::GetModel_Cube01(), bboxTransform);
        }
    }
} g_OverrideRangedTarget_CustomDraw;
#include "MyLog.h"
void RaycastPicker_PickRangedWeaponTarget()
{
    static RaycastPickerModal picker;
    static Entity* mostRecentEntity = nullptr;
    picker.Pick(
        "Pick an entity"
        , [&](const MyRaycastSuccessfulHit& hit)
        {
            mostRecentEntity = hit.m_Entity.GetPtr();
            LOG_DEBUG(DefaultLogger
                , L"[!] Selected!  %7.2f %7.2f %7.2f"
                , hit.m_HitLocation.x
                , hit.m_HitLocation.y
                , hit.m_HitLocation.z
            );
            g_PickedEntityToShoot = hit.m_Entity;
        }
        , RaycastPickerModal::DefaultOnEveryHit
        , RaycastPickerModal::DefaultOnNoHit
    );
    ImGui::Text("Selected Entity: %llX", mostRecentEntity);
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Click to copy to clipboard."
        );
    }
    if (ImGui::IsItemClicked(0))
    {
        static ImGuiTextBuffer buf;
        buf.clear();
        buf.appendf("%llX", mostRecentEntity);
        ImGui::SetClipboardText(buf.c_str());
    }
}
