#pragma once

#include "ImGuiCTX.h"
#include "Handles.h"

#include "ACU/ManagedPtrs/ManagedPtrs.h"
#include "ACU/Animation.h"


struct MyPlayableAnim
{
    const uint64 handle;
    const char* name;
};
extern int g_currentAnimIdx;

namespace ImGui
{
inline bool InputUInt64(const char* label, uint64* v, uint64 step = 1, uint64 step_fast = 100, ImGuiInputTextFlags flags = 0)
{
    const char* format = "%lld";
    return InputScalar(label, ImGuiDataType_U64, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}
} // namespace ImGui

class AnimationPicker
{
public:
    bool Draw(const char* label, ACU::StrongRef<Animation>& inOut);
};
