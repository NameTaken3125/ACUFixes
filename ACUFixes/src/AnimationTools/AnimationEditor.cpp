#include "pch.h"

#include "AnimationTools/AnimationEditor.h"
#include "AnimationTools/MyAnimationPlayer.h"
#include "AnimationTools/AnimationPicker.h"
#include "ACU/Memory/ACUAllocs.h"

#include "vmath/vmath.h"
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/NumericAdapters.h"
#include "Serialization/ToFromFile/ToFromFile.h"

#include "ImGuiCTX.h"




enum class AnimationInformationType : uint32
{
    LocalQuaternion = 0,
    LocalTranslation = 3,
    IntegerSignal = 5,
    SingleFloat = 7,
};
assert_sizeof(AnimationInformationType, 4);



class NativeAnimationTrackdataCodec
{
public:
    // @vtbl
    virtual void Unk_000();
    virtual void Unk_008();
    virtual void Unk_010();
    virtual AnimationInformationType Unk_018_GetInformationType();
    virtual void Unk_020();
    virtual uint32 Unk_028_GetKeyframeSize_Uncompressed();
    virtual uint32 Unk_030_GetKeyframeSize_Compressed();
    virtual void Unk_038();
    virtual uint32 Unk_040_GetNumKeyframes(byte* trackdata);
    virtual void Unk_048();
    virtual void Unk_050_CompressAndSetKeyframeValues(byte* trackdata, void* keyframes, unsigned int numKeyframes);
    virtual void Unk_058();
    virtual void Unk_060();
    virtual uint16 Unk_068_GetNthKeyframeTime_mb(byte* trackdata, uint32 keyframe);
    virtual void* Unk_070_GetStartOfKeyframeValues(byte* trackdata);
    virtual void Unk_078_UncompressKeyframeValue(byte* trackdata, unsigned int keyframeIdx, void* p_out);
    virtual void Unk_080_Deserialize_mb();
    virtual byte* Unk_088_AllocateTrackdata(SmallArray<uint16>& keyframeTimes);

    // @members
    uint32 dword_8_alignMb; //0x0008
    char pad_000C[4]; //0x000C
    __m128* (__fastcall* callback_10)(NativeAnimationTrackdataCodec* a1, byte* p_trackData, __m128* p_out, __int64 p_align_mb, float* p_animTime_mb, unsigned int* p_animTimeTimes60, __int64 p_keyframeIntervalIdx);
    char pad_0018[8]; //0x0018
    uint8 informationType_mb; //0x0020
    uint8 thisStructIdxInLUT; //0x0021
    char pad_0022[14]; //0x0022
};
assert_sizeof(NativeAnimationTrackdataCodec, 0x30);
auto& GetAnimationTrackdataCodecs()
{
    return *(std::array<NativeAnimationTrackdataCodec*, 72>*)0x145137A40;
}




using AnimationKeyframeTime_t = uint16;
using ResultEditedKeyframe_t = std::optional<AnimationKeyframeTime_t>;
class AnimationInfotypeSolverBase
{
public:
    AnimationInfotypeSolverBase() {}
    virtual ~AnimationInfotypeSolverBase() {}
    virtual AnimationInformationType GetInfotype() = 0;
    virtual ResultEditedKeyframe_t DrawEditor(byte* trackdata) = 0;
    virtual json::JSON ExportKeyframesToJSON(byte* trackdata) = 0;
    virtual byte* ImportKeyframesFromJSON(json::JSON& jsonKeyframes) = 0;
};
template<AnimationInformationType Infotype>
class AnimationInfotypeSolver : public AnimationInfotypeSolverBase
{
public:
    virtual AnimationInformationType GetInfotype() override { return Infotype; }
};
class AIS_Translation : public AnimationInfotypeSolver<AnimationInformationType::LocalTranslation>
{
public:
    using UncompressedValue_t = Vector3f;

    virtual ResultEditedKeyframe_t DrawEditor(byte* trackdata) override
    {
        ImGuiCTX::PushID _id(trackdata);
        ResultEditedKeyframe_t resultEditedKeyframe;
        ImGuiTextBuffer buf;
        uint8 codecType = trackdata[0];
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];

