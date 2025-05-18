#pragma once

#include "ACU/ManagedPtrs/ManagedPtrs.h"
#include "ACU/Animation.h"

class MyPlayedAnimation
{
public:
    MyPlayedAnimation(const ACU::StrongRef<Animation>& animStrongRef, uint64 playStartTime);

    float GetDuration();
    bool IsPaused();
private:
    friend class MyAnimationPlayer;
    ACU::StrongRef<Animation> m_playedAnimationStrongRef;
    uint64 m_LastChangeTimestamp;
    float m_LastChangeAnimTime;
    bool m_IsPaused = false;
};

using AnimationKeyframeTime_t = uint16;

class MyAnimationPlayer
{
public:
    void DrawControls();
    void StartAnimation(ACU::StrongRef<Animation>& sharedAnim);
    uint64 GetAnimatorTime();
    void UpdateAnimations();
    void OnAnimationEdited(Animation& anim, AnimationKeyframeTime_t animTime);

    void Pause(MyPlayedAnimation& anim);
    void Unpause(MyPlayedAnimation& anim);
    void PauseAt(MyPlayedAnimation& anim, float animTime);

    bool m_isLooping = true;
    float m_speedMult = 1.0f;

private:
    float CalculateAnimTime(MyPlayedAnimation& anim, uint64 animatorTime);
private:
    void DrawTimeSlider();
private:
    std::optional<MyPlayedAnimation> m_playedAnim;
};
extern MyAnimationPlayer g_MyAnimationPlayer;



#include "NewHandlesFactory.h"

class NewAnimationsFactory
{
public:
    ACU::StrongRef<Animation> LoadNewAnimationFromFile(const std::filesystem::path& filepath);
public:
    ACU::StrongRef<Animation> AllocateNewAnimation();
};
extern NewAnimationsFactory g_NewAnimationsFactory;
