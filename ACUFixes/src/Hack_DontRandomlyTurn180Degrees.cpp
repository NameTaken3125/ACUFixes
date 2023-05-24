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
    //uintptr_t whenSettingPlayerTransform = 0x1425DE360;
    //PresetScript_CCodeInTheMiddle(whenSettingPlayerTransform, 7,
    //    WhenSettingPlayerTransform_dontRandomlyFlip180, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    //uintptr_t whenSettingPlayerTransform_slightlyLessFrequent = 0x1420F7350;
    //PresetScript_CCodeInTheMiddle(whenSettingPlayerTransform_slightlyLessFrequent, 5,
    //    WhenSettingPlayerTransform_dontRandomlyFlip180, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
    uintptr_t whenSettingPlayerTransform_evenLessFrequent = 0x142608280;
    PresetScript_CCodeInTheMiddle(whenSettingPlayerTransform_evenLessFrequent, 7,
        WhenSettingPlayerTransform_evenLessFrequent_dontRandomlyFlip180, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}
