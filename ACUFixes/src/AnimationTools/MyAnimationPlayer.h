#pragma once

#include "ACU/SharedPtr.h"
#include "Experimental_StrongPtr.h"
#include "ACU/Animation.h"

class MyPlayedAnimation
{
public:
    MyPlayedAnimation(const ACUSharedPtr_Strong<Animation>& animStrongRef, uint64 playStartTime);

    float GetDuration();
    bool IsPaused();
private:
    friend class MyAnimationPlayer;
    ACUSharedPtr_Strong<Animation> m_playedAnimationStrongRef;
    uint64 m_LastChangeTimestamp;
    float m_LastChangeAnimTime;
    bool m_IsPaused = false;
};

using AnimationKeyframeTime_t = uint16;

class MyAnimationPlayer
{
public:
    void DrawControls();
    void StartAnimation(ACUSharedPtr_Strong<Animation>& sharedAnim);
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
    ACUSharedPtr_Strong<Animation> LoadNewAnimationFromFile(const std::filesystem::path& filepath);
public:
    ACUSharedPtr_Strong<Animation> AllocateNewAnimation();
};
extern NewAnimationsFactory g_NewAnimationsFactory;
