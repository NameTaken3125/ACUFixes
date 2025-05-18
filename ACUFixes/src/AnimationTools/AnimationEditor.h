#pragma once

#include "ACU/ManagedPtrs/ManagedPtrs.h"
#include "ACU/Animation.h"

class AnimationEditor
{
public:
    void Draw();
    void SetAnimation(ACU::StrongRef<Animation>& newEdited) { m_editedAnim = newEdited; }
private:
    ACU::StrongRef<Animation> m_editedAnim;
};
extern AnimationEditor g_AnimationEditor;
