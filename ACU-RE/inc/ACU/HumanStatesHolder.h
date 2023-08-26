#pragma once

#include "basic_types.h"
#include "vmath/vmath.h"
#include "SharedPtr.h"
#include "SmallArray.h"
#include "Enum_EquipmentType.h"

class BallisticProjectileAimingProcess_190
{
public:
    char pad_0000[28]; //0x0000
    float nearAndMidModeDistance; //0x001C
    float angleUpDown; //0x0020
    char pad_0024[4]; //0x0024
    Vector4f vecForward_sorta_1b8; //0x0028
    float flt_38; //0x0038
    char pad_003C[44]; //0x003C
}; //Size: 0x0068

class BallisticProjectileAimingProcess_118;
class BallisticProjectileSettings;
class BallisticProjectileAimingProcess
{
public:
    char pad_0000[128]; //0x0000
    Vector4f predictionBeamOrigin; //0x0080
    char pad_0090[32]; //0x0090
    Vector4f vecForwardFlat_b0; //0x00B0
    char pad_00C0[72]; //0x00C0
    float flt_108; //0x0108
    char pad_010C[12]; //0x010C
    BallisticProjectileAimingProcess_118* stru_118; //0x0118
    char pad_0120[104]; //0x0120
    BallisticProjectileSettings** projectileSettings_complicatedPtr; //0x0188
    BallisticProjectileAimingProcess_190 currentAimingState; //0x0190
    char pad_01F8[8]; //0x01F8
}; //Size: 0x0200
assert_sizeof(BallisticProjectileAimingProcess, 0x200);

class Entity;

class UsedDuringDisguise
{
public:
    char pad_0000[576]; //0x0000
    SharedPtrNew<Entity>* disguiseTargetEntity; //0x0240
    char pad_0248[48]; //0x0248
    uint8 isInDisguise_mb; //0x0278
    char pad_0279[15]; //0x0279
};
assert_offsetof(UsedDuringDisguise, disguiseTargetEntity, 0x240);
class HumanStatesHolder_D0
{
public:
    UsedDuringDisguise* usedDuringDisguise; //0x0000
    char pad_0008[256]; //0x0008
}; //Size: 0x0108


class FancyVFunction;
// `HumanStatesHolder` also seems to be derived from this.
// I call them `Functors` because they hold the Enter() and Exit() callbacks,
// but I think they might be "states" of the HumanStateMachine node system.
class FunctorBase
{
    using FunctorEnter_t = void(__fastcall*)(FunctorBase*);
    using FunctorExit_t = void(__fastcall*)(FunctorBase*);
public:
    // @members
    char pad_0000[8]; //0x0000
    FunctorBase* directChild_mb; //0x0008
    FunctorBase* pendingDirectChild; //0x0010
    FancyVFunction* fancyVTable;
    char pad_20[4];
    uint32 dword_24;
    SmallArraySemistatic<FunctorBase*, 0x10> parentStack;
    SmallArray<FunctorBase*> someNodes_B8; //0x00B8 // I haven't seen this array being non-empty.
    SmallArray<FunctorBase*> nonoverridingChildren; //0x00C4
    SmallArray<FunctorBase*> pendingNonoverridingChildren; //0x00C4
    FunctorEnter_t Enter;
    FunctorExit_t Exit;
    char pad_00F0[16]; //0x00F0

    // @helper_functions
    template<typename ParentFunctorType>
    ParentFunctorType* GetNthParent(unsigned short N) { return (ParentFunctorType*)parentStack[N]; }
};
assert_offsetof(FunctorBase, parentStack, 0x28);
assert_offsetof(FunctorBase, nonoverridingChildren, 0xC4);
assert_offsetof(FunctorBase, Enter, 0xE0);
assert_offsetof(FunctorBase, Exit, 0xE8);
// I think this structure is actually slightly bigger than 0x100 bytes.
assert_sizeof(FunctorBase, 0x100);

class HumanStates_100_8
{
public:
    FunctorBase* node; //0x0000
    char pad_0008[16]; //0x0008
}; //Size: 0x0018
class HumanStates_100
{
public:
    FunctorBase* p0; //0x0000
    SmallArray<HumanStates_100_8> arr8; //0x0008
}; //Size: 0x0018
assert_sizeof(HumanStates_100, 0x18);
#include "Timer.h"

