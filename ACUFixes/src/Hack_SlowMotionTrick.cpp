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
    if (!world) { return 0; }
    return world->clockInWorldWithSlowmotion.overrideTimescale;
}
namespace ACU
{

float GetCurrentTime_InGameWithSlowMotion()
{
    World* world = World::GetSingleton();
    if (!world) { return 0; }
    return World::GetSingleton()->clockInWorldWithSlowmotion.GetCurrentTimeFloat();
}
float GetCurrentTime_InGameWithoutSlowMotion()
{
    World* world = World::GetSingleton();
    if (!world) { return 0; }
    return World::GetSingleton()->clockUnpausedWithoutSlowmotion.GetCurrentTimeFloat();
}
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

#include "ACU_InputUtils.h"
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
    return !g_Config.hacks->slowmotionTrick;
}

class SlowMotionTrick
{
    struct State_Nothing {};
    struct State_EaseIn
    {
        float m_TimestampStart;
        float m_InitialTimescale;
    };
    struct State_InSlowMotion
    {
        float m_TimestampStart;
        float m_InitialTimescale;
    };
    struct State_EaseOut
    {
        float m_TimestampStart;
        float m_InitialTimescale;
    };
    using State_t = std::variant<
        State_Nothing
        , State_EaseIn
        , State_InSlowMotion
        , State_EaseOut
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
        const bool isSlowmotionTrickDisabled = IsSlowMotionTrickDisabled();
        if (auto* clearMode = std::get_if<State_Nothing>(&m_CurrentState))
        {
            if (isSlowmotionTrickDisabled)
            {
                return;
            }
            if (ACU::Input::IsPressed(MouseButton::Mouse4))
            {
                PlaySoundSlowmotionStart();
                m_CurrentState = State_EaseIn{ GetCurrentTimestampIgnoreSlowmotion(), GetCurrentTimeFactor() };
            }
            return;
        }
        if (isSlowmotionTrickDisabled)
        {
            ACUSetTimescaleInGameClock(1.0f);
            m_CurrentState = State_Nothing{};
            return;
        }
        if (auto* easeIn = std::get_if<State_EaseIn>(&m_CurrentState))
        {
            const float slowmoPhaseTargetTimescale = 0.05f;
            const float easeInDuration = 0.2f;
            const float interpTee = GetTimerCompletion(easeIn->m_TimestampStart, easeInDuration, GetCurrentTimestampIgnoreSlowmotion());
            if (interpTee >= 1.0f)
            {
                ACUSetTimescaleInGameClock(slowmoPhaseTargetTimescale);
                m_CurrentState = State_InSlowMotion{ GetCurrentTimestampIgnoreSlowmotion(), GetCurrentTimeFactor() };
                return;
            }

            const float currentSmoothedTimescale = Interpolations::smoothstep_arbitrary(easeIn->m_InitialTimescale, slowmoPhaseTargetTimescale, interpTee);
            ACUSetTimescaleInGameClock(currentSmoothedTimescale);
            return;
        }
        if (auto* slowMotionMode = std::get_if<State_InSlowMotion>(&m_CurrentState))
        {
            const float slowmoDuration = 4.0f;
            const float interpTee = GetTimerCompletion(slowMotionMode->m_TimestampStart, slowmoDuration, GetCurrentTimestampIgnoreSlowmotion());
            if (interpTee >= 1.0f)
            {
                PlaySoundSlowmotionEnd();
                m_CurrentState = State_EaseOut{ GetCurrentTimestampIgnoreSlowmotion(), GetCurrentTimeFactor() };
                return;
            }
            return;
        }
        if (auto* exiting = std::get_if<State_EaseOut>(&m_CurrentState))
        {
            const float exitTargetTimescale = 1.0f;
            const float easeOutDuration = 2.3f;
            const float interpTee = GetTimerCompletion(exiting->m_TimestampStart, easeOutDuration, GetCurrentTimestampIgnoreSlowmotion());
            if (interpTee >= 1.0f)
            {
                ACUSetTimescaleInGameClock(exitTargetTimescale);
                m_CurrentState = State_Nothing{};
                return;
            }

            const float currentSmoothedTimescale = Interpolations::smoothstep_arbitrary(exiting->m_InitialTimescale, exitTargetTimescale, interpTee);
            ACUSetTimescaleInGameClock(currentSmoothedTimescale);
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
    float GetCurrentTimeFactor()
    {
        return World::GetSingleton()->clockInWorldWithSlowmotion.overrideTimescale;
    }
};
void DoSlowMotionTrick()
{
    static SlowMotionTrick
        smt;
    smt.Update();
}
void DrawSlowMotionTrickControls()
{
    ImGui::Checkbox("Enable Slow Motion Trick", &g_Config.hacks->slowmotionTrick.get());
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Press \"Mouse Button 4\" to activate a brief slowmotion sequence.\n"
            "Try doing that right _after_ something cool happens."
        );
    }
}
void DrawSlowMotionControls()
{
    World* world = World::GetSingleton();
    if (!world) { return; }
    float currentTimescale = world->clockInWorldWithSlowmotion.overrideTimescale;
    if (ImGui::SliderFloat("Timescale", &currentTimescale, 0.00f, 2.0f))
    {
        ACUSetTimescaleInGameClock(currentTimescale);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        ACUSetTimescaleInGameClock(1.0f);
    }
}
