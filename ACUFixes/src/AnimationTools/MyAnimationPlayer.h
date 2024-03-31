#pragma once

#include "ACU/SharedPtr.h"
#include "Experimental_StrongPtr.h"
#include "ACU/Animation.h"

struct AnimState_Playing
{
    float m_startingAnimtime = 0;
    uint64 m_startingAnimatorTimestamp = 0;
};
struct AnimState_Paused
{
    float m_pausedAtAnimTime = 0;
};
using AnimState_t = std::variant<
    AnimState_Playing
    , AnimState_Paused
>;
class MyPlayedAnimation
{
public:
    MyPlayedAnimation(const ACUSharedPtr_Strong<Animation>& animStrongRef, uint64 playStartTime);

    float GetDuration();
    bool IsPaused();
private:
    friend class MyAnimationPlayer;
    ACUSharedPtr_Strong<Animation> m_playedAnimationStrongRef;
    AnimState_t m_currentState;
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
    float CalculateAnimTime(MyPlayedAnimation& anim);
private:
    void DrawTimeSlider();
private:
    std::optional<MyPlayedAnimation> m_playedAnim;
};
extern MyAnimationPlayer g_MyAnimationPlayer;




#include "Experimental_StrongPtr.h"
constexpr uint64 handle_MaximumInACU = 272054823035; // => LargeMesh_LV_Ground_07A_LOD0_Chunk_0047_MeshShape/LargeMesh_LV_Ground_07A_LOD0_Chunk_0047_MeshShape.MeshShape
class NewAnimationsFactory
{
public:
    ACUSharedPtr_Strong<Animation> LoadNewAnimationFromFile(const std::filesystem::path& filepath);
public:
    ACUSharedPtr_Strong<Animation> AllocateNewAnimation();
private:
    uint64 m_NextFreeHandle = handle_MaximumInACU + 1;
};
extern NewAnimationsFactory g_NewAnimationsFactory;
