#include "pch.h"

#include "Request_FreezeFOV.h"
#include "MainConfig.h"

#include "ACU/ACUPlayerCameraComponent.h"

extern float g_FreezeFOV_min = 0.2f;
extern float g_FreezeFOV_max = 2.0f;

inline float ClampFloat(float value, float min_, float max_)
{
    if (value < min_) { return min_; }
    if (value > max_) { return max_; }
    return value;
}
void WhenSettingFOVforFrame_Override(AllRegisters* params)
{
    auto* cam = (ACUPlayerCameraComponent*)params->r14_;
    float& fov = g_Config.personalRequests->freezeFOV->fov;
    fov = ClampFloat(fov, g_FreezeFOV_min, g_FreezeFOV_max);
    cam->fov_mb_pi_4 = fov;
}
FreezeFOV::FreezeFOV()
{
    uintptr_t whenSettingFOVforFrame = 0x141F3FE3B;
    PresetScript_CCodeInTheMiddle(whenSettingFOVforFrame, 6,
        WhenSettingFOVforFrame_Override, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, false);
}

#include "ImGuiCTX.h"
void DrawFreezeFOVControls()
{
    ImGuiCTX::Indent _ind;
    ImGui::SliderFloat("Global FOV", &g_Config.personalRequests->freezeFOV->fov.get(), g_FreezeFOV_min, g_FreezeFOV_max);
    ImGui::SameLine();
    if (ImGui::Button("Reset to 1"))
    {
        g_Config.personalRequests->freezeFOV->fov = 1.0f;
    }
}
