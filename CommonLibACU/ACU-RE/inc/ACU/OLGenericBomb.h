#pragma once

#include "OLAbstract.h"
#include "SharedPtr.h"
#include "DangerEvent.h"

#include "ManagedObject.h"
class CustomActionPack : public ManagedObject
{
public:
    char pad_0010[16]; //0x0010
}; //Size: 0x0020
assert_sizeof(CustomActionPack, 0x20);

class ConversationTemplate : public ManagedObject
{
public:
    char pad_0010[16]; //0x0010
}; //Size: 0x0020
assert_sizeof(ConversationTemplate, 0x20);

class FacialGroupContainer;

class CustomActionPackBundle
{
public:
    SharedPtrNew<CustomActionPack>* CustomActionPack_; //0x0000
    FacialGroupContainer* FacialGroupContainer_; //0x0008
    SharedPtrNew<ConversationTemplate>* ConversationTemplate_; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020
assert_sizeof(CustomActionPackBundle, 0x20);

class FX;

class OLGenericBomb : public OLAbstract
{
public:
    enum class EventSendingMode : uint32
    {
        EventSendingMode_Once = 0,
        EventSendingMode_Repeat = 1,
        EventSendingMode_RepeatImpactOnly = 2,
    };

    char pad_00A8[208]; //0x00A8
    SharedPtrNew<FX>* ExplosionFX; //0x0178
    SharedPtrNew<FX>* PlayerGlobalFX; //0x0180
    DangerEvent::SubType ImpactDangerEventType; //0x0188
    DangerEvent::SubType CloseWitnessDangerEventType; //0x018C
    DangerEvent::SubType WitnessDangerEventType; //0x0190
    DangerEvent::SubType BombEffectDissipatedDangerEventType; //0x0194
    char pad_0198[8]; //0x0198
    EventSendingMode EventSendingMode; //0x01A0
    char pad_01A4[44]; //0x01A4
    float ImpactRadius; //0x01D0
    float CloseWitnessRadius; //0x01D4
    float WitnessRadius; //0x01D8
    char pad_01DC[4]; //0x01DC
    float EventPeriod; //0x01E0
    float CleanFadeOutTime; //0x01E4
    float ExpandingTime; //0x01E8
    float EffectDurationTime; //0x01EC
    float FadingTime; //0x01F0
    float AfterExplosionTime; //0x01F4
    float AddedRandomDuration; //0x01F8
    char pad_01FC[4]; //0x01FC
    CustomActionPackBundle CustomActionPack_; //0x0200
    float PlayerDetectionRadius; //0x0220
    char pad_0224[2]; //0x0224
    uint8 ShouldBlockLineOfSight; //0x0226
    uint8 DisablePlayerLockTarget; //0x0227
    char pad_0228[72]; //0x0228
}; //Size: 0x0270
assert_sizeof(OLGenericBomb, 0x270);