class AtomAnimComponent;
class Entity;
class UsedDuringCrouch;
class UsedDuringDisguiseParent_b_UsedDuringCrouch;
class UsedDuringQuickshot_40
{
public:
    uint32 dword_0; //0x0000
    uint8 byte_4_isChanged_mb; //0x0004
    char pad_0005[3]; //0x0005
    uint32 dword_8; //0x0008
    uint8 byte_C; //0x000C
    char pad_000D[3]; //0x000D
    uint32 dword_10_isPlaying_mb; //0x0010
    uint32 dword_14; //0x0014
    uint8 byte_18; //0x0018
    uint8 byte_19_isJustStarted_mb; //0x0019
    char pad_001A[2]; //0x001A
    uint32 dword_1C_startFrame_mb; //0x001C
}; //Size: 0x0020
assert_sizeof(UsedDuringQuickshot_40, 0x20);
class HumanStatesHolder;
class UsedDuringQuickshot
{
public:
    char pad_0000[32]; //0x0000
    uint64 qword_20; //0x0020
    HumanStatesHolder* humanStates; //0x0028
    Entity* player; //0x0030
    AtomAnimComponent* atomAnimCpnt; //0x0038
    SmallArray<UsedDuringQuickshot_40> animEvents_mb; //0x0040
    char pad_004C[2]; //0x004C
    uint8 byte_4E_isChanged_mb; //0x004E
    char pad_004F[1]; //0x004F
    float flt_50; //0x0050
    char pad_0054[4]; //0x0054
}; //Size: 0x0058
assert_sizeof(UsedDuringQuickshot, 0x58);
class CallbackWithReceiver
{
public:
    void* fn; //0x0000
    FunctorBase* humanStateNode; //0x0008
}; //Size: 0x0010
assert_sizeof(CallbackWithReceiver, 0x10);
/*
There is a family of functions I just refer to as "one of those".
It seems their first parameter is the "HumanStatesHolder", and the rest of parameters,
and the number of them, vary.
I believe they are all template instantiations over some large hard-to-describe enum.
Each of those functions seems to do the following:
- Lock changes to array at `HumanStatesHolder+0x3C4`;
- Get array of pairs [callback, human state node] at a hardcoded index
  (which is why I believe they're templated over an enum)
  in the aforementioned array (if necessary, update that array to the current frame);
- For each of the gathered pairs, run `callback(human state node, _rest_of_parameters_to_the_function_)`;
  (so if the signature of the function is, for example,
    `void oneOfThoseFns(HumanStatesHolder*, SharedPtr<Entity>* sharedPtrOut, Vector4f* vec)`
  , then the callbacks will have signature
    `void callback(human state node, SharedPtr<Entity>* sharedPtrOut, Vector4f* vec)`
  );
- Unlock the array at `HumanStatesHolder+0x3C4`.

These "gather callbacks" often act as filters, with signature
  `void oneOfThoseFns(HumanStatesHolder*, bool& resultOut)`,
then using the `resultOut` for flow control.
(e.g.:
  141B17260, which seems to check whether Disguise is in cooldown,
  1419D22A0, which seems to check whether Quickshot is being performed),
but also often are used to gather data, depending on the state the player character is in
(e.g.:
  1419FB1C0, which seems to get the SharedPtr<Entity>* for the current ranged weapon target entity
  (yellow highlighted). The gathered callbacks in this case are different depending on whether
  the player is on ground or sitting on a ledge (the callback presumably performs a scan for entity,
  and _can_ have a result), or sitting on a peak or is currently jumping (returns empty shared ptr)),
and often still have deep side effects
(e.g.:
  1419FAE40, whose signature is
    `__int64 __fastcall oneOfThoseFns_navigationAndAssassinationAndWhoKnowsWhatElse(HumanStatesHolder*, __int64 p_movementDirInGroundPlane, float p_isWASDMoving, unsigned int a4, __int64 p_isInHighProfile, __int64 a6)`,
    and whose gathered callbacks contain logic for the entire player parkour system
    (different large callbacks for WhileOnGround, WhileOnWall)).
The one at 14198F020 is used when player attempts to start aiming bomb (long press F),
and both acts as a filter, with its second parameter `bool& resultOut` indicating
whether Bomb Aim has been started, and as a message receiver, I guess, since its callbacks
(when Bomb Aim is allowed) do start aiming and allocate a node that lives for the duration of Aim.

So, callbacks? message receivers? processors? filters? I dunno,
but there are about a thousand and they are found everywhere if you go digging into
player mechanics.
*/
class HumanStatesHolder_3C4_BagOfCallbacks
{
public:
    uint32 frameWhenMostRecentlyUpdated_mb; //0x0000
    char pad_0004[4]; //0x0004
    SmallArray<CallbackWithReceiver> callbacksWithReceivers; //0x0008
    char pad_0014[4]; //0x0014
}; //Size: 0x0018
assert_sizeof(HumanStatesHolder_3C4_BagOfCallbacks, 0x18);

