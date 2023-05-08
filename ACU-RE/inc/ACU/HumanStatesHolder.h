#pragma once

#include "basic_types.h"
#include "vmath/vmath.h"
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

class SharedPtr_mb;

class UsedDuringDisguise
{
public:
    char pad_0000[576]; //0x0000
    SharedPtr_mb* disguiseTargetEntity; //0x0240
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

class HumanStates_100
{
public:
    class HumanStates_NonstaticNode* p0; //0x0000
    class HumanStates_100_8(*arr8)[20]; //0x0008
    char pad_0010[4]; //0x0010
}; //Size: 0x0014
class Timer
{
public:
    uint64 timestampStart; //0x0000
    uint64 timestampEnd; //0x0008
    char pad_0010[8]; //0x0010
    class Clock* clock; //0x0018
    uint8 isActive_mb_20; //0x0020
    char pad_0021[7]; //0x0021
}; //Size: 0x0028
assert_sizeof(Timer, 0x28);

class UsedDuringCrouch;
class UsedDuringDisguiseParent_b_UsedDuringCrouch;
class ThoseFns_FilterElem
{
public:
    void* fn; //0x0000
    class HumanStates_NonstaticNode* nonstaticNode; //0x0008
}; //Size: 0x0010
class HumanStatesHolder_3C4_Callback
{
public:
    uint32 currentFrame_mb; //0x0000
    char pad_0004[4]; //0x0004
    SmallArray<ThoseFns_FilterElem*> arrThoseFnsElem; //0x0008
    char pad_0014[4]; //0x0014
}; //Size: 0x0018
assert_sizeof(HumanStatesHolder_3C4_Callback, 0x18);

// vtbl: 142FFD128
// I haven't really figured out what this structure or its children do.
// Hence, don't trust names like `UsedDuringDisguise` too much.
// I think it's really a root node in some HumanStateMachine node system.
class HumanStatesHolder
{
public:
    // @members
    char pad_0000[208]; //0x0000
    HumanStatesHolder_D0* humanStatesHolder_D0; //0x00D0
    char pad_00D8[40]; //0x00D8
    HumanStates_100 usedDuringLeaveCoverBySprintTowardEdge; //0x0100
    char pad_0114[8]; //0x0114
    uint32 currentFrame_mb; //0x011C
    char pad_0120[40]; //0x0120
    int32 lock_148; //0x0148
    uint32 dword_14C; //0x014C
    char pad_0150[88]; //0x0150
    uint8 byte_1A8; //0x01A8
    char pad_01A9[3]; //0x01A9
    uint32 dword_1AC; //0x01AC
    SmallArray<void*> arr_1B0; //0x01B0
    char pad_01C0[8]; //0x01C0
    HumanStates_NonstaticNode* haystackStates_mb; //0x01C8
    char pad_01D0[56]; //0x01D0
    UsedDuringCrouch* usedDuringBombQuickDrop; //0x0208
    char pad_0210[152]; //0x0210
    UsedDuringDisguiseParent_b_UsedDuringCrouch* UsedDuringDisguiseParent_b_UsedDuringCrouch_; //0x02A8
    char pad_02B0[272]; //0x02B0
    uint32 lock_3C0; //0x03C0
    SmallArray<HumanStatesHolder_3C4_Callback*> arr3C4; //0x03C4
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
    char pad_1B70[816]; //0x1B70

    // @helper_functions
    static HumanStatesHolder* GetForPlayer();
}; //Size: 0x1EA0
assert_offsetof(HumanStatesHolder, ballisticAimingCurrentEquipmentType, 0xD34);
assert_offsetof(HumanStatesHolder, aimingGuillotineGun, 0x1970);