        auto EditTranslation = [&](uint32 keyframeIdx) -> ResultEditedKeyframe_t
        {
            UncompressedValue_t editedKeyframeData;

            auto SetFirstKeyframeValuesUpToTheEditedOne = [&codec, &trackdata, &editedKeyframeData](uint32 editedKeyframeIdx)
            {
                std::vector<UncompressedValue_t> firstKFValues = [&]()
                {
                    static std::vector<UncompressedValue_t> values;
                    values.clear();
                    values.reserve(editedKeyframeIdx + 1);
                    for (uint32 i = 0; i < editedKeyframeIdx; i++)
                    {
                        UncompressedValue_t iKeyframeData;
                        codec.Unk_078_UncompressKeyframeValue(trackdata, i, &iKeyframeData);
                        values.push_back(iKeyframeData);
                    }
                    values.push_back(editedKeyframeData);
                    return values;
                }();
                codec.Unk_050_CompressAndSetKeyframeValues(trackdata, &firstKFValues[0], (uint32)firstKFValues.size());
            };
            ResultEditedKeyframe_t editedKeyframe;

            buf.clear();

            uint16 keyframeFrame = codec.Unk_068_GetNthKeyframeTime_mb(trackdata, keyframeIdx);

            float keyframeTimeFlt = keyframeFrame / 60.0f;
            buf.appendf("Frame: %d, Time: %f", keyframeFrame, keyframeTimeFlt);
            codec.Unk_078_UncompressKeyframeValue(trackdata, keyframeIdx, &editedKeyframeData);
            ImGuiCTX::PushID _id(keyframeIdx);
            if (ImGui::SliderFloat3(buf.c_str(), editedKeyframeData, -3, 3))
            {
                SetFirstKeyframeValuesUpToTheEditedOne(keyframeIdx);
                editedKeyframe = keyframeFrame;
            }
            return editedKeyframe;
        };
        uint32 numKeyframes = codec.Unk_040_GetNumKeyframes(trackdata);
        for (uint32 i = 0; i < numKeyframes; i++)
        {
            if (ResultEditedKeyframe_t edited = EditTranslation(i))
            {
                resultEditedKeyframe = edited;
            }
        }
        return resultEditedKeyframe;
    }
    virtual json::JSON ExportKeyframesToJSON(byte* trackdata) override
    {
        json::JSON jsonKeyframes = json::Array();

        uint8 codecType = trackdata[0];
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];
        uint32 numKeyframes = codec.Unk_040_GetNumKeyframes(trackdata);

        auto WriteKeyframe = [](uint16 Frame, Vector3f& LocalPosition) -> json::JSON
        {
            json::JSON jsonKF;
            WRITE_JSON_VARIABLE(jsonKF, Frame, IntegerAdapter);
            WRITE_JSON_VARIABLE(jsonKF, LocalPosition, Vector3fAdapter);
            return jsonKF;
        };
        for (uint32 i = 0; i < numKeyframes; i++)
        {
            Vector3f keyframeData;
            codec.Unk_078_UncompressKeyframeValue(trackdata, i, &keyframeData);
            uint16 keyframeTime = codec.Unk_068_GetNthKeyframeTime_mb(trackdata, i);
            jsonKeyframes.append(WriteKeyframe(keyframeTime, keyframeData));
        }
        return jsonKeyframes;
    }
    virtual byte* ImportKeyframesFromJSON(json::JSON& jsonKeyframes) override
    {
        std::vector<byte> result;

        uint16 numKeyframes = (uint16)jsonKeyframes.size();


        SmallArray<uint16> keyframeTimes;
        SmallArray<Vector3f> keyframeValues;
        void* g_currentHeap_mb = *(void**)0x1452585D0;
        ACU::Memory::SmallArrayReserve(keyframeTimes, numKeyframes, g_currentHeap_mb);
        for (uint16 i = 0; i < numKeyframes; i++)
        {
            json::JSON& jsonFrame = jsonKeyframes[i];
            uint16 Frame;
            Vector3f LocalPosition;
            READ_JSON_VARIABLE(jsonFrame, Frame, IntegerAdapter);
            READ_JSON_VARIABLE(jsonFrame, LocalPosition, Vector3fAdapter);
            ACU::Memory::SmallArrayAppend(keyframeTimes, Frame);
            ACU::Memory::SmallArrayAppend(keyframeValues, LocalPosition);
        }
        uint8 codecType = 27;
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];
        byte* newTrackdata = codec.Unk_088_AllocateTrackdata(keyframeTimes);
        codec.Unk_050_CompressAndSetKeyframeValues(newTrackdata, keyframeValues.arr, numKeyframes);
        return newTrackdata;
    }
};
// this implementation assumes normalized quaternion
// converts to Euler angles in 3-2-1 sequence
static Vector3f QuaternionToEulerAngles(Vector4f q) {
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
static Vector4f EulerToQuaternion(float roll, float pitch, float yaw) // roll (x), pitch (y), yaw (z), angles are in radians
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
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

inline constexpr float math_pi() { return float(3.1415926535897932384626433832795029); }

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
    float v_min[3] = { -math_pi(), -math_pi() / 2 + gimbalLockEps, -math_pi(), };
    float v_max[3] = { math_pi(), math_pi() / 2 - gimbalLockEps, math_pi() };
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
class AIS_Quaternion : public AnimationInfotypeSolver<AnimationInformationType::LocalQuaternion>
{
public:
    using UncompressedValue_t = __declspec(align(alignof(__m128))) Vector4f;

    virtual ResultEditedKeyframe_t DrawEditor(byte* trackdata) override
    {
        ImGuiCTX::PushID _id(trackdata);
        ResultEditedKeyframe_t resultEditedKeyframe;
        ImGuiTextBuffer buf;
        uint8 codecType = trackdata[0];
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];

        auto EditQuatEuler = [&](uint32 keyframeIdx) -> ResultEditedKeyframe_t
        {
            UncompressedValue_t editedKeyframeData;

            auto SetFirstKeyframeValuesUpToTheEditedOne = [&codec, &trackdata, &editedKeyframeData](uint32 editedKeyframeIdx)
            {
                std::vector<UncompressedValue_t> firstKFValues = [&]()
                {
                    static std::vector<UncompressedValue_t> values;
                    values.clear();
                    values.reserve(editedKeyframeIdx + 1);
                    for (uint32 i = 0; i < editedKeyframeIdx; i++)
                    {
                        UncompressedValue_t iKeyframeData;
                        codec.Unk_078_UncompressKeyframeValue(trackdata, i, &iKeyframeData);
                        values.push_back(iKeyframeData);
                    }
                    values.push_back(editedKeyframeData);
                    return values;
                }();
                codec.Unk_050_CompressAndSetKeyframeValues(trackdata, &firstKFValues[0], (uint32)firstKFValues.size());
            };
            ResultEditedKeyframe_t editedKeyframe;

            buf.clear();

            uint16 keyframeFrame = codec.Unk_068_GetNthKeyframeTime_mb(trackdata, keyframeIdx);

            float keyframeTimeFlt = keyframeFrame / 60.0f;
            buf.appendf("Frame: %d, Time: %f", keyframeFrame, keyframeTimeFlt);

            codec.Unk_078_UncompressKeyframeValue(trackdata, keyframeIdx, &editedKeyframeData);
            ImGuiCTX::PushID _id(keyframeIdx);

            Vector3f keyframeDataEuler = QuaternionToEulerAngles(editedKeyframeData);
            ImGui::Text(buf.c_str());
            ImGui::NextColumn();
            if (ImGui::SliderEulerAngles("", keyframeDataEuler))
            {
                editedKeyframeData = EulerToQuaternion(keyframeDataEuler.x, keyframeDataEuler.y, keyframeDataEuler.z);
                SetFirstKeyframeValuesUpToTheEditedOne(keyframeIdx);
                editedKeyframe = keyframeFrame;
            }
            ImGui::NextColumn();
            return editedKeyframe;
        };

        ImGui::Columns(2, "mycolumns"); // 4-ways, with border
        ImGui::Separator();
        ImGui::Text("Frame/Time"); ImGui::NextColumn();
        ImGui::Text("Data"); ImGui::NextColumn();
        ImGui::Separator();
        uint32 numKeyframes = codec.Unk_040_GetNumKeyframes(trackdata);
        for (uint32 i = 0; i < numKeyframes; i++)
        {
            if (ResultEditedKeyframe_t edited = EditQuatEuler(i))
            {
                resultEditedKeyframe = edited;
            }
        }
        ImGui::Columns(1);
        ImGui::Separator();
        return resultEditedKeyframe;
    }
    virtual json::JSON ExportKeyframesToJSON(byte* trackdata) override
    {
        json::JSON jsonKeyframes = json::Array();

        uint8 codecType = trackdata[0];
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];
        uint32 numKeyframes = codec.Unk_040_GetNumKeyframes(trackdata);


        auto WriteKeyframe = [](uint16 Frame, UncompressedValue_t& LocalRotation) -> json::JSON
        {
            json::JSON jsonKF;
            WRITE_JSON_VARIABLE(jsonKF, Frame, IntegerAdapter);
            WRITE_JSON_VARIABLE(jsonKF, LocalRotation, Vector4fAdapter);
            return jsonKF;
        };
        for (uint32 i = 0; i < numKeyframes; i++)
        {
            UncompressedValue_t keyframeData;
            codec.Unk_078_UncompressKeyframeValue(trackdata, i, &keyframeData);
            uint16 keyframeTime = codec.Unk_068_GetNthKeyframeTime_mb(trackdata, i);
            jsonKeyframes.append(WriteKeyframe(keyframeTime, keyframeData));
        }
        return jsonKeyframes;
    }
    virtual byte* ImportKeyframesFromJSON(json::JSON& jsonKeyframes) override
    {
        std::vector<byte> result;

        uint16 numKeyframes = (uint16)jsonKeyframes.size();


        SmallArray<uint16> keyframeTimes;
        SmallArray<UncompressedValue_t> keyframeValues;
        void* g_currentHeap_mb = *(void**)0x1452585D0;
        ACU::Memory::SmallArrayReserve(keyframeTimes, numKeyframes, g_currentHeap_mb);
        for (uint16 i = 0; i < numKeyframes; i++)
        {
            json::JSON& jsonFrame = jsonKeyframes[i];
            uint16 Frame;
            UncompressedValue_t LocalRotation;
            READ_JSON_VARIABLE(jsonFrame, Frame, IntegerAdapter);
            READ_JSON_VARIABLE(jsonFrame, LocalRotation, Vector4fAdapter);
            ACU::Memory::SmallArrayAppend(keyframeTimes, Frame);
            ACU::Memory::SmallArrayAppend(keyframeValues, LocalRotation);
        }
        uint8 codecType = 1;
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];
        byte* newTrackdata = codec.Unk_088_AllocateTrackdata(keyframeTimes);
        codec.Unk_050_CompressAndSetKeyframeValues(newTrackdata, keyframeValues.arr, numKeyframes);
        return newTrackdata;
    }
};
class AIS_IntegerSignal : public AnimationInfotypeSolver<AnimationInformationType::IntegerSignal>
{
public:
    using UncompressedValue_t = uint32;

