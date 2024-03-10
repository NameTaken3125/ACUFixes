#pragma once

#include "Experimental_StrongPtr.h"
#include "ACU/Animation.h"

class AnimationEditor
{
public:
    void Draw();
    void SetAnimation(ACUSharedPtr_Strong<Animation>& newEdited) { m_editedAnim = newEdited; }
private:
    ACUSharedPtr_Strong<Animation> m_editedAnim;
};
extern AnimationEditor g_AnimationEditor;
