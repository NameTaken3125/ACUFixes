#pragma once

#include "ACU/ManagedPtrs/ManagedPtrs.h"
#include "ACU/Animation.h"

#include "ImGui3D/ImGui3DCustomDraw.h"

class SkeletonComponent;
class Entity;
class Animation;

class AnimationEditor;
class AnimEditor_Draw3D : public ImGui3D::CustomDraw::CustomDrawer
{
public:
    virtual void DoDraw() override;
    bool m_Enabled = false;
    float m_MarkerSize = 0.013f;

public:
    void DrawBoneIfFound(SkeletonComponent& skelCpnt, uint32 boneID);
    void DrawBonesThatAreUsedInAnimation(Entity& ent, Animation& anim);
public:
    AnimationEditor& m_AnimEditor;
    AnimEditor_Draw3D(AnimationEditor& editor) : m_AnimEditor(editor) {}
};

class AnimationEditor
{
public:
    void Draw();
    void SetAnimation(ACU::StrongRef<Animation>& newEdited) { m_editedAnim = newEdited; }


public:
    AnimEditor_Draw3D m_Draw3D;
public:
    AnimationEditor();
    ~AnimationEditor();
public:
    ACU::StrongRef<Animation> m_editedAnim;
};
extern AnimationEditor g_AnimationEditor;