    virtual ResultEditedKeyframe_t DrawEditor(byte* trackdata) override
    {
        ImGuiCTX::PushID _id(trackdata);
        ResultEditedKeyframe_t resultEditedKeyframe;
        ImGuiTextBuffer buf;
        uint8 codecType = trackdata[0];
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];

        auto EditQuatEuler = [&](uint32 keyframeIdx) -> ResultEditedKeyframe_t
        {
            UncompressedValue_t editedKeyframeData;

            auto SetFirstKeyframeValuesUpToTheEditedOne = [&codec, &trackdata, &editedKeyframeData](uint32 editedKeyframeIdx)
            {
                std::vector<UncompressedValue_t> firstKFValues = [&]()
                {
                    static std::vector<UncompressedValue_t> values;
                    values.clear();
                    values.reserve(editedKeyframeIdx + 1);
                    for (uint32 i = 0; i < editedKeyframeIdx; i++)
                    {
                        UncompressedValue_t iKeyframeData;
                        codec.Unk_078_UncompressKeyframeValue(trackdata, i, &iKeyframeData);
                        values.push_back(iKeyframeData);
                    }
                    values.push_back(editedKeyframeData);
                    return values;
                }();
                codec.Unk_050_CompressAndSetKeyframeValues(trackdata, &firstKFValues[0], (uint32)firstKFValues.size());
            };
            ResultEditedKeyframe_t editedKeyframe;

            buf.clear();

            uint16 keyframeFrame = codec.Unk_068_GetNthKeyframeTime_mb(trackdata, keyframeIdx);

            float keyframeTimeFlt = keyframeFrame / 60.0f;
            buf.appendf("Frame: %d, Time: %f", keyframeFrame, keyframeTimeFlt);

            codec.Unk_078_UncompressKeyframeValue(trackdata, keyframeIdx, &editedKeyframeData);
            ImGuiCTX::PushID _id(keyframeIdx);

            ImGui::Text(buf.c_str());
            ImGui::NextColumn();
            ImGui::Text("%llX", editedKeyframeData);
            ImGui::NextColumn();
            return editedKeyframe;
        };

