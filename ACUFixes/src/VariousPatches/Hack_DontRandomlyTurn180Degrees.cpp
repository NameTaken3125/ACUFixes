#include "pch.h"

#include "Hack_DontRandomlyTurn180Degrees.h"

#include "ACU/Entity.h"
#include "ACU/ACUGetSingletons.h"

inline bool IsBasicallyNegated(float x, float y)
{
    return fabs(x + y) < 0.01f;
}
inline bool Is180DegreesFlip(const Matrix4f& m1, const Matrix4f& m2)
{
    return IsBasicallyNegated(m1.at(0, 0), m2.at(0, 0))
        && IsBasicallyNegated(m1.at(0, 1), m2.at(0, 1))
        && IsBasicallyNegated(m1.at(1, 0), m2.at(1, 0))
        && IsBasicallyNegated(m1.at(1, 1), m2.at(1, 1));
}
void FixRandomlyFlippedTransform(Matrix4f& prevTransform, Matrix4f& newTransform)
{
    newTransform.at(0, 0) *= -1;
    newTransform.at(0, 1) *= -1;
    newTransform.at(1, 0) *= -1;
    newTransform.at(1, 1) *= -1;
}

void WhenSettingPlayerTransform_dontRandomlyFlip180(AllRegisters* params)
{
    Entity* entity = (Entity*)(params->rcx_);
    if (entity != ACU::GetPlayer())
    {
        return;
    }
    Matrix4f* newTransform = (Matrix4f*)params->rdx_;
    if (Is180DegreesFlip(*newTransform, entity->GetTransform()))
    {
        FixRandomlyFlippedTransform(entity->GetTransform(), *newTransform);
    }
}
void WhenSettingPlayerTransform_evenLessFrequent_dontRandomlyFlip180(AllRegisters* params)
{
    Entity* entity = *(Entity**)(params->rcx_ + 0x10);
    if (entity != ACU::GetPlayer())
    {
        return;
    }
    Matrix4f* newTransform = (Matrix4f*)params->rdx_;
    if (Is180DegreesFlip(*newTransform, entity->GetTransform()))
    {
        FixRandomlyFlippedTransform(entity->GetTransform(), *newTransform);
    }
}
DontRandomlyTurn180Degrees::DontRandomlyTurn180Degrees()
{
    {
        /*
        This is a hook at `BaseEntity::SetTransform()`.
        Triggers very often, thousands of times per second, I guess.
        I do not detect any FPS drop from placing this hook, to be fair.
        */
        //uintptr_t whenSettingPlayerTransform = 0x1425DE360;
        //PresetScript_CCodeInTheMiddle(whenSettingPlayerTransform, 7,
        //    WhenSettingPlayerTransform_dontRandomlyFlip180, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    }
    {
        /*
        This one still triggers more often than I'd like,
        but less frequently, and the results seem to be the same.
        */
        uintptr_t whenSettingPlayerTransform_slightlyLessFrequent = 0x1420F7350;
        PresetScript_CCodeInTheMiddle(whenSettingPlayerTransform_slightlyLessFrequent, 5,
            WhenSettingPlayerTransform_dontRandomlyFlip180, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    }
    {
        /*
        This one is located at the level of `BipedComponent`, and triggers much less often
        then the previous two. It is still good enough to eliminate the _consistent_ "turnaround"
        (when you are crouching and take exactly 1 (unfinished) step with the right foot),
        but sadly there were still some missed "turnarounds".
        */
        //uintptr_t whenSettingPlayerTransform_evenLessFrequent = 0x142608280;
        //PresetScript_CCodeInTheMiddle(whenSettingPlayerTransform_evenLessFrequent, 7,
        //    WhenSettingPlayerTransform_evenLessFrequent_dontRandomlyFlip180, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    }
}
