#include "pch.h"

#include "Common_Plugins/Common_PluginSide.h"

#include "MyAnimationPlayer.h"
#include "AnimationEditor.h"
#include "AnimationPicker.h"

#include "ACU/TypeInfo.h"
#include "ACU/World.h"
#include "ACU/Entity.h"
#include "ACU/HumanStatesHolder.h"
#include "ACU/AtomAnimComponent.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/Skeleton.h"
#include "ACU/SkeletonComponent.h"
#include "ACU/Bone.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"


#include "Handles.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/NumericAdapters.h"
#include "ImGuiCTX.h"

#include "ACU/Memory/ACUAllocs.h"
#include "AtomGraphControls.h"






DEFINE_GAME_FUNCTION(oneOfThoseFns_WhenStartActing, 0x141AC3CA0, __int64, __fastcall, (HumanStatesHolder* p_humanStates, char* a2, Animation* p_anim, uint32 a4, char a5, char a6));
DEFINE_GAME_FUNCTION(oneOfThoseFns_UpdateCinematicAnimationTime, 0x141B1AD30, __int64, __fastcall, (HumanStatesHolder* p_humanStates, float p_time, uint32 p_bodyPartChannelIdx));










static AtomAnimComponent* GetPlayerAtomAnimComponent()
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return nullptr; }
    int cpntIdx_atomAnimCpnt = player->cpntIndices_157.atomAnimCpnt;
    return static_cast<AtomAnimComponent*>(player->cpnts_mb[cpntIdx_atomAnimCpnt]);
}








MyPlayedAnimation::MyPlayedAnimation(const ACU::StrongRef<Animation>& animStrongRef, uint64 playStartTime)
    : m_playedAnimationStrongRef(animStrongRef)
    , m_LastChangeTimestamp(playStartTime)
    , m_LastChangeAnimTime(0)
{}
float MyPlayedAnimation::GetDuration()
{
    if (Animation* anim = m_playedAnimationStrongRef.GetPtr())
    {
        return anim->Length;
    }
    return 0;
}
bool MyPlayedAnimation::IsPaused()
{
    const bool isPaused = m_IsPaused;
    return isPaused;
}
float MyAnimationPlayer::CalculateAnimTime(MyPlayedAnimation& anim, uint64 animatorTime)
{
    if (anim.IsPaused())
    {
        return anim.m_LastChangeAnimTime;
    }
    float timeElapsedSinceLastChange = (animatorTime - anim.m_LastChangeTimestamp) / 30000.0f;
    float animTimeChange = timeElapsedSinceLastChange * m_speedMult;
    float newAnimTime = anim.m_LastChangeAnimTime + animTimeChange;
    if (!m_isLooping)
    {
        if (newAnimTime < 0) newAnimTime = 0;
        else if (newAnimTime > anim.GetDuration()) newAnimTime = anim.GetDuration();
    }
    else
    {
        float duration = anim.GetDuration();
        if (newAnimTime > duration) newAnimTime = fmodf(newAnimTime, duration);
        else if (newAnimTime < 0) newAnimTime = duration - fmodf(-newAnimTime, duration);
    }
    return newAnimTime;
}
void MyAnimationPlayer::Pause(MyPlayedAnimation& anim)
{
    uint64 animatorTime = GetAnimatorTime();
    float currentAnimTime = CalculateAnimTime(anim, animatorTime);
    anim.m_LastChangeTimestamp = animatorTime;
    anim.m_LastChangeAnimTime = currentAnimTime;
    anim.m_IsPaused = true;
}
void MyAnimationPlayer::PauseAt(MyPlayedAnimation& anim, float animTime)
{
    uint64 animatorTime = GetAnimatorTime();
    anim.m_LastChangeTimestamp = animatorTime;
    anim.m_LastChangeAnimTime = animTime;
    anim.m_IsPaused = true;
}
void MyAnimationPlayer::Unpause(MyPlayedAnimation& anim)
{
    anim.m_LastChangeTimestamp = GetAnimatorTime();
    anim.m_IsPaused = false;
}
void MyAnimationPlayer::OnAnimationEdited(Animation& anim, AnimationKeyframeTime_t animTime)
{
    if (m_playedAnim && m_playedAnim->m_playedAnimationStrongRef.GetPtr() == &anim)
    {
        PauseAt(*m_playedAnim, animTime / 60.0f);
    }
}

MyAnimationPlayer g_MyAnimationPlayer;