        ImGui::Columns(2, "mycolumns"); // 4-ways, with border
        ImGui::Separator();
        ImGui::Text("Frame/Time"); ImGui::NextColumn();
        ImGui::Text("Data"); ImGui::NextColumn();
        ImGui::Separator();
        uint32 numKeyframes = codec.Unk_040_GetNumKeyframes(trackdata);
        for (uint32 i = 0; i < numKeyframes; i++)
        {
            if (ResultEditedKeyframe_t edited = EditQuatEuler(i))
            {
                resultEditedKeyframe = edited;
            }
        }
        ImGui::Columns(1);
        ImGui::Separator();
        return resultEditedKeyframe;
    }
    virtual json::JSON ExportKeyframesToJSON(byte* trackdata) override
    {
        json::JSON jsonKeyframes = json::Array();

        uint8 codecType = trackdata[0];
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];
        uint32 numKeyframes = codec.Unk_040_GetNumKeyframes(trackdata);


        auto WriteKeyframe = [](uint16 Frame, UncompressedValue_t& Int) -> json::JSON
        {
            json::JSON jsonKF;
            WRITE_JSON_VARIABLE(jsonKF, Frame, IntegerAdapter);
            WRITE_JSON_VARIABLE(jsonKF, Int, IntegerAdapter);
            return jsonKF;
        };
        for (uint32 i = 0; i < numKeyframes; i++)
        {
            UncompressedValue_t keyframeData;
            codec.Unk_078_UncompressKeyframeValue(trackdata, i, &keyframeData);
            uint16 keyframeTime = codec.Unk_068_GetNthKeyframeTime_mb(trackdata, i);
            jsonKeyframes.append(WriteKeyframe(keyframeTime, keyframeData));
        }
        return jsonKeyframes;
    }
    virtual byte* ImportKeyframesFromJSON(json::JSON& jsonKeyframes) override
    {
        std::vector<byte> result;

        uint16 numKeyframes = (uint16)jsonKeyframes.size();


        SmallArray<uint16> keyframeTimes;
        SmallArray<UncompressedValue_t> keyframeValues;
        void* g_currentHeap_mb = *(void**)0x1452585D0;
        ACU::Memory::SmallArrayReserve(keyframeTimes, numKeyframes, g_currentHeap_mb);
        for (uint16 i = 0; i < numKeyframes; i++)
        {
            json::JSON& jsonFrame = jsonKeyframes[i];
            uint16 Frame;
            UncompressedValue_t Int;
            READ_JSON_VARIABLE(jsonFrame, Frame, IntegerAdapter);
            READ_JSON_VARIABLE(jsonFrame, Int, IntegerAdapter);
            ACU::Memory::SmallArrayAppend(keyframeTimes, Frame);
            ACU::Memory::SmallArrayAppend(keyframeValues, Int);
        }
        uint8 codecType = 58;
        NativeAnimationTrackdataCodec& codec = *GetAnimationTrackdataCodecs()[codecType];
        byte* newTrackdata = codec.Unk_088_AllocateTrackdata(keyframeTimes);
        codec.Unk_050_CompressAndSetKeyframeValues(newTrackdata, keyframeValues.arr, numKeyframes);
        return newTrackdata;
    }
};
class AnimationSolvers2
{
public:
    AnimationInfotypeSolverBase* FindSolverForInfotype(AnimationInformationType infotype)
    {
        auto foundIt = std::find_if(m_solvers.begin(), m_solvers.end(), [&](std::unique_ptr<AnimationInfotypeSolverBase>& e)
            {
                return e->GetInfotype() == infotype;
            });
        if (foundIt == m_solvers.end())
        {
            return nullptr;
        }
        return foundIt->get();
    }
    AnimationSolvers2()
    {
        m_solvers.push_back(std::make_unique<AIS_Translation>());
        m_solvers.push_back(std::make_unique<AIS_Quaternion>());
        m_solvers.push_back(std::make_unique<AIS_IntegerSignal>());
    }
    AnimationInfotypeSolverBase* operator[](uint8 idx) { return m_solvers[idx].get(); }
public:
    std::vector<std::unique_ptr<AnimationInfotypeSolverBase>> m_solvers;
};
AnimationSolvers2 g_AnimationInfotypeSolvers;




json::JSON ExportAnimationTracksToJSON(Animation& anim)
{
    json::JSON resultTracks = json::Array();
    for (uint16 i = 0; i < anim.rawTracks.size; i++)
    {
        json::JSON track = json::Object();
        uint32 trackID = anim.AnimTrackData_->AnimTrackDataMapping_[i].TrackID;
        track["TrackID"] = std::to_string(trackID);
        byte* trackdata = (byte*)anim.rawTracks[i];
        uint8 trackdataType = (uint8)trackdata[0];
        track["codec"] = trackdataType;
        AnimationInformationType infotype = GetAnimationTrackdataCodecs()[trackdataType]->Unk_018_GetInformationType();
        track["infotype"] = (uint32)infotype;
        auto* solver = g_AnimationInfotypeSolvers.FindSolverForInfotype(infotype);
        if (solver)
        {
            track["Keyframes"] = solver->ExportKeyframesToJSON(trackdata);
        }
        resultTracks.append(std::move(track));
    }
    return resultTracks;
}
json::JSON ExportAnimationToJSON(Animation& anim)
{
    json::JSON result = json::Object();
    result["Length"] = anim.Length;
    result["Tracks"] = ExportAnimationTracksToJSON(anim);
    return result;
}

ResultEditedKeyframe_t ImGuiDrawAnimationTrackRawData_2(Animation& anim, uint16 trackdataIdx)
{
    ResultEditedKeyframe_t editedKeyframe;
    if (trackdataIdx >= anim.rawTracks.size)
    {
        ImGui::Text("Bad trackdata index.");
        return editedKeyframe;
    }
    byte* trackdata = (byte*)anim.rawTracks[trackdataIdx];
    uint8 trackdataType = trackdata[0];
    auto* codec = GetAnimationTrackdataCodecs()[trackdataType];
    if (auto* solver = g_AnimationInfotypeSolvers.FindSolverForInfotype(codec->Unk_018_GetInformationType()))
    {
        editedKeyframe = solver->DrawEditor(trackdata);
    }
    else
    {
        ImGui::Text("Unknown trackdata type.");
    }
    return editedKeyframe;
}
struct RecognizedTrackID
{
    uint32 m_TrackID;
    std::string_view m_PreferredName;
    std::string_view m_ActualReversedHashName;
};

