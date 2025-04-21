#include "pch.h"

#include "ACU/World.h"
#include "ACU/Clock.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU_DefineNativeFunction.h"

DEFINE_GAME_FUNCTION(World__SetUnpausedGameTimescale_onSlowMotion, 0x141D5E210, void, __fastcall, (World* world, float newTimescale));
void ACUSetTimescaleInGameClock(float newTimescale)
{
    World* world = World::GetSingleton();
    if (!world) { return; }
    World__SetUnpausedGameTimescale_onSlowMotion(world, newTimescale);
}
float ACUGetTimescaleInGameClock()
{
    World* world = World::GetSingleton();
    if (!world) { return 1.0f; }
    return world->clockInWorldWithSlowmotion.overrideTimescale;
}
class SlowMotionRawControls
{
public:
    void SetTimescale(float newTimescale) { ACUSetTimescaleInGameClock(newTimescale); }
    float GetTimescale() { return ACUGetTimescaleInGameClock(); }
};
SlowMotionRawControls g_GameSlowmotionRawControls;
namespace ACU
{
Clock* GetClock_InGameWithoutSlowmotion()
{
    World* world = World::GetSingleton();
    if (!world)
    {
        return nullptr;
    }
    return &world->clockUnpausedWithoutSlowmotion;
}

} // namespace ACU
namespace Interpolations {

float smoothstep_arbitrary(float from, float to, float xinRange01);
float smoothstep_reverse_arbitrary(float fromY, float toY, float currentY);

}
float GetTimerCompletion(float startTime, float duration, float currentTime)
{
    return (currentTime - startTime) / duration;
}
float GetTimerCompletion(float duration, float timeSinceStart)
{
    return timeSinceStart / duration;
}
struct TimecurveEvalResult
{
    float newTimescale;
    bool isFinished;
};
struct PlayedTimecurve_SmoothTransition
{
    float m_Duration;
    float m_InitialTimescale;
    float m_TargetTimescale;
    TimecurveEvalResult Evaluate(float timeSinceStart)
    {
        const float interpTee = GetTimerCompletion(m_Duration, timeSinceStart);
        if (interpTee >= 1.0f)
        {
            return { m_TargetTimescale, true };
        }
        const float currentSmoothedTimescale = Interpolations::smoothstep_arbitrary(
            m_InitialTimescale,
            m_TargetTimescale,
            interpTee);
        return { currentSmoothedTimescale, false };
    }
    PlayedTimecurve_SmoothTransition(
        float duration,
        float initialTimescale,
        float targetTimescale)
        : m_Duration(duration)
        , m_InitialTimescale(initialTimescale)
        , m_TargetTimescale(targetTimescale)
    {}
};
class SlowMotionManager
{
    struct TimecurvePlaybackData
    {
        PlayedTimecurve_SmoothTransition m_Timecurve;

        std::reference_wrapper<Clock> m_ReferenceClock;
        float m_InitialTimestamp;
    };
    std::optional<TimecurvePlaybackData> m_CurrentTimecurve;
public:
    void PlayTimecurve_SmoothTransition(float targetTimescale, float easeInDuration)
    {
        Clock* currentClock = ACU::GetClock_InGameWithoutSlowmotion();
        if (!currentClock) { return; }
        m_CurrentTimecurve = {
            PlayedTimecurve_SmoothTransition(
            easeInDuration,
            GetCurrentTimeFactor(),
            targetTimescale),
            *currentClock,
            currentClock->GetCurrentTimeFloat(),
        };
    }
    void ResetTimescale()
    {
        g_GameSlowmotionRawControls.SetTimescale(1.0f);
        m_CurrentTimecurve.reset();
    }
private:
    friend void DoSlowMotionTrick();
    void Update()
    {
        if (!m_CurrentTimecurve)
        {
            return;
        }
        Clock* currentClock = ACU::GetClock_InGameWithoutSlowmotion();
        if (!currentClock || currentClock != &m_CurrentTimecurve->m_ReferenceClock.get())
        {
            m_CurrentTimecurve.reset();
            return;
        }
        const float currentTimestamp = currentClock->GetCurrentTimeFloat();
        const float timeSinceStart = currentTimestamp - m_CurrentTimecurve->m_InitialTimestamp;
        TimecurveEvalResult evalResult = m_CurrentTimecurve->m_Timecurve.Evaluate(timeSinceStart);
        g_GameSlowmotionRawControls.SetTimescale(evalResult.newTimescale);
        if (evalResult.isFinished)
        {
            m_CurrentTimecurve.reset();
        }
    }
    float GetCurrentTimeFactor()
    {
        return g_GameSlowmotionRawControls.GetTimescale();
    }
};
SlowMotionManager g_SlowmotionManager;