uint64 MyAnimationPlayer::GetAnimatorTime()
{
    auto* world = World::GetSingleton();
    if (!world) { return 0; }
    return world->clockInWorldWithSlowmotion.currentTimestamp;
}
void RestartAnimation_impl(Animation& anim)
{
    auto* humanStates = HumanStatesHolder::GetForPlayer();
    if (humanStates)
    {
        char a2 = 0;
        oneOfThoseFns_WhenStartActing(humanStates, &a2, &anim, 0, 1, 1);
    }
}
void MyAnimationPlayer::StartAnimation(ACU::StrongRef<Animation>& sharedAnim)
{
    m_playedAnim.emplace(sharedAnim, GetAnimatorTime());
    if (Animation* anim = sharedAnim.GetPtr())
    {
        RestartAnimation_impl(*anim);
    }
}
//scalar CinematicAnimationTime; // 0x2cf6a276/754360950
const uint32 hash_CinematicAnimationTime = 0x2cf6a276;
void MyAnimationPlayer::DrawTimeSlider()
{
    auto* animCpnt = GetPlayerAtomAnimComponent();
    if (!animCpnt)
    {
        ImGuiCTX::PushStyleColor _redText(ImGuiCol_Text, ImVec4(0.869f, 0.389f, 0.376f, 1.0f));
        ImGui::Text("Cannot find the AtomAnimComponent");
        return;
    }
    if (!animCpnt->pD0)
    {
        ImGuiCTX::PushStyleColor _redText(ImGuiCol_Text, ImVec4(0.869f, 0.389f, 0.376f, 1.0f));
        ImGui::Text("AtomAnimComponent GraphEvaluation doesn't exist");
        return;
    }
    float* cinematicAnimTime = GetGraphVariable<float>(*animCpnt->pD0, hash_CinematicAnimationTime);
    if (!cinematicAnimTime)
    {
        ImGuiCTX::PushStyleColor _redText(ImGuiCol_Text, ImVec4(0.869f, 0.389f, 0.376f, 1.0f));
        ImGui::Text("Cannot find the \"CinematicAnimationTime\" graph variable");
        return;
    }
    float timeMin = 0;
    float timeMax = m_playedAnim ? m_playedAnim->GetDuration() : 5;
    if (ImGui::SliderFloat("CinematicAnimationTime", cinematicAnimTime, timeMin, timeMax))
    {
        if (m_playedAnim)
        {
            PauseAt(*m_playedAnim, *cinematicAnimTime);
        }
        SetGraphVariable<float>(*animCpnt->pD0, hash_CinematicAnimationTime, *cinematicAnimTime);
    }
}
void DrawSpoofAnimationExperiments();
void MyAnimationPlayer::DrawControls()
{
    DrawSpoofAnimationExperiments();
    static AnimationPicker picker;
    static ACU::StrongRef<Animation> animToRestart;
    ImGui::Separator();
    if (picker.Draw("Pick animation to play", animToRestart))
    {
        if (Animation* anim = animToRestart.GetPtr())
        {
            m_playedAnim.emplace(animToRestart, GetAnimatorTime());
            RestartAnimation_impl(*anim);
        }
    }
    ImGui::Separator();
    if (ImGui::Button("Restart cinematic anim"))
    {
        if (Animation* anim = animToRestart.GetPtr())
        {
            m_playedAnim.emplace(animToRestart, GetAnimatorTime());
            RestartAnimation_impl(*anim);
        }
    }
    if (m_playedAnim)
    {
        if (ImGui::Button("|<<"))
        {
            m_playedAnim->m_LastChangeTimestamp = GetAnimatorTime();
            m_playedAnim->m_LastChangeAnimTime = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("<<"))
        {
            Pause(*m_playedAnim);
            m_speedMult = -fabsf(m_speedMult);
            Unpause(*m_playedAnim);
        }
        ImGui::SameLine();
        const bool isPaused = m_playedAnim->IsPaused();
        if (!isPaused)
        {
            if (ImGui::Button(" Pause "))
            {
                Pause(*m_playedAnim);
            }
        }
        else
        {
            if (ImGui::Button("Unpause"))
            {
                Unpause(*m_playedAnim);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(">>"))
        {
            Pause(*m_playedAnim);
            m_speedMult = fabsf(m_speedMult);
            Unpause(*m_playedAnim);
        }
        ImGui::SameLine();
        if (ImGui::Button(">>|"))
        {
            m_playedAnim->m_LastChangeTimestamp = GetAnimatorTime();
            m_playedAnim->m_LastChangeAnimTime = m_playedAnim->GetDuration();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop cinematic animation"))
        {
            if (auto* humanStates = HumanStatesHolder::GetForPlayer())
            {
                DEFINE_GAME_FUNCTION(oneOfThoseFns_WhenFinishedCinematic2_mb, 0x141B1A630, __int64, __fastcall, (HumanStatesHolder * a1, SeamlessCinematicOutroTransitionOperatorData * a2));
                oneOfThoseFns_WhenFinishedCinematic2_mb(humanStates, &humanStates->cinematicOutroData_mb);
            }
            m_playedAnim.reset();
        }
        if (ImGui::Button("Send to editor"))
        {
            g_AnimationEditor.SetAnimation(m_playedAnim->m_playedAnimationStrongRef);
        }
    }
    DrawTimeSlider();
    float newSpeedMult = m_speedMult;
    if (ImGui::SliderFloat("Speed", &newSpeedMult, -2.0f, 2.0f))
    {
        if (m_playedAnim)
        {
            Pause(*m_playedAnim);
            Unpause(*m_playedAnim);
        }
        m_speedMult = newSpeedMult;
    }
    bool newIsLooping = m_isLooping;
    if (ImGui::Checkbox("Looping", &newIsLooping))
    {
        if (m_playedAnim)
        {
            uint64 animatorTime = GetAnimatorTime();
            float currentAnimTime = CalculateAnimTime(*m_playedAnim, animatorTime);
            m_playedAnim->m_LastChangeTimestamp = animatorTime;
            m_playedAnim->m_LastChangeAnimTime = currentAnimTime;
        }
        m_isLooping = newIsLooping;
    }
}
void MyAnimationPlayer::UpdateAnimations()
{
    if (!m_playedAnim) { return; }
    auto* animCpnt = GetPlayerAtomAnimComponent();
    if (!animCpnt) { return; }

    float currentAnimTime = CalculateAnimTime(*m_playedAnim, GetAnimatorTime());
    auto* graphEval = animCpnt->pD0;
    if (!graphEval) return;
    SetGraphVariable<float>(*graphEval, hash_CinematicAnimationTime, currentAnimTime);
}

DEFINE_GAME_FUNCTION(Animation__ctor, 0x140053270, Animation*, __fastcall, (Animation* a1));
DEFINE_GAME_FUNCTION(AnimTrackData__ctor, 0x1400530D0, AnimTrackData*, __fastcall, (AnimTrackData* a1));

class MySpoofAnimation
{
public:
    MySpoofAnimation(uint64 handle);
    ~MySpoofAnimation();
    ACU::StrongRef<Animation>& GetAnim() { return m_sharedPtr; }
    AnimTrackData& GetAnimTrackData() { return (AnimTrackData&)m_buf_animTrackData[0]; }
private:
    uint64 m_handle;
    ACU::StrongRef<Animation> m_sharedPtr;
    std::vector<byte> m_buf_animTrackData;
};
class SmthDuringDeserializeManagedObj
{
public:
    static SmthDuringDeserializeManagedObj& GetSingleton() { return *(SmthDuringDeserializeManagedObj*)0x1450A9628; }
};

DEFINE_GAME_FUNCTION(FindManagedObjectByHandle, 0x1426F5950, ManagedObject*, __fastcall, (SmthDuringDeserializeManagedObj* a1, unsigned __int64 p_handle));
Animation* CreateManagedAnimationByHandle(uint64 handle)
{
    using TargetType = Animation;

    ManagedObject* preexistingManObj = FindManagedObjectByHandle(&SmthDuringDeserializeManagedObj::GetSingleton(), handle);
    if (preexistingManObj) { return static_cast<TargetType*>(preexistingManObj); }

    return static_cast<TargetType*>(UsesTypeInfoCreate(handle, 0, &TargetType::GetTI()));
}
ACU::StrongRef<Animation> FindOrCreateSharedAnimationByHandle(uint64 handle)
{
    using TargetType = Animation;

    ACU::StrongRef<TargetType> sharedAnim = ACU::StrongRef<TargetType>(handle);
    // Managed objects lock is unnecessarily released here.
    if (!sharedAnim.GetSharedBlock().manObj)
    {
        TypeInfo& ti = TargetType::GetTI();
        TargetType* newManObj = static_cast<TargetType*>(UsesTypeInfoCreate(handle, 0, &ti));
        JoinManagedObjectAndHandle_mb(handle, newManObj);
    }
    return sharedAnim;
}
struct MyKeyframe_Translation
{
    uint8 keyframeTime;
    Vector3f translationData;
};
struct MyKeyframe_Quaternion
{
    uint8 keyframeTime;
    Vector4f quat;
};
struct MyKeyframe_FullblastQuaternion_0x1_u16
{
    uint16 keyframeTime;
    Vector4f quat;
};
struct MyKeyframe_FullblastQuaternion_0x0_u8_unused
{
    uint8 keyframeTime;
    Vector4f quat;
};
constexpr float magic_mul = std::_Bit_cast<float, uint32>(0x35B5050A); // roughly (1/741453.7753816685)
constexpr float magic_add = std::_Bit_cast<float, uint32>(0xBF3504F3); // roughly -0.7071067691
using AnimQuaternionPacked = uint64;
Vector4f ACUQuaternionUnpack(AnimQuaternionPacked quatPacked)
{
    // Implementation from 0x140087490: `AnimationTrackSolver_0xF_UnpackTwoNeighboringQuatKeyframes()`
    Vector4f result;

    __m128 quatPacked2{ 0 };
    quatPacked2.m128_u64[0] = quatPacked;


    // The quaternion is packed into 8 bytes.
    // Take for example the following 8 bytes, comprised of 2 dwords:
    // C4DAFF1E EA765C60
    // To unpack:
    // 1. Split and rearrange the bits into three 20-bit integer values like so: [ai, bi, ci] := [4DAFF, 1EEA7, 65C60] == [318207, 126631, 416864]
    // 2. Convert the three integers into floats and perform a bit of arithmetic to bring them into their original [-1, 1] range:
    //      [318207, 126631, 416864] * magic_mul + magic_add == [-0.27794043, -0.53631932, -0.14488157] =: [a, b, c]
    // 3. Since the squares of quaternion's components add up to 1, recover the fourth component. We assume that this last component is positive:
    //    d = sqrt(1-a*a-b*b-c*c)
    // 4. This final component is the one with the largest absolute magnitude among the [x, y, z, w], which is not necessarily the `w`.
    // 5. The 2 highest bits of the first of two original dwords (C4DAFF1E) are used to determine exactly where the final component goes.
    //      firstDword & 0xC0000000 == 0xC0000000 => [x, y, z, w] := [a, b, c, d]
    //      firstDword & 0xC0000000 == 0x80000000 => [x, y, z, w] := [a, b, d, c]
    //      firstDword & 0xC0000000 == 0x40000000 => [x, y, z, w] := [a, d, b, c]
    //      firstDword & 0xC0000000 == 0x00000000 => [x, y, z, w] := [d, a, b, c]

    uint32 ai = (quatPacked2.m128_u32[0] >> 8) & 0xFFFFF;
    uint32 bi = ((quatPacked2.m128_u32[0] & 0xFF) << 12) | ((quatPacked2.m128_u32[1] >> 20) & 0xFFF);
    uint32 ci = ((quatPacked2.m128_u32[1]) & 0xFFFFF);

    float a = float(ai) * magic_mul + magic_add;
    float b = float(bi) * magic_mul + magic_add;
    float c = float(ci) * magic_mul + magic_add;

    float d = std::sqrtf(1 - a * a - b * b - c * c);

    uint32 whereToPutTheFourthComponent = quatPacked2.m128_u32[0] & 0xC0000000;

    if (whereToPutTheFourthComponent)
    {
        if (whereToPutTheFourthComponent == 0x40000000)
        {
            result = { a, d, b, c };
        }
        else if (whereToPutTheFourthComponent == 0x80000000)
        {
            result = { a, b, d, c };
        }
        else
        {
            result = { a, b, c, d };
        }
    }
    else
    {
        result = { d, a, b, c };
    }
    return result;
}
AnimQuaternionPacked ACUQuaternionPack(Vector4f quat)
{
    float ax = fabsf(quat.x);
    float ay = fabsf(quat.y);
    float az = fabsf(quat.z);
    float aw = fabsf(quat.w);
    //std::array<float, 4> quatAbs = { fabsf(quat.v.x), fabsf(quat.v.y), fabsf(quat.v.z),fabsf(quat.w) };
    //int componentIdxWithLargestMagnitude = std::max_element(quatAbs.begin(), quatAbs.end()) - quatAbs.begin();
    int componentIdxWithLargestMagnitude = 0;
    float largestComponent = ax;
    if (ay > largestComponent)
    {
        largestComponent = ay;
        componentIdxWithLargestMagnitude = 1;
    }
    if (az > largestComponent)
    {
        largestComponent = az;
        componentIdxWithLargestMagnitude = 2;
    }
    if (aw > largestComponent)
    {
        largestComponent = aw;
        componentIdxWithLargestMagnitude = 3;
    }
    uint32 mask_whereToPutTheFourthComponent;
    float a, b, c;

    std::array<float, 4>& quatAsArr = (std::array<float, 4>&)quat;
    if (quatAsArr[componentIdxWithLargestMagnitude] < 0)
    {
        quat = -quat;
    }

    switch (componentIdxWithLargestMagnitude)
    {
    case 0:
        mask_whereToPutTheFourthComponent = 0;
        a = quat.y;
        b = quat.z;
        c = quat.w;
        break;
    case 1:
        mask_whereToPutTheFourthComponent = 0x40000000;
        a = quat.x;
        b = quat.z;
        c = quat.w;
        break;
    case 2:
        mask_whereToPutTheFourthComponent = 0x80000000;
        a = quat.x;
        b = quat.y;
        c = quat.w;
        break;
    case 3:
        mask_whereToPutTheFourthComponent = 0xC0000000;
        a = quat.x;
        b = quat.y;
        c = quat.z;
        break;
    }

    uint32 ai = uint32((a - magic_add) / magic_mul);
    uint32 bi = uint32((b - magic_add) / magic_mul);
    uint32 ci = uint32((c - magic_add) / magic_mul);

    __m128 result{ 0 };

    result.m128_u32[0] |= mask_whereToPutTheFourthComponent;
    result.m128_u32[0] |= (ai & 0xFFFFF) << 8;
    result.m128_u32[0] |= (bi & 0xFF000) >> 12;
    result.m128_u32[1] |= (bi & 0xFFF) << 20;
    result.m128_u32[1] |= ci & 0xFFFFF;

    return result.m128_u64[0];
}
std::vector<byte> ConvertKeyframesToRawTrackBytes(const std::vector<MyKeyframe_FullblastQuaternion_0x1_u16>& keyframes)
{
    const uint16 numKeyframes = (uint16)keyframes.size();
    const uint32 startOfKeyframeValues = (
        2                       // First byte indicates the trackdata type, second would be used to store `u8` number of keyframes, but isn't used by me
        + numKeyframes * 2      // Write keyframetimes in u8 format. First keyframetime is implicitly `0`, so it isn't written.
        + 15) & 0xFFFFFFF0;     // Rounded up to be divisible by 4

    uint32 totalSize =
        startOfKeyframeValues
        + sizeof(Vector4f) * numKeyframes;
    std::vector<byte> result;
    result.resize(totalSize, 0);



    result[0] = 1;




    result[1] = 0;
    (uint16&)result[2] = numKeyframes;

    // Write keyframetimes in u8 format. First keyframetime is implicitly `0`, so it isn't written.
    for (size_t i = 1; i < numKeyframes; i++)
    {
        (uint16&)result[2 + i*2] = keyframes[i].keyframeTime;
    }
    Vector4f* keyframeValues = (Vector4f*)&result[startOfKeyframeValues];
    for (size_t i = 0; i < keyframes.size(); i++)
    {
        keyframeValues[i] = keyframes[i].quat;
    }
    return result;
}
std::vector<byte> ConvertKeyframesToRawTrackBytes(const std::vector<MyKeyframe_FullblastQuaternion_0x0_u8_unused>& keyframes)
{
    const uint16 numKeyframes = (uint16)keyframes.size();
    const uint32 startOfKeyframeValues = (
        2                       // First byte indicates the trackdata type, second would be used to store `u8` number of keyframes, but isn't used by me
        + numKeyframes * 2      // Write keyframetimes in u8 format. First keyframetime is implicitly `0`, so it isn't written.
        + 15) & 0xFFFFFFF0;     // Rounded up to be divisible by 4

    uint32 totalSize =
        startOfKeyframeValues
        + sizeof(Vector4f) * numKeyframes;
    std::vector<byte> result;
    result.resize(totalSize, 0);



    result[0] = 0;




    result[1] = 0;
    (uint16&)result[2] = numKeyframes;

    // Write keyframetimes in u8 format. First keyframetime is implicitly `0`, so it isn't written.
    for (size_t i = 1; i < numKeyframes; i++)
    {
        (uint16&)result[2 + i*2] = keyframes[i].keyframeTime;
    }
    Vector4f* keyframeValues = (Vector4f*)&result[startOfKeyframeValues];
    for (size_t i = 0; i < keyframes.size(); i++)
    {
        keyframeValues[i] = keyframes[i].quat;
    }
    return result;
}
std::vector<byte> ConvertKeyframesToRawTrackBytes(const std::vector<MyKeyframe_Quaternion>& keyframes)
{
    const uint32 startOfKeyframeValues = (
        1                       // First byte indicates the trackdata type
        + keyframes.size() - 1  // Write keyframetimes in u8 format. First keyframetime is implicitly `0`, so it isn't written.
        + 4) & 0xFFFFFFFC;      // Rounded up to be divisible by 4

    uint32 totalSize =
        startOfKeyframeValues
        + sizeof(AnimQuaternionPacked) * (uint32)keyframes.size();
    std::vector<byte> result;
    result.resize(totalSize, 0);



    result[0] = 15;




    result[1] = (uint8)keyframes.size();

    // Write keyframetimes in u8 format. First keyframetime is implicitly `0`, so it isn't written.
    for (size_t i = 1; i < keyframes.size(); i++)
    {
        result[1 + i] = keyframes[i].keyframeTime;
    }
    for (size_t i = 0; i < keyframes.size(); i++)
    {
        (AnimQuaternionPacked&)result[startOfKeyframeValues + sizeof(AnimQuaternionPacked) * i] = ACUQuaternionPack(keyframes[i].quat);
    }
    return result;
}
std::vector<byte> ConvertKeyframesToRawTrackBytes(const std::vector<MyKeyframe_Translation>& keyframes)
{
    const uint32 startOfKeyframeValues = (
        1                       // First byte indicates the trackdata type
        + keyframes.size() - 1  // Write keyframetimes in u8 format. First keyframetime is implicitly `0`, so it isn't written.
        + 4) & 0xFFFFFFFC;      // Rounded up to be divisible by 4

    uint32 totalSize =
        startOfKeyframeValues
        + sizeof(Vector3f) * (uint32)keyframes.size();
    std::vector<byte> result;
    result.resize(totalSize, 0);



    result[0] = 27;




    result[1] = (uint8)keyframes.size();

    // Write keyframetimes in u8 format. First keyframetime is implicitly `0`, so it isn't written.
    for (size_t i = 1; i < keyframes.size(); i++)
    {
        result[1 + i] = keyframes[i].keyframeTime;
    }
    for (size_t i = 0; i < keyframes.size(); i++)
    {
        (Vector3f&)result[startOfKeyframeValues + sizeof(Vector3f) * i] = keyframes[i].translationData;
    }
    return result;
}




#include "imGuIZMO.quat/imGuIZMOquat.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
namespace ImGui
{
inline bool SliderEulerAngles(const char* label, Vector3f& v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiDataType data_type = ImGuiDataType_Float;
    int components = 3;
    constexpr float gimbalLockEps = 1e-2f;
    float v_min[3] = { -pi(), -pi() / 2 + gimbalLockEps, -pi(), };
    float v_max[3] = { pi(), pi() / 2 - gimbalLockEps, pi() };
    const char* format = "%.3f";
    ImGuiSliderFlags flags = 0;

    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components, CalcItemWidth());

    value_changed |= SliderScalar("Roll", data_type, &v.x, &v_min[0], &v_max[0], format, flags);
    PopItemWidth();

    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= SliderScalar("Pitch", data_type, &v.y, &v_min[1], &v_max[1], format, flags);
    PopItemWidth();

    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= SliderScalar("Yaw", data_type, &v.z, &v_min[2], &v_max[2], format, flags);
    PopItemWidth();

    PopID(); // `label`

    const char* label_end = FindRenderedTextEnd(label);
    if (label != label_end)
    {
        SameLine(0, g.Style.ItemInnerSpacing.x);
        TextEx(label, label_end);
    }

    EndGroup();
    return value_changed;
}
} // namespace ImGui


using ResultEditedKeyframe_t = std::optional<AnimationKeyframeTime_t>;
class AnimationDatatypeSolverBase
{
public:
    AnimationDatatypeSolverBase() {}
    virtual ~AnimationDatatypeSolverBase() {}
    virtual uint8 GetDatatype() = 0;
    virtual ResultEditedKeyframe_t DrawEditor(byte* trackdata) = 0;
    virtual json::JSON ExportKeyframesToJSON(byte* trackdata) = 0;
    virtual std::vector<byte> ImportKeyframesFromJSON(json::JSON& jsonKeyframes) = 0;
};
template<uint8 Datatype>
class AnimationDatatypeSolver : public AnimationDatatypeSolverBase
{
public:
    virtual uint8 GetDatatype() override { return Datatype; }
};
class ADS_Translation : public AnimationDatatypeSolver<27>
{
public:
    virtual ResultEditedKeyframe_t DrawEditor(byte* trackdata) override
    {
        uint8 numKeyframes = trackdata[1];
        const uint32 startOfKeyframeValues = (numKeyframes + 4) & 0xFFFFFFFC;      // Rounded up to be divisible by 4

        ResultEditedKeyframe_t resultEditedKeyframe;

        ImGuiTextBuffer buf;

        auto EditTranslation = [&buf](uint8 keyframeTimeU8, Vector3f& keyframeData) -> ResultEditedKeyframe_t
        {
            ResultEditedKeyframe_t editedKeyframe;

            buf.clear();
            float keyframeTimeFlt = keyframeTimeU8 / 60.0f;
            buf.appendf("Frame: %d, Time: %f", keyframeTimeU8, keyframeTimeFlt);
            ImGuiCTX::PushID _id(&keyframeData);
            if (ImGui::SliderFloat3(buf.c_str(), keyframeData, -3, 3))
            {
                editedKeyframe = keyframeTimeU8;
            }
            return editedKeyframe;
        };
        //First keyframe (with implicit keyframetime `0`).
        if (ResultEditedKeyframe_t edited = EditTranslation(0, (Vector3f&)trackdata[startOfKeyframeValues]))
        {
            resultEditedKeyframe = edited;
        }
        for (size_t i = 1; i < numKeyframes; i++)
        {
            if (ResultEditedKeyframe_t edited = EditTranslation(trackdata[1 + i], (Vector3f&)trackdata[startOfKeyframeValues + sizeof(Vector3f&) * i]))
            {
                resultEditedKeyframe = edited;
            }
        }
        return resultEditedKeyframe;
    }
    virtual json::JSON ExportKeyframesToJSON(byte* trackdata) override
    {
        json::JSON jsonKeyframes = json::Array();
        uint8 numKeyframes = trackdata[1];
        const uint32 startOfKeyframeValues = (numKeyframes + 4) & 0xFFFFFFFC;      // Rounded up to be divisible by 4

        Vector3f* values = (Vector3f*)&trackdata[startOfKeyframeValues];

        auto WriteKeyframe = [](uint8 Frame, Vector3f& LocalPosition) -> json::JSON
        {
            json::JSON jsonKF;
            WRITE_JSON_VARIABLE(jsonKF, Frame, IntegerAdapter);
            WRITE_JSON_VARIABLE(jsonKF, LocalPosition, Vector3fAdapter);
            return jsonKF;
        };
        jsonKeyframes.append(WriteKeyframe(0, values[0]));
        for (size_t i = 1; i < numKeyframes; i++)
        {
            jsonKeyframes.append(WriteKeyframe(trackdata[1 + i], values[i]));
        }
        return jsonKeyframes;
    }
    virtual std::vector<byte> ImportKeyframesFromJSON(json::JSON& jsonKeyframes) override
    {
        std::vector<byte> result;

        using CompressedValue_t = Vector3f;

        uint8 numKeyframes = (uint8)jsonKeyframes.size();

        const uint32 startOfKeyframeValues = (numKeyframes + 4) & 0xFFFFFFFC;      // Rounded up to be divisible by 4

        uint32 totalSize =
            startOfKeyframeValues
            + sizeof(CompressedValue_t) * (uint32)numKeyframes;
        result.resize(totalSize, 0);

        CompressedValue_t* values = (CompressedValue_t*)&result[startOfKeyframeValues];



        result[0] = GetDatatype();




        result[1] = numKeyframes;


        for (uint8 i = 0; i < numKeyframes; i++)
        {
            json::JSON& jsonFrame = jsonKeyframes[i];
            if (i != 0)
            {
                uint8 Frame;
                READ_JSON_VARIABLE(jsonFrame, Frame, IntegerAdapter);
                result[1 + i] = Frame;
            }
            Vector3f LocalPosition;
            READ_JSON_VARIABLE(jsonFrame, LocalPosition, Vector3fAdapter);
            values[i] = LocalPosition;
        }
        return result;
    }
};
// this implementation assumes normalized quaternion
// converts to Euler angles in 3-2-1 sequence
Vector3f QuaternionToEulerAngles(Vector4f q) {
    Vector3f angles;

    // roll (x-axis rotation)
    float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.x = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
    float cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
    angles.y = 2 * std::atan2(sinp, cosp) - M_PI / 2;

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.z = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}
Vector4f EulerToQuaternion(float roll, float pitch, float yaw) // roll (x), pitch (y), yaw (z), angles are in radians
{
    // Abbreviations for the various angular functions

    float cr = cos(roll * 0.5);
    float sr = sin(roll * 0.5);
    float cp = cos(pitch * 0.5);
    float sp = sin(pitch * 0.5);
    float cy = cos(yaw * 0.5);
    float sy = sin(yaw * 0.5);

    Vector4f q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}
class ADS_Quaternion : public AnimationDatatypeSolver<15>
{
public:
    virtual ResultEditedKeyframe_t DrawEditor(byte* trackdata) override
    {
        uint8 numKeyframes = trackdata[1];
        const uint32 startOfKeyframeValues = (numKeyframes + 4) & 0xFFFFFFFC;      // Rounded up to be divisible by 4

        ImGuiTextBuffer buf;

        ResultEditedKeyframe_t resultEditedKeyframe;
        auto EditQuat = [&buf](uint8 keyframeTimeU8, AnimQuaternionPacked& quatPackedRaw)
        {
            buf.clear();
            float keyframeTimeFlt = keyframeTimeU8 / 60.0f;
            buf.appendf("Frame: %d, Time: %f", keyframeTimeU8, keyframeTimeFlt);
            Vector4f keyframeData = ACUQuaternionUnpack(quatPackedRaw);
            ImGuiCTX::PushID _id(&quatPackedRaw);
            auto SaturateThreeFloats = [](float& a, float& b, float& c, float requiredSumOfSquares)
            {
                float initialSumSquares = a * a + b * b + c * c;
                float sumSqrt = sqrtf(initialSumSquares);
                a /= sumSqrt;
                b /= sumSqrt;
                c /= sumSqrt;
            };
            bool isChanged = false;
            isChanged |= ImGui::gizmo3D("##gizmo1", (quat&)keyframeData /*, size,  mode */);
            ImGui::SameLine();
            isChanged |= ImGui::SliderFloat4(buf.c_str(), (float*)&keyframeData, -1, 1);
            if (isChanged)
            {
                keyframeData.normalize();
                quatPackedRaw = ACUQuaternionPack(keyframeData);
            }
        };
        auto EditQuatEuler = [&buf](uint8 keyframeTimeU8, AnimQuaternionPacked& quatPackedRaw) -> ResultEditedKeyframe_t
        {
            ResultEditedKeyframe_t editedKeyframe;

            buf.clear();
            float keyframeTimeFlt = keyframeTimeU8 / 60.0f;
            buf.appendf("%d/%f", keyframeTimeU8, keyframeTimeFlt);
            Vector4f keyframeDataQuat = ACUQuaternionUnpack(quatPackedRaw);
            Vector3f keyframeData = QuaternionToEulerAngles(keyframeDataQuat);
            ImGuiCTX::PushID _id(&quatPackedRaw);
            auto SaturateThreeFloats = [](float& a, float& b, float& c, float requiredSumOfSquares)
            {
                float initialSumSquares = a * a + b * b + c * c;
                float sumSqrt = sqrtf(initialSumSquares);
                a /= sumSqrt;
                b /= sumSqrt;
                c /= sumSqrt;
            };
            // TODO:
            // - Pitch/Y is in [-pi/2, -pi/2]
            // - Roll/X snaps from -pi to +pi
            // - not sure about Z
            ImGui::Text(buf.c_str());
            ImGui::NextColumn();
            if (ImGui::SliderEulerAngles("", keyframeData))
            {
                quatPackedRaw = ACUQuaternionPack(EulerToQuaternion(keyframeData.x, keyframeData.y, keyframeData.z));
                editedKeyframe = keyframeTimeU8;
            }
            ImGui::NextColumn();
            return editedKeyframe;
        };
        //{
        //    // TABLES API
        //    static ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable;
        //    if (ImGui::BeginTable("keyframes", 2, flags))
        //    {
        //        ImGui::TableSetupColumn("Frame/Time", ImGuiTableColumnFlags_WidthAutoResize);
        //        ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthStretch);
        //        ImGui::TableHeadersRow();
        //        ImGui::TableNextRow();
        //        //First keyframe (with implicit keyframetime `0`).
        //        if (ResultEditedKeyframe_t edited = EditQuatEuler(0, (AnimQuaternionPacked&)trackdata[startOfKeyframeValues]))
        //        {
        //            resultEditedKeyframe = edited;
        //        }
        //        ImGui::TableNextRow();
        //        for (size_t i = 1; i < numKeyframes; i++)
        //        {
        //            if (ResultEditedKeyframe_t edited = EditQuatEuler(trackdata[1 + i], (AnimQuaternionPacked&)trackdata[startOfKeyframeValues + sizeof(AnimQuaternionPacked) * i]))
        //            {
        //                resultEditedKeyframe = edited;
        //            }
        //        }
        //        ImGui::EndTable();
        //    }
        //}
        {
            // COLUMNS API

            ImGui::Columns(2, "mycolumns"); // 4-ways, with border
            ImGui::Separator();
            ImGui::Text("Frame/Time"); ImGui::NextColumn();
            ImGui::Text("Data"); ImGui::NextColumn();
            ImGui::Separator();

            //First keyframe (with implicit keyframetime `0`).
            if (ResultEditedKeyframe_t edited = EditQuatEuler(0, (AnimQuaternionPacked&)trackdata[startOfKeyframeValues]))
            {
                resultEditedKeyframe = edited;
            }
            for (size_t i = 1; i < numKeyframes; i++)
            {
                if (ResultEditedKeyframe_t edited = EditQuatEuler(trackdata[1 + i], (AnimQuaternionPacked&)trackdata[startOfKeyframeValues + sizeof(AnimQuaternionPacked) * i]))
                {
                    resultEditedKeyframe = edited;
                }
            }
            ImGui::Columns(1);
            ImGui::Separator();
        }
        return resultEditedKeyframe;
    }
    virtual json::JSON ExportKeyframesToJSON(byte* trackdata) override
    {
        json::JSON jsonKeyframes = json::Array();
        uint8 numKeyframes = trackdata[1];
        const uint32 startOfKeyframeValues = (numKeyframes + 4) & 0xFFFFFFFC;      // Rounded up to be divisible by 4

        AnimQuaternionPacked* values = (AnimQuaternionPacked*)&trackdata[startOfKeyframeValues];

        auto WriteKeyframe = [](uint8 Frame, AnimQuaternionPacked& LocalRotation_packed) -> json::JSON
        {
            json::JSON jsonKF;
            Vector4f LocalRotation = ACUQuaternionUnpack(LocalRotation_packed);
            WRITE_JSON_VARIABLE(jsonKF, Frame, IntegerAdapter);
            WRITE_JSON_VARIABLE(jsonKF, LocalRotation, Vector4fAdapter);
            return jsonKF;
        };
        jsonKeyframes.append(WriteKeyframe(0, values[0]));
        for (size_t i = 1; i < numKeyframes; i++)
        {
            jsonKeyframes.append(WriteKeyframe(trackdata[1 + i], values[i]));
        }
        return jsonKeyframes;
    }
    virtual std::vector<byte> ImportKeyframesFromJSON(json::JSON& jsonKeyframes) override
    {
        std::vector<byte> result;

        using CompressedValue_t = AnimQuaternionPacked;

        uint8 numKeyframes = (uint8)jsonKeyframes.size();

        const uint32 startOfKeyframeValues = (numKeyframes + 4) & 0xFFFFFFFC;      // Rounded up to be divisible by 4

        uint32 totalSize =
            startOfKeyframeValues
            + sizeof(CompressedValue_t) * (uint32)numKeyframes;
        result.resize(totalSize, 0);

        CompressedValue_t* values = (CompressedValue_t*)&result[startOfKeyframeValues];



        result[0] = GetDatatype();




        result[1] = numKeyframes;


        for (uint8 i = 0; i < numKeyframes; i++)
        {
            json::JSON& jsonFrame = jsonKeyframes[i];
            if (i != 0)
            {
                uint8 Frame;
                READ_JSON_VARIABLE(jsonFrame, Frame, IntegerAdapter);
                result[1 + i] = Frame;
            }
            Vector4f LocalRotation;
            READ_JSON_VARIABLE(jsonFrame, LocalRotation, Vector4fAdapter);
            values[i] = ACUQuaternionPack(LocalRotation);
        }
        return result;
    }
};

class AnimationSolvers
{
public:
    AnimationDatatypeSolverBase* FindSolverForDatatype(uint8 datatype)
    {
        auto foundIt = std::find_if(m_solvers.begin(), m_solvers.end(), [&](std::unique_ptr<AnimationDatatypeSolverBase>& e)
            {
                return e->GetDatatype() == datatype;
            });
        if (foundIt == m_solvers.end())
        {
            return nullptr;
        }
        return foundIt->get();
    }
    AnimationSolvers()
    {
        m_solvers.push_back(std::make_unique<ADS_Translation>());
        m_solvers.push_back(std::make_unique<ADS_Quaternion>());
        m_solvers.push_back(std::make_unique<ADS_Quaternion>());
    }
    AnimationDatatypeSolverBase* operator[](uint8 idx) { return m_solvers[idx].get(); }
public:
    std::vector<std::unique_ptr<AnimationDatatypeSolverBase>> m_solvers;
};
AnimationSolvers g_AnimationDatatypeSolvers;
ResultEditedKeyframe_t ImGuiDrawAnimationTrackRawData(Animation& anim, uint16 trackdataIdx)
{
    ResultEditedKeyframe_t editedKeyframe;
    if (trackdataIdx >= anim.rawTracks.size)
    {
        ImGui::Text("Bad trackdata index.");
        return editedKeyframe;
    }
    byte* trackdata = (byte*)anim.rawTracks[trackdataIdx];
    uint8 trackdataType = trackdata[0];
    auto* solver = g_AnimationDatatypeSolvers.FindSolverForDatatype(trackdataType);
    if (solver)
    {
        editedKeyframe = solver->DrawEditor(trackdata);
    }
    else
    {
        ImGui::Text("Unknown trackdata type.");
    }
    return editedKeyframe;
}


json::JSON ExportAnimationTracksToJSON_1(Animation& anim)
{
    json::JSON resultTracks = json::Array();
    for (uint16 i = 0; i < anim.rawTracks.size; i++)
    {
        json::JSON track = json::Object();
        uint32 trackID = anim.AnimTrackData_->AnimTrackDataMapping_[i].TrackID;
        track["TrackID"] = std::to_string(trackID);
        byte* trackdata = (byte*)anim.rawTracks[i];
        uint8 trackdataType = (uint8)trackdata[0];
        track["type"] = trackdataType;
        auto* solver = g_AnimationDatatypeSolvers.FindSolverForDatatype(trackdataType);
        if (solver)
        {
            track["Keyframes"] = solver->ExportKeyframesToJSON(trackdata);
        }
        resultTracks.append(std::move(track));
    }
    return resultTracks;
}






template<class Keyframe_t>
class MyAnimTrackdata
{
public:
    uint32 m_TrackID;
    std::vector<Keyframe_t> m_keyframes;
};
template<class Keyframe_t>
void SpoofAnimationRawTrackData(Animation& anim, const MyAnimTrackdata<Keyframe_t>& myTrackdata)
{
    std::vector<byte> trackRawData = ConvertKeyframesToRawTrackBytes(myTrackdata.m_keyframes);
    byte* trackRawDataDetached = ACU::Memory::ACUAllocateBytes((uint32)trackRawData.size(), 0x10);
    std::memcpy(trackRawDataDetached, &trackRawData[0], trackRawData.size());

    auto FindWhereToPutNewTrackInSortedArray = [](uint32 newTrackID, SmallArray<AnimTrackDataMapping>& tracksArray)
    {
        for (uint16 i = 0; i < tracksArray.size; i++)
        {
            if (tracksArray[i].TrackID > newTrackID)
            {
                return i;
            }
        }
        return tracksArray.size;
    };
    uint16 idxOfTheNewTrackInTheSortedArray = FindWhereToPutNewTrackInSortedArray(myTrackdata.m_TrackID, anim.AnimTrackData_->AnimTrackDataMapping_);
    ACU::Memory::SmallArrayInsert(anim.AnimTrackData_->AnimTrackDataMapping_, AnimTrackDataMapping{ myTrackdata.m_TrackID }, idxOfTheNewTrackInTheSortedArray);
    ACU::Memory::SmallArrayInsert<void*>(anim.rawTracks, trackRawDataDetached, idxOfTheNewTrackInTheSortedArray);
}
MySpoofAnimation::~MySpoofAnimation()
{
    Animation& thisAnim = *this->GetAnim().GetPtr();
    std::memset(&thisAnim.rawTracks, 0, sizeof(thisAnim.rawTracks));
    thisAnim.AnimTrackData_ = nullptr;
}
constexpr uint64 handle_skeleton_BaseMale = 28540328525; // => NEW_SDN_CN_U_ArmorSet_02/ACU_BaseSkeleton_Male.Skeleton
constexpr uint64 handle_skeleton_HiddenBlade = 87332257962; // => CN_P_LegacyAvatar_ThomasCarneillon_Base/ACU_U_Arno_IconicWeapon.Skeleton
namespace fs = std::filesystem;
#include "Serialization/ToFromFile/ToFromFile.h"

NewHandlesFactory g_NewHandlesFactory;
ACU::StrongRef<Animation> NewAnimationsFactory::AllocateNewAnimation()
{
    auto InitializeNewAnimation = [](Animation& newManObj, uint64 newHandle) {
        const uint32 animationKey_shouldBeUniqueToAvoidCachingProblemsButIdkHowItsGenerated = newHandle & 0xFFFFFFFF;
        newManObj.AnimationKey = animationKey_shouldBeUniqueToAvoidCachingProblemsButIdkHowItsGenerated;
    };
    return g_NewHandlesFactory.CreateNewManagedObject<Animation>(InitializeNewAnimation);
}
MySpoofAnimation::MySpoofAnimation(uint64 handle)
    : m_handle(handle)
    , m_sharedPtr(FindOrCreateSharedAnimationByHandle(handle))
{
    m_buf_animTrackData.resize(sizeof(AnimTrackData), 0);

    Animation& thisAnim = *this->GetAnim().GetPtr();
    AnimTrackData& thisAnimTrackData = this->GetAnimTrackData();

    AnimTrackData__ctor(&thisAnimTrackData);

    thisAnim.Length = 3.5f;
    thisAnim.AnimTrackData_ = &thisAnimTrackData;
    MyAnimTrackdata<MyKeyframe_Translation> translation_wholeEntity{
        0,
        {
            MyKeyframe_Translation{0x00,Vector3f(0,0,0)},
            MyKeyframe_Translation{0x14,Vector3f(-0.115647f, 0.869830f, -0.000000f) },
            MyKeyframe_Translation{0x26,Vector3f(-0.115647f, 2.094424f, 0.000000f) },
            MyKeyframe_Translation{0x2E,Vector3f(-0.115647f, 2.704295f, 0.000000f) },
            MyKeyframe_Translation{0x34,Vector3f(-0.175634f, 2.944243f, 0.000000f) },
            MyKeyframe_Translation{0x84,Vector3f(-0.175634f, 2.944243f, 0.000000f) },
            MyKeyframe_Translation{0xA2,Vector3f(-0.208109f, 2.535769f, 0.000000f) },
            MyKeyframe_Translation{0xB4,Vector3f(-0.251860f, 2.535769f, 0.000000f) },
        }
    };
    MyAnimTrackdata<MyKeyframe_Translation> translation_hiddenBladeFinalSection{
        0xFD418ADE,
        {
            MyKeyframe_Translation{0x00,Vector3f(0.000000f, 0.000000f, -0.000000f) },
            MyKeyframe_Translation{0x30,Vector3f(0.000000f, 0.000000f, -0.000000f) },
            MyKeyframe_Translation{0x34,Vector3f(-0.000000f, -0.144114f, -0.001529f) },
            MyKeyframe_Translation{0x7E,Vector3f(0.000000f, -0.144115f, -0.001529f) },
            MyKeyframe_Translation{0x82,Vector3f(0.000000f, 0.000000f, -0.000000f) },
        }
    };
    MyAnimTrackdata<MyKeyframe_Quaternion> rotation_Reference{
        743623600U,
        {
            MyKeyframe_Quaternion{0x00,Vector4f(0.000000f, 0.000000f, -0.000000f, 1) },
            MyKeyframe_Quaternion{0x20,Vector4f(0.000000f, 0.000000f, 1.000000f, 0) },
            MyKeyframe_Quaternion{0x40,Vector4f(0.000000f, 1.000000f, -0.000000f, 0) },
            MyKeyframe_Quaternion{0x60,Vector4f(1.000000f, 0.000000f, -0.000000f, 0) },
            MyKeyframe_Quaternion{0x80,Vector4f(0.000000f, 0.000000f, -0.000000f, 1) },
        }
    };
    MyAnimTrackdata<MyKeyframe_Quaternion> rotation_RightArm{
        1806956327U,
        {
            MyKeyframe_Quaternion{0x00,Vector4f(0.000000f, 0.000000f, -0.000000f, 1) },
            MyKeyframe_Quaternion{0x20,Vector4f(0.000000f, 0.000000f, 1.000000f, 0) },
            MyKeyframe_Quaternion{0x40,Vector4f(0.000000f, 1.000000f, -0.000000f, 0) },
            MyKeyframe_Quaternion{0x60,Vector4f(1.000000f, 0.000000f, -0.000000f, 0) },
            MyKeyframe_Quaternion{0x80,Vector4f(0.000000f, 0.000000f, -0.000000f, 1) },
        }
    };
    MyAnimTrackdata<MyKeyframe_Quaternion> rotation_RightArm_defaults{
        1806956327U,
        {
            MyKeyframe_Quaternion{0x00,Vector4f(-0.084260f, -0.456437f, 0.048378f, 0.884435f) },
            MyKeyframe_Quaternion{0x20,Vector4f(-0.084260f, -0.456437f, 0.048378f, 0.884435f) },
            MyKeyframe_Quaternion{0x40,Vector4f(0.8491491675f, -0.0005763564259f, -0.5281325579f, -0.004616710357f) },
            MyKeyframe_Quaternion{0x60,Vector4f(0.8491491675f, -0.0005763564259f, -0.5281325579f, -0.004616710357f) },
            MyKeyframe_Quaternion{0x80,Vector4f(-0.600031f, 0.600847f, 0.370182f, 0.376711f) },
            MyKeyframe_Quaternion{0xA0,Vector4f(-0.600031f, 0.600847f, 0.370182f, 0.376711f) },
        }
    };
    MyAnimTrackdata<MyKeyframe_Translation> translation_RightArm_defaults{
        1806956327U,
        {
            MyKeyframe_Translation{0x00,Vector3f(0.150712f, 0.000000f, 0.000000f) },
            MyKeyframe_Translation{0x30,Vector3f(0.150712f, 0.000000f, 0.000000f) },
            MyKeyframe_Translation{0x60,Vector3f(0.150712f, 0.000000f, -1.000000f) },
            MyKeyframe_Translation{0x90,Vector3f(0.150712f, 0.000000f, 1.000000f) },
            MyKeyframe_Translation{0xC0,Vector3f(0.150712f, 0.000000f, 0.000000f) },
            MyKeyframe_Translation{0xF0,Vector3f(0.150712f, 0.000000f, 0.000000f) },
        }
    };
    //MyAnimTrackdata<MyKeyframe_Quaternion> rotation_RightArm2_defaults{
    //    1918345642,
    //    {
    //        MyKeyframe_Quaternion{0x00,Vector4f(-0.000000f, 0.000000f, -0.122041f, 0.992525f) },
    //        MyKeyframe_Quaternion{0x20,Vector4f(-0.000000f, 0.000000f, -0.122041f, 0.992525f) },
    //        MyKeyframe_Quaternion{0x40,Vector4f(0, 0, 0.1989974874213242f, 0.98f) },
    //        MyKeyframe_Quaternion{0x60,Vector4f(0, 0, 0.98f, 0.1989974874213242f) },
    //        MyKeyframe_Quaternion{0x80,Vector4f(0, 0.98f, 0, 0.1989974874213242f) },
    //        MyKeyframe_Quaternion{0xA0,Vector4f(0.98f, 0, 0, 0.1989974874213242f) },
    //    }
    //};
    MyAnimTrackdata<MyKeyframe_FullblastQuaternion_0x1_u16> rotation_RightArm2_defaults{
        1918345642,
        {
            MyKeyframe_FullblastQuaternion_0x1_u16{0x00,Vector4f(-0.000000f, 0.000000f, -0.122041f, 0.992525f) },
            MyKeyframe_FullblastQuaternion_0x1_u16{0x20,Vector4f(-0.000000f, 0.000000f, -0.122041f, 0.992525f) },
            MyKeyframe_FullblastQuaternion_0x1_u16{0x40,Vector4f(0, 0, 0.1989974874213242f, 0.98f) },
            MyKeyframe_FullblastQuaternion_0x1_u16{0x60,Vector4f(0, 0, 0.98f, 0.1989974874213242f) },
            MyKeyframe_FullblastQuaternion_0x1_u16{0x80,Vector4f(0, 0.98f, 0, 0.1989974874213242f) },
            MyKeyframe_FullblastQuaternion_0x1_u16{0xA0,Vector4f(0.98f, 0, 0, 0.1989974874213242f) },
        }
    };
    MyAnimTrackdata<MyKeyframe_Translation> translation_RightArm2_defaults{
        1918345642,
        {
            MyKeyframe_Translation{0x00,Vector3f(0.296530f, 0.000000f, 0.000000f) },
            MyKeyframe_Translation{0x20,Vector3f(0.296530f, 0.000000f, 0.000000f) },
            MyKeyframe_Translation{0x40,Vector3f(0.296530f, 0.000000f, 0.000000f) },
            MyKeyframe_Translation{0x60,Vector3f(0.296530f, 0.000000f, 0.000000f) },
            MyKeyframe_Translation{0x80,Vector3f(0.296530f, 0.000000f, 0.000000f) },
            MyKeyframe_Translation{0xA0,Vector3f(0.296530f, 0.000000f, 0.000000f) },
        }
    };
    MyAnimTrackdata<MyKeyframe_Quaternion> rotation_LeftArm{
        3951233754U,
        {
            MyKeyframe_Quaternion{0x00,Vector4f(0.000000f, 0.000000f, -0.000000f, 1) },
            MyKeyframe_Quaternion{0x20,Vector4f(0.000000f, 0.000000f, 1.000000f, 0) },
            MyKeyframe_Quaternion{0x40,Vector4f(0.000000f, 1.000000f, -0.000000f, 0) },
            MyKeyframe_Quaternion{0x60,Vector4f(1.000000f, 0.000000f, -0.000000f, 0) },
            MyKeyframe_Quaternion{0x80,Vector4f(0.000000f, 0.000000f, -0.000000f, 1) },
        }
    };
    MyAnimTrackdata<MyKeyframe_Quaternion> rotation_Hips{
        3738240529U,
        {
            MyKeyframe_Quaternion{0x00,Vector4f(0.000000f, 0.000000f, -0.000000f, 1) },
            MyKeyframe_Quaternion{0x20,Vector4f(0.000000f, 0.000000f, 1.000000f, 0) },
            MyKeyframe_Quaternion{0x40,Vector4f(0.000000f, 1.000000f, -0.000000f, 0) },
            MyKeyframe_Quaternion{0x60,Vector4f(1.000000f, 0.000000f, -0.000000f, 0) },
            MyKeyframe_Quaternion{0x80,Vector4f(0.000000f, 0.000000f, -0.000000f, 1) },
        }
    };


    //SpoofAnimationRawTrackData(thisAnim, translation_RightArm2_defaults);
    //SpoofAnimationRawTrackData(thisAnim, rotation_RightArm2_defaults);
    ////SpoofAnimationRawTrackData(thisAnim, translation_wholeEntity);
    //SpoofAnimationRawTrackData(thisAnim, translation_hiddenBladeFinalSection);
    ////SpoofAnimationRawTrackData(thisAnim, rotation_RightArm_defaults);
    SpoofAnimationRawTrackData(thisAnim, translation_RightArm_defaults);
    ////SpoofAnimationRawTrackData(thisAnim, rotation_RightArm);
    ////SpoofAnimationRawTrackData(thisAnim, rotation_LeftArm);
    ////SpoofAnimationRawTrackData(thisAnim, rotation_Hips);
    ////SpoofAnimationRawTrackData(thisAnim, rotation_Reference);
    return;
    ACU::StrongRef<Skeleton> skeleton_maleBase(handle_skeleton_BaseMale);
    if (Skeleton* skel = skeleton_maleBase.GetPtr())
    {
        const uint16 startFromHipsBoneNotFromReferenceBone = 1;
        for (uint16 i = startFromHipsBoneNotFromReferenceBone; i < skel->Bones.size; i++)
        {
            Bone* bone = skel->Bones[i];
            MyAnimTrackdata<MyKeyframe_FullblastQuaternion_0x1_u16> rotation{
                bone->BoneID,
                {
                    MyKeyframe_FullblastQuaternion_0x1_u16{0x00,bone->LocalRotation },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0x30,bone->LocalRotation },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0x60,Vector4f(0, 0, 0, 1) },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0x90,Vector4f(0, 0, 0, 1) },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0xC0,bone->LocalRotation },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0xF0,bone->LocalRotation },
                }
            };
            //MyAnimTrackdata<MyKeyframe_Quaternion> rotation{
            //    bone->BoneID,
            //    {
            //        MyKeyframe_Quaternion{0x00,bone->LocalRotation },
            //        MyKeyframe_Quaternion{0x30,bone->LocalRotation },
            //        MyKeyframe_Quaternion{0x60,Vector4f(0, 0, 0, 1) },
            //        MyKeyframe_Quaternion{0x90,Vector4f(0, 0, 0, 1) },
            //        MyKeyframe_Quaternion{0xC0,bone->LocalRotation },
            //        MyKeyframe_Quaternion{0xF0,bone->LocalRotation },
            //    }
            //};
            SpoofAnimationRawTrackData(thisAnim, rotation);
            MyAnimTrackdata<MyKeyframe_Translation> translation{
                bone->BoneID,
                {
                    MyKeyframe_Translation{0x00,(Vector3f&)bone->LocalPosition },
                    MyKeyframe_Translation{0x30,(Vector3f&)bone->LocalPosition },
                    MyKeyframe_Translation{0x60,Vector3f() },
                    MyKeyframe_Translation{0x90,Vector3f() },
                    MyKeyframe_Translation{0xC0,(Vector3f&)bone->LocalPosition },
                    MyKeyframe_Translation{0xF0,(Vector3f&)bone->LocalPosition },
                }
            };
            SpoofAnimationRawTrackData(thisAnim, translation);
        }
    }
}
constexpr uint32 boneID_Reference = 743623600;
std::optional<ACU::StrongRef<Animation>> CreatePosingAnimation(const std::vector<Skeleton*>& skels)
{
    ACU::StrongRef<Animation> newAnim = g_NewAnimationsFactory.AllocateNewAnimation();
    Animation& thisAnim = *newAnim.GetPtr();

    uint8 framesLength = 0xf0;

    thisAnim.Length = (framesLength + 1) / 60.0f;

    AnimTrackData* newAnimTrackData = (AnimTrackData*)ACU::Memory::ACUAllocateBytes(sizeof(AnimTrackData), 0x10);
    std::memset(newAnimTrackData, 0, sizeof(AnimTrackData));
    AnimTrackData__ctor(newAnimTrackData);
    thisAnim.AnimTrackData_ = newAnimTrackData;

    for (Skeleton* skel : skels)
    {
        for (Bone* bone : skel->Bones)
        {
            if (bone->BoneID == boneID_Reference)
            {
                continue;
            }
            const bool isThisBoneIDTrackAlreadyAdded = std::find_if(newAnimTrackData->AnimTrackDataMapping_.begin(), newAnimTrackData->AnimTrackDataMapping_.end(), [bone](AnimTrackDataMapping& tdMapp)
                {
                    return tdMapp.TrackID == bone->BoneID;
                }) != newAnimTrackData->AnimTrackDataMapping_.end();
            if (isThisBoneIDTrackAlreadyAdded)
            {
                continue;
            }
            MyAnimTrackdata<MyKeyframe_FullblastQuaternion_0x1_u16> rotation{
                bone->BoneID,
                {
                    MyKeyframe_FullblastQuaternion_0x1_u16{0x00,bone->LocalRotation },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0x30,bone->LocalRotation },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0x60,Vector4f(0, 0, 0, 1) },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0x90,Vector4f(0, 0, 0, 1) },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0xC0,bone->LocalRotation },
                    MyKeyframe_FullblastQuaternion_0x1_u16{0xF0,bone->LocalRotation },
                }
            };
            MyAnimTrackdata<MyKeyframe_Translation> translation{
                bone->BoneID,
                {
                    MyKeyframe_Translation{0x00,(Vector3f&)bone->LocalPosition },
                    MyKeyframe_Translation{0x30,(Vector3f&)bone->LocalPosition },
                    MyKeyframe_Translation{0x60,Vector3f() },
                    MyKeyframe_Translation{0x90,Vector3f() },
                    MyKeyframe_Translation{0xC0,(Vector3f&)bone->LocalPosition },
                    MyKeyframe_Translation{0xF0,(Vector3f&)bone->LocalPosition },
                }
            };
            SpoofAnimationRawTrackData(thisAnim, translation);
            SpoofAnimationRawTrackData(thisAnim, rotation);
        }
    }
    return newAnim;
}
NewAnimationsFactory g_NewAnimationsFactory;
void DrawConfigurationPopupForSkeletonPoser(SkeletonComponent& skelCpnt)
{
    static ImGuiTextBuffer buf;
    auto FormatHandle = [&](SharedBlock& sharedBlock)
    {
        buf.clear();
        buf.appendf("%llu => %s", sharedBlock.handle, ACU::Handles::HandleToText(sharedBlock.handle));
    };
    static std::map<uint64, bool> checkboxByHandle;
    static std::vector<Skeleton*> skels;
    skels.clear();
    skels.reserve(1 + skelCpnt.AddonSkeletons.size);

    ImGui::Text("MainSkeleton:");
    {
        SharedPtrNew<Skeleton>* sharedSkel = skelCpnt.MainSkeleton;

        FormatHandle(*sharedSkel);
        bool* pIsThisSkelEnabled = &checkboxByHandle[sharedSkel->handle];
        ImGui::Checkbox(buf.c_str(), pIsThisSkelEnabled);
        if (*pIsThisSkelEnabled) { skels.push_back(sharedSkel->GetPtr()); }
    }
    ImGui::Separator();
    ImGui::Text("AddonSkeletons:");
    for (SharedPtrNew<Skeleton>* sharedSkel : skelCpnt.AddonSkeletons)
    {
        FormatHandle(*sharedSkel);
        bool* pIsThisSkelEnabled = &checkboxByHandle[sharedSkel->handle];
        ImGui::Checkbox(buf.c_str(), pIsThisSkelEnabled);
        if (*pIsThisSkelEnabled) { skels.push_back(sharedSkel->GetPtr()); }
    }
    if (ImGui::Button(
        "Create and play the \"posing animation\" for these skeletons.\n"
        "If you press \"Send to editor\", then in the Animedit tab\n"
        "you can use some rudimentary controls for the bones."
    )) {
        static ACU::StrongRef<Animation> posingAnim;
        if (std::optional<ACU::StrongRef<Animation>> recreatedAnim = CreatePosingAnimation(skels))
        {
            posingAnim = *recreatedAnim;
            g_MyAnimationPlayer.StartAnimation(posingAnim);
        }
    }
}
void DrawSpoofAnimationExperiments()
{
    uint64 spoofHandle = 31096122264; // +1 from "Game Bootstrap Settings/xx_Invalid_Animation.Animation"
    if (ImGui::Button("Start spoof anim"))
    {
        static MySpoofAnimation spoof(spoofHandle);
        g_MyAnimationPlayer.StartAnimation(spoof.GetAnim());
    }
    if (ImGui::Button("Create and start a \"posing animation\""))
    {
        ImGui::OpenPopup("SkeletonPoserPopup");
    }
    if (ImGuiCTX::Popup _skeletonPoserWindow{ "SkeletonPoserPopup" })
    {
        if (SkeletonComponent* skelCpnt = ACU::GetPlayerCpnt_SkeletonComponent())
        {
            DrawConfigurationPopupForSkeletonPoser(*skelCpnt);
        }
    }
    const char* quickTestAnimFilepath = "NewAnimations/QuickTestAnimation.anim.json";
    ImGuiTextBuffer buf;
    buf.appendf("Load \"%s\" JSON animation and start", quickTestAnimFilepath);
    if (ImGui::Button(buf.c_str()))
    {
        ACU::StrongRef<Animation> loadedAnim = g_NewAnimationsFactory.LoadNewAnimationFromFile(AbsolutePathInThisDLLDirectory(quickTestAnimFilepath));
        if (loadedAnim.GetPtr())
        {
            g_MyAnimationPlayer.StartAnimation(loadedAnim);
        }
    }
}