static RecognizedTrackID g_RecognizedTracks[] = {
    // ACU_BaseSkeleton_Male.Skeleton
    {743623600, "Reference", "Reference"},
    {3738240529, "Hips", "Hips"},
    {392266736, "LeftUpLeg", "LeftUpLeg"},
    {101577729, "LeftLeg", "LeftLeg"},
    {1486391408, "LeftFoot", "LeftFoot"},
    {93034205, "LeftFootThumb1", "LeftFootThumb1"},
    {1971262097, "RightUpLeg", "RightUpLeg"},
    {2252147196, "RightLeg", "RightLeg"},
    {2601793068, "RightFoot", "RightFoot"},
    {3847670206, "RightFootThumb1", "RightFootThumb1"},
    {1393476043, "Spine", "Spine"},
    {2402941518, "Spine1", "Spine1"},
    {372288500, "Spine2", "Spine2"},
    {4214103169, "A_BloodFX", "A_BloodFX"},
    {3097015817, "LeftClavicle", "LeftClavicle"},
    {3951233754, "LeftArm", "LeftArm"},
    {2310617728, "LeftForeArm", "LeftForeArm"},
    {3061183340, "LeftHand", "LeftHand"},
    {2732186484, "LeftHandDeform1Pos", ""},
    {2132509054, "LeftHandDeform2Pos", ""},
    {2489893254, "LeftForearmDeformPos", ""},
    {3495420892, "LeftHandIndex0", ""},
    {1507990061, "LeftHandIndex1Pos", ""},
    {3236645783, "LeftHandIndex2Pos", ""},
    {3085728513, "LeftHandIndex3Pos", ""},
    {1722978338, "LeftHandMiddle0", ""},
    {1381282282, "LeftHandMiddle1Pos", ""},
    {3411927120, "LeftHandMiddle2Pos", ""},
    {3160068294, "LeftHandMiddle3Pos", ""},
    {3294869080, "LeftHandPinky0", ""},
    {3005442369, "LeftHandPinky1Pos", ""},
    {707410171, "LeftHandPinky2Pos", ""},
    {1563232365, "LeftHandPinky3Pos", ""},
    {2921927686, "LeftHandRing0", ""},
    {3825977889, "LeftHandRing1Pos", ""},
    {2097322907, "LeftHandRing2Pos", ""},
    {168143629, "LeftHandRing3Pos", ""},
    {510107344, "LeftHandIndex1Rot", "LeftHandIndex1"},
    {2272186218, "LeftHandIndex2Rot", "LeftHandIndex2"},
    {4033478652, "LeftHandIndex3Rot", "LeftHandIndex3"},
    {2434775759, "LeftHandMiddle1Rot", "LeftHandMiddle1"},
    {135719797, "LeftHandMiddle2Rot", "LeftHandMiddle2"},
    {2131876835, "LeftHandMiddle3Rot", "LeftHandMiddle3"},
    {4104580540, "LeftHandPinky1Rot", "LeftHandPinky1"},
    {1840233478, "LeftHandPinky2Rot", "LeftHandPinky2"},
    {447253648, "LeftHandPinky3Rot", "LeftHandPinky3"},
    {3544205519, "LeftHandRing1Rot", "LeftHandRing1"},
    {1246304629, "LeftHandRing2Rot", "LeftHandRing2"},
    {1028532707, "LeftHandRing3Rot", "LeftHandRing3"},
    {2029314476, "LeftHandThumb1Rot", "LeftHandThumb1"},
    {3791499286, "LeftHandThumb2Rot", "LeftHandThumb2"},
    {2532999296, "LeftHandThumb3Rot", "LeftHandThumb3"},
    {2841710851, "PistolInHandLeft", ""},
    {1064396113, "LeftHandThumb1Pos", ""},
    {2792895723, "LeftHandThumb2Pos", ""},
    {3514762365, "LeftHandThumb3Pos", ""},
    {1583624203, "LeftWristDeformPos", ""},
    {546318936, "SmthBetweenIndexAndMiddleLeft", ""},
    {1296147564, "SmthBetweenRingAndPinkyLeft", ""},
    {1067497670, "SmthAboveHandLeft", ""},
    {2149808493, "Neck", "Neck"},
    {130111906, "Head", "Head"},
    {4223024711, "ChildOfNeckSmthOnGroundLevel", ""},
    {1675137348, "RightClavicle", "RightClavicle"},
    {1806956327, "RightArm", "RightArm"},
    {1918345642, "RightForeArm", "RightForeArm"},
    {1979272496, "RightHand", "RightHand"},
    {1192371270, "RightHandDeform1Pos", ""},
    {3072924417, "RightHandDeform2Pos", ""},
    {747454002, "RightForearmDeformPos", ""},
    {1533195754, "RightHandThumb1Pos", ""},
    {3261850704, "RightHandThumb2Pos", ""},
    {3043808454, "RightHandThumb3Pos", ""},
    {2798102536, "RightWristDeformPos", ""},
    {1967653204, "SmthBetweenIndexAndMiddleRight", ""},
    {2096068236, "SmthBetweenRingAndPinkyRight", ""},
    {354721517, "SmthAboveHandRight", ""},
    {1602714039, "RightHandIndex0", ""},
    {1039256214, "RightHandIndex1Pos", ""},
    {2767756076, "RightHandIndex2Pos", ""},
    {3556748218, "RightHandIndex3Pos", ""},
    {3160275048, "RightHandMiddle0", ""},
    {243517502, "RightHandMiddle1Pos", ""},
    {2542442884, "RightHandMiddle2Pos", ""},
    {3767380242, "RightHandMiddle3Pos", ""},
    {1270033459, "RightHandPinky0", ""},
    {3610294778, "RightHandPinky1Pos", ""},
    {1312417856, "RightHandPinky2Pos", ""},
    {960420054, "RightHandPinky3Pos", ""},
    {455966938, "RightHandRing0", ""},
    {2624827704, "RightHandRing1Pos", ""},
    {91938946, "RightHandRing2Pos", ""},
    {1920847892, "RightHandRing3Pos", ""},
    {4273652147, "RightHandIndex1Rot", "RightHandIndex1"},
    {1739821065, "RightHandIndex2Rot", "RightHandIndex2"},
    {280273055, "RightHandIndex3Rot", "RightHandIndex3"},
    {1162106738, "RightHandMiddle1Rot", "RightHandMiddle1"},
    {3696035528, "RightHandMiddle2Rot", "RightHandMiddle2"},
    {2873767518, "RightHandMiddle3Rot", "RightHandMiddle3"},
    {343651039, "RightHandPinky1Rot", "RightHandPinky1"},
    {2373116773, "RightHandPinky2Rot", "RightHandPinky2"},
    {4202025971, "RightHandPinky3Rot", "RightHandPinky3"},
    {3690345925, "RightHandRing1Rot", "RightHandRing1"},
    {1124033663, "RightHandRing2Rot", "RightHandRing2"},
    {905467113, "RightHandRing3Rot", "RightHandRing3"},
    {2552872655, "RightHandThumb1Rot", "RightHandThumb1"},
    {18935669, "RightHandThumb2Rot", "RightHandThumb2"},
    {1982324707, "RightHandThumb3Rot", "RightHandThumb3"},
    {1068652261, "SwordInHandRight", "RightHand_Weapon_Ref"},
    {2841710851, "LeftHand_Weapon_Ref", "LeftHand_Weapon_Ref"},
    // AI_CHR_Inventory\\ACU_Arnaud_Pistols.Skeleton
    { 74478048, "PistolHolsterPos", "" },
    { 4140400542, "PistolHolsterRot", "A_Pistol_L_Tag" },
    { 689526397, "PistolHolsterUnknown", "A_Pistol_R_Tag" },
    // AI_CHR_Inventory\\Addon_Human_PlayerWeapon.Skeleton
    { 4238337076, "HeavyAndSpearSheathSomeKindOfControlBonePos", "" },
    { 4125497242, "HeavyAndSpearSheath0Pos", "" },
    { 813332866, "HeavyAndSpearSheath1", "" },
    { 1432036701, "HeavyAndGuillotineGunSheath", "A_HeavyWeapon_Align" },
    { 1801137822, "SpearSheath2", "A_LongWeapon_Bottom_Align" },
    // CN_U_Arno_Head_Rig.Skeleton
    { 4253370951, "Jaw", "" },
    // ACU_U_Arno_IconicWeapon.Skeleton
    {1266956304, "A_HiddenBlade_Master", "A_HiddenBlade_Master"},
    {3724594747, "WristbowHiddenBlade1", ""},
    {4248931038, "WristbowHiddenBlade2", ""},
    {3978338407, "WristbowLeftWing", ""},
    {1948905949, "WristbowRightWing", ""},
    {714219653, "WristbowLatch", ""},
    // ACU_Avatar_ForeArmTwist.Skeleton
    { 3195923239, "LeftForearm3Pos", "" },
    { 1382888217, "RightForearm3Pos", "" },
    // Addon_Human_Sword_Tag.Skeleton
    { 3734611192, "SmthAtSamePlaceAsSwordOnHipLeft", "" },
    { 3055624953, "SwordOnHipLeft1", "" },
    { 981686566, "SwordOnHipLeft2", "P_LeftWaist_Tag" },
    // ACU_Unisex_Pecks.Skeleton
    { 453451896, "LeftShoulderRot", "" },
    { 1148800888, "RightShoulderRot", "" },
    // ACU_Avatar_Hood_3.Skeleton - bones are confusing here
    { 1626292512, "HoodBackMid1Pos", "" },
    { 667031710, "HoodBackMid2Pos", "" },
    { 3702268482, "HoodLeftAndRightPos", "" },
    { 3362068404, "HoodFrontLeftAndRight", "" },
    { 838628611, "HoodLeftFront1", "" },
    { 1860261462, "HoodRightFront", "" },
    { 2615161646, "HoodLeftFront2Pos", "" },
    { 776916429, "HoodBackUpMid1Pos", "" },
    { 1797656211, "HoodBackUpMid2Pos", "" },
    { 603310818, "HoodRightMid1Pos", "" },
    { 4160822844, "HoodRightMid2", "" },
    { 1339771878, "HoodFrontTipPos", "" },
    { 2022475892, "HoodFrontTipChild", "" },
    { 1079242285, "HoodBackUpMid3Pos", "" },
    { 4157392128, "HoodBack", "" },
    { 1471676820, "HoodRightMid4Pos", "" },
    { 3643013005, "HoodRight2", "" },
    { 3467726894, "HoodLeftPos", "" },
    { 1170572361, "HoodRight1", "" },
    { 1635441672, "HoodBackMid3Pos", "" },
    { 1609898009, "HoodLeftMidPos", "" },
    { 2010381004, "HoodBackMid4Pos", "" },
    { 1016000321, "HoodRightMid3Pos", "" },
    { 1431217235, "", "" },
    { 3427268073, "", "" },
    { 3330940232, "", "" },
    { 2670960703, "", "" },
    { 3137117016, "", "" },
    { 3682933006, "", "" },
    { 3626827515, "", "" },
    { 2796098915, "", "" },
    { 2275181260, "", "" },
    { 513135478, "", "" },
    { 3338422691, "", "" },
    { 2023387723, "", "" },
    { 2777136891, "", "" },
    // ACU_FR_Avatar_Dress.Skeleton:
    { 599350769, "CoatFlap0", "" },
    { 1746322849, "CoatFlapHipLeft0", "" },
    { 1741509359, "CoatFlapHipLeft1", "" },
    { 378898525, "CoatFlapHipLeft2", "" },
    { 3907532851, "CoatFlapTipPointAtLeft", "" },
    { 1343483873, "CoatFlapKneeLeftPos", "" },
    { 1449164322, "CoatFlapHipRight0", ""},
    { 1245798057, "CoatFlapHipRight1", "" },
    { 3793044312, "CoatFlapHipRight2", "" },
    { 1222734519, "CoatFlapTipPointAtRight", "" },
    { 545536664, "CoatFlapKneeRightPos", "" },
    { 3036307237, "CoatFlapHipFrontLeft", "" },
    { 3166620502, "CoatFlapHipFrontRight", "" },
    // ACU_Unisex_ArmsSimple.Skeleton
    { 1656206146, "LeftMidShoulderControl", "" },
    { 962087949, "LeftForearm4Control", "" },
    { 1518371811, "LeftUpperShoulderControl", "" },
    { 2074741019, "RightMidShoulderControl", "" },
    { 3790520129, "RightElbow", "" },
    { 4008774044, "RightUpperShoulder1", "" },
    { 2012764198, "RightUpperShoulder2", "" },
    // ACU_Unisex_LegsRollbones.Skeleton
    { 3851367608, "LeftFrontCrotchPos", "" },
    { 1185001965, "LeftUpLegDeformPos", "" },
    { 1576199145, "RightFrontCrotchPos", "" },
    { 1491111456, "RightUpLegDeformPos", "" },
    { 721252245, "LeftLowLeg2Pos", "" },
    { 1869153567, "LeftUpLeg2Pos", "" },
    { 192599460, "RightUpLeg2Pos", "" },
    { 1431295197, "RightLowLeg2Pos", "" },

    { 403702418, "EyeRight", "" },
    { 485040451, "EyeLeft", "" },
    { 2735471887, "EyelidLeftDown", "" },
    { 4212489091, "EyelidLeftUp", "" },
    { 3754828124, "EyelidRightDown", "" },
    { 2764555285, "EyelidRightUp", "" },
    { 3124699560, "EyebrowLeftIn", "" },
    { 1853759095, "EyebrowRightIn", "" },
    { 1624427826, "A_SmallWeapon_Tag", "A_SmallWeapon_Tag" },
    { 3516996980, "P_SmallWeapon", "" },
    { 2512990213, "P_Bow_Tag", "" },
    { 2788905547, "P_Generic_Object_01", "P_Generic_Object_01" },
    { 1060242417, "P_Generic_Object_02", "P_Generic_Object_02" },
    { 1211446119, "P_Generic_Object_03", "P_Generic_Object_03" },
    { 3595675332, "P_Generic_Object_04", "P_Generic_Object_04" },
    { 2706806354, "P_Generic_Object_05", "P_Generic_Object_05" },
    { 3796064396, "I_LB_Skirt", "I_LB_Skirt" },
    { 226571186, "H_LB_Skirt", "H_LB_Skirt" },
    { 1088042174, "H_LB2_Skirt", "H_LB2_Skirt" },
    { 3297786436, "H_LF2_Skirt", "H_LF2_Skirt" },
    { 2339535765, "I_RB_Skirt", "I_RB_Skirt" },
    { 1689260203, "H_RB_Skirt", "H_RB_Skirt" },
    { 618179933, "H_RB2_Skirt", "H_RB2_Skirt" },
    { 2693964711, "H_RF2_Skirt", "H_RF2_Skirt" },
    { 2585245024, "H_LB_Flaps", "H_LB_Flaps" },
    { 4079392377, "H_RB_Flaps", "H_RB_Flaps" },
    { 306835619, "H_Backflaps", "" },
    { 4005612296, "P_Lute_Tag", "" },
    { 2347475608, "P_Crossbow", "" },
    { 332778049, "P_Musket", "" },
    { 2858492965, "P_LeftHandPistol", "" },
    { 3211630021, "A_ChainBlade", "A_ChainBlade" },
    { 2576213442, "BJ_DrumBone", "BJ_DrumBone" },
    { 322806999, "A_Knife_L_Tag", "A_Knife_L_Tag" },
    { 2887995033, "A_Musket_Bottom_Align", "A_Musket_Bottom_Align" },
    { 3912332660, "A_HeavySword_Align", "A_HeavySword_Align" },
    { 565228797, "A_Blunderbuss_Align", "A_Blunderbuss_Align" },
};
static void AppendTrackName(ImGuiTextBuffer& buf, uint32 trackID)
{
    auto foundIt = std::find_if(std::begin(g_RecognizedTracks), std::end(g_RecognizedTracks), [trackID](const RecognizedTrackID& track) {
        return track.m_TrackID == trackID;
        });

    if (foundIt != std::end(g_RecognizedTracks))
    {
        if (foundIt->m_PreferredName.size())
        {
            buf.appendf("%s (%x)", foundIt->m_PreferredName.data(), trackID);
            return;
        }
        else if (foundIt->m_ActualReversedHashName.size())
        {
            buf.appendf("%s (%x)", foundIt->m_ActualReversedHashName.data(), trackID);
            return;
        }
    }
    buf.appendf("%x", trackID);
}
ResultEditedKeyframe_t DrawAnimationEditor(Animation& anim)
{
    ResultEditedKeyframe_t resultKeyframe;
    ImGuiTextBuffer buf;
    for (uint16 i = 0; i < anim.rawTracks.size; i++)
    {
        buf.clear();
        uint32 trackID = anim.AnimTrackData_->AnimTrackDataMapping_[i].TrackID;
        byte* trackdata = (byte*)anim.rawTracks[i];
        uint16 trackdataType = (uint16)trackdata[0];
        auto* codec = GetAnimationTrackdataCodecs()[trackdataType];
        uint32 numKeyframes = codec->Unk_040_GetNumKeyframes(trackdata);
        buf.appendf(
            "%3u. TrackID: "
            , i
        );
        AppendTrackName(buf, trackID);
        buf.appendf(
            "; Codec: %u; Infotype: %u; Num keyframes: %u"
            , trackdataType
            , codec->Unk_018_GetInformationType()
            , numKeyframes
        );
        static std::vector<uint32> kftimes = []() {std::vector<uint32> v; v.reserve(256); return v; }();
        [&]()
        {
            kftimes.clear();
            for (uint32 i = 0; i < numKeyframes; i++)
            {
                kftimes.push_back(codec->Unk_068_GetNthKeyframeTime_mb(trackdata, i));
            }
        }();
        if (ImGuiCTX::TreeNode _node{ buf.c_str() })
        {
            if (ResultEditedKeyframe_t editedKeyframe = ImGuiDrawAnimationTrackRawData_2(anim, i))
            {
                resultKeyframe = editedKeyframe;
            }
        }
    }
    return resultKeyframe;
}
struct TrackID_t
{
    uint32 TrackID;
    TrackID_t(uint32 TrackID) : TrackID(TrackID) {}