#include "Common_Plugins/ACU_InputUtils.h"
#include "ACU_SoundUtils.h"
#include "ACU/SoundEvent.h"
DEFINE_GAME_FUNCTION(SoundInstance__ctor_0, 0x141C79760, SoundInstance*, __fastcall, (SoundInstance* placeAt));
void PlaySoundFromPlayer(SoundID soundID)
{
    Entity* player = ACU::GetPlayer();
    if (!player) { return; }
    std::array<unsigned char, sizeof(SoundInstance)> newSoundInstBuf = { 0 };
    SoundInstance* newSoundInst = (SoundInstance*)&newSoundInstBuf;
    SoundInstance__ctor_0(newSoundInst);
    newSoundInst->soundEvn.soundID = soundID;
    ACU::Sound::PlaySoundFromEntity(*newSoundInst, *player);
}
constexpr SoundID soundID_SlowMotionStart{ 0x5FD589BD };
constexpr SoundID soundID_SlowMotionEnd{ 0xB3BEE1 };
void PlaySoundSlowmotionStart()
{
    PlaySoundFromPlayer(soundID_SlowMotionStart);
}
void PlaySoundSlowmotionEnd()
{
    PlaySoundFromPlayer(soundID_SlowMotionEnd);
}

#include "MainConfig.h"
static bool IsSlowMotionTrickDisabled()
{
    return !g_Config.hacks->slowmotionTrick->isActive;
}


class SlowMotionTrick
{
    struct State_Nothing {};
    struct State_Activated
    {
        float m_TimestampStart;
    };
    using State_t = std::variant<
        State_Nothing
        , State_Activated
    >;
    State_t m_CurrentState;
public:
    void Update()
    {
        if (IsClockChanged_NeedToReset())
        {
            m_CurrentState = State_Nothing{};
            return;
        }

        constexpr float easeInDuration = 0.2f;
        constexpr float slowmoPhaseTargetTimescale = 0.05f;
        constexpr float slowmoDuration = 4.0f;
        constexpr float easeOutDuration = 2.3f;

        const bool isSlowmotionTrickDisabled = IsSlowMotionTrickDisabled();

        if (auto* clearMode = std::get_if<State_Nothing>(&m_CurrentState))
        {
            if (isSlowmotionTrickDisabled)
            {
                return;
            }
            if (ACU::Input::IsPressed(g_Config.hacks->slowmotionTrick->hotkey))
            {
                PlaySoundSlowmotionStart();
                g_SlowmotionManager.PlayTimecurve_SmoothTransition(slowmoPhaseTargetTimescale, easeInDuration);

                m_CurrentState = State_Activated{ GetCurrentTimestampIgnoreSlowmotion() };
            }
            return;
        }
        if (isSlowmotionTrickDisabled)
        {
            g_SlowmotionManager.ResetTimescale();
            m_CurrentState = State_Nothing{};
            return;
        }
        if (auto* slowMotionMode = std::get_if<State_Activated>(&m_CurrentState))
        {
            constexpr float howLongToWaitBeforeDeactivate = easeInDuration + slowmoDuration;
            const float interpTee = GetTimerCompletion(slowMotionMode->m_TimestampStart, howLongToWaitBeforeDeactivate, GetCurrentTimestampIgnoreSlowmotion());
            if (interpTee >= 1.0f)
            {
                PlaySoundSlowmotionEnd();
                g_SlowmotionManager.PlayTimecurve_SmoothTransition(1.0f, easeOutDuration);
                m_CurrentState = State_Nothing();
                return;
            }
            return;
        }
    }
private:
    Clock* m_InGameClockWithoutSlowmotion = nullptr;
    bool IsClockChanged_NeedToReset()
    {
        Clock* currentClock = ACU::GetClock_InGameWithoutSlowmotion();
        if (currentClock == m_InGameClockWithoutSlowmotion && currentClock != nullptr)
        {
            return false;
        }
        m_InGameClockWithoutSlowmotion = currentClock;
        return true;
    }
    float GetCurrentTimestampIgnoreSlowmotion()
    {
        return m_InGameClockWithoutSlowmotion->GetCurrentTimeFloat();
    }
};
void DoSlowMotionTrick()
{
    static SlowMotionTrick
        smt;
    smt.Update();
    g_SlowmotionManager.Update();
}
#include "ImGuiCTX.h"
#include "ImGuiConfigUtils.h"
void DrawSlowMotionTrickControls()
{
    ImGui::Checkbox("Enable Slow Motion Trick", &g_Config.hacks->slowmotionTrick->isActive.get());
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Press a hotkey (default \"Mouse Button 4\") to activate a brief slowmotion sequence.\n"
            "Try doing that right _after_ something cool happens."
        );
    }
    if (g_Config.hacks->slowmotionTrick->isActive)
    {
        ImGuiCTX::Indent _ind;
        ImGui::DrawEnumPicker("Slow Motion Trick hotkey", g_Config.hacks->slowmotionTrick->hotkey.get(), ImGuiComboFlags_HeightLarge);
    }
}
void DrawSlowMotionControls()
{
    World* world = World::GetSingleton();
    if (!world) { return; }
    float currentTimescale = g_GameSlowmotionRawControls.GetTimescale();
    if (ImGui::SliderFloat("Timescale", &currentTimescale, 0.00f, 2.0f))
    {
        g_GameSlowmotionRawControls.SetTimescale(currentTimescale);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        g_GameSlowmotionRawControls.SetTimescale(1.0f);
    }
}
