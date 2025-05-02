#include "pch.h"

#include "AnimationTools/AnimationsLog/AnimationGraphEvaluationLog.h"

#include "ACU/AtomAnimComponent.h"
#include "ACU/AtomAnimationDataBaseNode.h"
#include "ACU/ACUGetSingletons.h"
#include "ACU/Animation.h"
#include "ACU_SharedPtrs.h"

#include "Handles.h"

#include "MyLog.h"
DEFINE_LOGGER_CONSOLE(AnimationsLog, "[Anims]")


class AnimationProvider;
class AnimationCalculation_Lv2_20_HasAnimationProviders
{
public:
    char pad_0000[32]; //0x0000
    SmallArray<byte> instanceDataBuffer; //0x0020
    char pad_002C[564]; //0x002C
    SmallArray<AnimationProvider*> animProviders_mb; //0x0260
    char pad_026C[20]; //0x026C
}; //Size: 0x0280
assert_offsetof(AnimationCalculation_Lv2_20_HasAnimationProviders, instanceDataBuffer, 0x20);
assert_offsetof(AnimationCalculation_Lv2_20_HasAnimationProviders, animProviders_mb, 0x260);
class AnimationDataNodeEvaluation_Stack_a2
{
public:
    GraphEvaluation* graphEvaluation; //0x0000
    AnimationCalculation_Lv2_20_HasAnimationProviders* hasAnimProviders; //0x0008
    char pad_0010[88]; //0x0010
}; //Size: 0x0068
assert_sizeof(AnimationDataNodeEvaluation_Stack_a2, 0x68); // 140124B88

void WhenJustRetrievedAnimationForNewlyInitializedAnimationDataNode_DoLog(AllRegisters* params)
{
    AnimationDataNodeEvaluation_Stack_a2* currentEval = (AnimationDataNodeEvaluation_Stack_a2*)params->rdi_;
    if (currentEval->graphEvaluation->animCpnt->entity != ACU::GetPlayer())
    {
        return;
    }
    auto* animDataNode = (AtomAnimationDataBaseNode*)params->rbx_;
    Animation* anim = (Animation*)params->GetRAX();
    SharedPtrNew<Animation>& animSharedBlock = (SharedPtrNew<Animation>&)FindSharedBlockByObjectAndIncrementStrongRefcount(*anim);
    uint64 animHandle = animSharedBlock.handle;
    LOG_DEBUG(AnimationsLog
        , L"N:%llX A:%llX H:%llu => %s\n"
        , animDataNode
        , anim
        , animHandle
        , utf8_and_wide_string_conversion::utf8_decode(ACU::Handles::HandleToText(animHandle).c_str()).c_str()
    );
}

AnimGraphEvaluationLogging::AnimGraphEvaluationLogging()
{
    uintptr_t whenJustRetrievedAnimationForNewlyInitializedAnimationDataNode = 0x1400EA0F9;
    PresetScript_CCodeInTheMiddle(whenJustRetrievedAnimationForNewlyInitializedAnimationDataNode, 6,
        WhenJustRetrievedAnimationForNewlyInitializedAnimationDataNode_DoLog, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}