    struct Adapter : public JSONAdapter<uint32, JSON::Class::String>
    {
        using BaseAdapter::BaseAdapter;
        // Serialize.
        JSON ToJSON()
        {
            return std::to_string(source);
        }
        // Deserialize.
        bool FromJSON(const JSON& obj)
        {
            if (!this->IsCorrectJSONType(obj))
            {
                return false;
            }
            std::string sourceAsStr = obj.ToString();
            try
            {
                source = std::stoul(sourceAsStr);
            }
            catch (const std::invalid_argument&)
            {
                return false;
            }
            return true;
        }
    };
};

DEFINE_GAME_FUNCTION(Animation__ctor, 0x140053270, Animation*, __fastcall, (Animation* a1));
DEFINE_GAME_FUNCTION(AnimTrackData__ctor, 0x1400530D0, AnimTrackData*, __fastcall, (AnimTrackData* a1));
void FillAnimationFromJSON(Animation& thisAnim, json::JSON& jsonAnim)
{
    float& Length = thisAnim.Length;
    READ_JSON_VARIABLE(jsonAnim, Length, NumericAdapter);

    AnimTrackData* newAnimTrackData = (AnimTrackData*)ACU::Memory::ACUAllocateBytes(sizeof(AnimTrackData), 0x10);
    std::memset(newAnimTrackData, 0, sizeof(AnimTrackData));
    AnimTrackData__ctor(newAnimTrackData);
    thisAnim.AnimTrackData_ = newAnimTrackData;

    json::JSON& jsonTracks = jsonAnim["Tracks"];
    for (json::JSON& jsTrack : jsonTracks.ArrayRange())
    {
        uint32 TrackID;
        READ_JSON_VARIABLE(jsTrack, TrackID, TrackID_t::Adapter);
        uint8 codec;
        READ_JSON_VARIABLE(jsTrack, codec, IntegerAdapter);
        AnimationInformationType infotype;
        READ_JSON_VARIABLE(jsTrack, infotype, IntegerAdapter);
        auto* solver = g_AnimationInfotypeSolvers.FindSolverForInfotype(infotype);
        if (solver)
        {
            json::JSON& jsKeyframes = jsTrack["Keyframes"];
            byte* newTrackdata = solver->ImportKeyframesFromJSON(jsKeyframes);
            ACU::Memory::SmallArrayAppend(thisAnim.AnimTrackData_->AnimTrackDataMapping_, AnimTrackDataMapping{ TrackID });
            ACU::Memory::SmallArrayAppend(thisAnim.rawTracks, (void*)newTrackdata);
        }
    }
}
ACU::StrongRef<Animation> NewAnimationsFactory::LoadNewAnimationFromFile(const fs::path& filepath)
{
    JSON jsonAnim = json::FromFile(filepath);
    ACU::StrongRef<Animation> result = AllocateNewAnimation();
    if (Animation* anim = result.GetPtr())
    {
        FillAnimationFromJSON(*anim, jsonAnim);
    }
    return result;
}
void AddNewOrReplaceAnimtrack(Animation& anim, uint32 trackID, uint8 trackdataType, byte* trackRawData)
{
    struct SearchResult
    {
        bool isPresent;
        uint16 whereItShouldBePresentOrInserted;
    };

    auto FindWhereToPutNewTrackInSortedArray = [&anim](uint32 newTrackID, uint8 newTrackdataType) -> SearchResult
    {
        auto& tracksArray = anim.AnimTrackData_->AnimTrackDataMapping_;
        for (uint16 i = 0; i < anim.AnimTrackData_->AnimTrackDataMapping_.size; i++)
        {
            // The tracks are sorted in ascending order.
            if (tracksArray[i].TrackID > newTrackID)
            {
                return { false, i };
            }
            if (tracksArray[i].TrackID < newTrackID)
            {
                continue;
            }
            uint8 datatype = ((byte*)anim.rawTracks[i])[0];
            // The trackdata types are sorted in _descending_ order.
            // At least the datatype `27` (== translation) comes before datatype `15` (== packed quaternion)
            if (datatype == newTrackdataType)
            {
                return { true, i };
            }
            if (datatype < newTrackdataType)
            {
                return { false, i };
            }
        }
        return { false, tracksArray.size };
    };
    auto [isPresent, whereItShouldBePresentOrInserted] = FindWhereToPutNewTrackInSortedArray(trackID, trackdataType);
    if (isPresent)
    {
        anim.rawTracks[whereItShouldBePresentOrInserted] = trackRawData;
    }
    else
    {
        ACU::Memory::SmallArrayInsert(anim.AnimTrackData_->AnimTrackDataMapping_, AnimTrackDataMapping{ trackID }, whereItShouldBePresentOrInserted);
        ACU::Memory::SmallArrayInsert(anim.rawTracks, (void*)trackRawData, whereItShouldBePresentOrInserted);
    }
}
void ImportAnimationTracksFromJSON_KeepOthers(Animation& anim, json::JSON& jsonAnim)
{
    json::JSON& newTracks = jsonAnim["Tracks"];
    anim.Length = (float)jsonAnim["Length"].ToFloat();
    for (json::JSON& jsonTrack : newTracks.ArrayRange())
    {
        uint32 TrackID;
        uint8 type;
        READ_JSON_VARIABLE(jsonTrack, TrackID, TrackID_t::Adapter);
        READ_JSON_VARIABLE(jsonTrack, type, IntegerAdapter);
        //AnimationDatatypeSolverBase* solver = g_AnimationDatatypeSolvers.FindSolverForDatatype(type);
        //if (!solver)
        //{
        //    continue;
        //}
        //std::vector<byte> newRawTrackdata = solver->ImportKeyframesFromJSON(jsonTrack["Keyframes"]);
        //byte* newAllocatedRawData = ACUAllocateBytes((uint32)newRawTrackdata.size(), 1);
        //std::memcpy(newAllocatedRawData, newRawTrackdata.data(), newRawTrackdata.size());
        //AddNewOrReplaceAnimtrack(anim, TrackID, type, newAllocatedRawData);
    }
}
void AnimationEditor::Draw()
{
    static AnimationPicker picker;
    picker.Draw("Pick animation to edit", m_editedAnim);
    if (Animation* anim = m_editedAnim.GetPtr())
    {
        if (ImGui::Button("Export to JSON to clipboard"))
        {
            json::JSON exported = ExportAnimationToJSON(*anim);
            ImGui::SetClipboardText(exported.dump().c_str());
        }
        if (ImGui::Button("Import from JSON from clipboard; keep nonoverridden tracks"))
        {
            json::JSON imported = json::JSON::Load(ImGui::GetClipboardText());
            if (imported.JSONType() == json::JSON::Class::Object)
            {
                ImportAnimationTracksFromJSON_KeepOthers(*anim, imported);
            }
        }
        ResultEditedKeyframe_t editedKeyframe = DrawAnimationEditor(*anim);
        if (editedKeyframe)
        {
            g_MyAnimationPlayer.OnAnimationEdited(*anim, *editedKeyframe);
        }
    }
}
AnimationEditor g_AnimationEditor;
void DrawAnimationEditor()
{
    g_AnimationEditor.Draw();
}
