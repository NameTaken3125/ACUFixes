#include "pch.h"

#include "AnimationTools/AnimationEditor.h"
#include "AnimationTools/MyAnimationPlayer.h"
#include "AnimationTools/AnimationPicker.h"
#include "AnimationTools/ACUAllocs.h"

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
        SmallArrayReserve(keyframeTimes, numKeyframes, g_currentHeap_mb);
        for (uint16 i = 0; i < numKeyframes; i++)
        {
            json::JSON& jsonFrame = jsonKeyframes[i];
            uint16 Frame;
            Vector3f LocalPosition;
            READ_JSON_VARIABLE(jsonFrame, Frame, IntegerAdapter);
            READ_JSON_VARIABLE(jsonFrame, LocalPosition, Vector3fAdapter);
            SmallArrayAppend(keyframeTimes, Frame);
            SmallArrayAppend(keyframeValues, LocalPosition);
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
        SmallArrayReserve(keyframeTimes, numKeyframes, g_currentHeap_mb);
        for (uint16 i = 0; i < numKeyframes; i++)
        {
            json::JSON& jsonFrame = jsonKeyframes[i];
            uint16 Frame;
            UncompressedValue_t LocalRotation;
            READ_JSON_VARIABLE(jsonFrame, Frame, IntegerAdapter);
            READ_JSON_VARIABLE(jsonFrame, LocalRotation, Vector4fAdapter);
            SmallArrayAppend(keyframeTimes, Frame);
            SmallArrayAppend(keyframeValues, LocalRotation);
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
        SmallArrayReserve(keyframeTimes, numKeyframes, g_currentHeap_mb);
        for (uint16 i = 0; i < numKeyframes; i++)
        {
            json::JSON& jsonFrame = jsonKeyframes[i];
            uint16 Frame;
            UncompressedValue_t Int;
            READ_JSON_VARIABLE(jsonFrame, Frame, IntegerAdapter);
            READ_JSON_VARIABLE(jsonFrame, Int, IntegerAdapter);
            SmallArrayAppend(keyframeTimes, Frame);
            SmallArrayAppend(keyframeValues, Int);
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
        track["type"] = trackdataType;
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
            "%3u. TrackID: %10u; Codec: %u; Infotype: %u; Num keyframes: %u"
            , i
            , trackID
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

    AnimTrackData* newAnimTrackData = (AnimTrackData*)ACUAllocateBytes(sizeof(AnimTrackData), 0x10);
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
            SmallArrayAppend(thisAnim.AnimTrackData_->AnimTrackDataMapping_, AnimTrackDataMapping{ TrackID });
            SmallArrayAppend(thisAnim.rawTracks, (void*)newTrackdata);
        }
    }
}
ACUSharedPtr_Strong<Animation> NewAnimationsFactory::LoadNewAnimationFromFile(const fs::path& filepath)
{
    JSON jsonAnim = json::FromFile(filepath);
    ACUSharedPtr_Strong<Animation> result = AllocateNewAnimation();
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
        SmallArrayInsert(anim.AnimTrackData_->AnimTrackDataMapping_, AnimTrackDataMapping{ trackID }, whereItShouldBePresentOrInserted);
        SmallArrayInsert(anim.rawTracks, (void*)trackRawData, whereItShouldBePresentOrInserted);
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
        if (ImGui::Button("Import from JSON from clipboard; keep former nonoverridden tracks"))
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