#include "SharedPtr.h"
class HumanStatesHolder_1B0
{
public:
    void* ppNode; //0x0000
    FunctorBase* pNode; //0x0008
}; //Size: 0x0010
assert_sizeof(HumanStatesHolder_1B0, 0x10);

// vtbl: 142FFD128
// I haven't really figured out what this large structure or its children do.
// Hence, don't trust names like `UsedDuringDisguise` too much.
// I think this is really a root node in some HumanStateMachine node system.
class HumanStatesHolder
{
public:
    // @members
    char pad_0000[208]; //0x0000
    HumanStatesHolder_D0* humanStatesHolder_D0; //0x00D0
    char pad_00D8[40]; //0x00D8
    HumanStates_100 usedDuringLeaveCoverBySprintTowardEdge; //0x0100
    char pad_0118[4]; //0x0118
    uint32 frameWhenMostRecentlyUpdated_mb; //0x011C
    char pad_0120[40]; //0x0120
    int32 lock_148; //0x0148
    uint32 dword_14C; //0x014C
    char pad_0150[88]; //0x0150
    uint8 byte_1A8; //0x01A8
    char pad_01A9[3]; //0x01A9
    uint32 dword_1AC; //0x01AC
    SmallArraySemistatic<HumanStatesHolder_1B0, 0x20> primaryCallbackReceivers; //0x01B0
    uint32 lock_3C0; //0x03C0
    SmallArray<HumanStatesHolder_3C4_BagOfCallbacks*> callbacksForIdx; //0x03C4
    char pad_03D0[264]; //0x03D0
    Timer timerToLeaveCoverByMovingAway; //0x04D8
    char pad_0500[2100]; //0x0500
    EquipmentType ballisticAimingCurrentEquipmentType; //0x0D34
    char pad_0D38[56]; //0x0D38
    BallisticProjectileAimingProcess aimingSmokeBomb; //0x0D70
    BallisticProjectileAimingProcess aimingMoneyPouch; //0x0F70
    BallisticProjectileAimingProcess aimingCherryBomb; //0x1170
    BallisticProjectileAimingProcess aimingPoison; //0x1370
    BallisticProjectileAimingProcess aimingDefault; //0x1570
    BallisticProjectileAimingProcess aiming_equip19_1770; //0x1770
    BallisticProjectileAimingProcess aimingGuillotineGun; //0x1970
    char pad_1B70[280]; //0x1B70
    Entity* ownerEntity; //0x1C88
    AtomAnimComponent* atomAnimCpnt; //0x1C90
    char pad_1C98[56]; //0x1C98
    UsedDuringQuickshot* hasAnimationEventsData_mb; //0x1CD0
    char pad_1CD8[288]; //0x1CD8
    SharedPtrNew<Entity>* shared_quickshotTarget_mb; //0x1DF8
    char pad_1E00[160]; //0x1E00

    // @helper_functions
    static HumanStatesHolder* GetForPlayer();
}; //Size: 0x1EA0
assert_offsetof(HumanStatesHolder, primaryCallbackReceivers, 0x1B0);
assert_offsetof(HumanStatesHolder, callbacksForIdx, 0x3C4);
assert_offsetof(HumanStatesHolder, ballisticAimingCurrentEquipmentType, 0xD34);
assert_offsetof(HumanStatesHolder, aimingGuillotineGun, 0x1970);
