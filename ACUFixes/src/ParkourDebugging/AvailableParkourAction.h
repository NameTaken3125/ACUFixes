#pragma once

#include "vmath/vmath.h"

#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "ACU/SharedPtr.h"

#include "ParkourDebugging/EnumParkourAction.h"
#include "ParkourDebugging/FancyVFunctionDescription.h"

class Entity;
class EntityGroup;

// Macro used as an annotation that this name is the actual original name of class member/function,
// confirmed because its CRC32 hash is found *somewhere nearby*.
// For example the "fancy vtables" contain the function name hashes next to the function pointers etc.
// ACViewer has a ton of correctly reversed member names,
// I better mark those names with this macro at some point.
#define CONFIRMED_NAME(name) name

struct ParkourAction_FancyVTable
{
    std::array<FancyVFunction, 0x78> fancyVirtuals;
};

struct PlayerRefInParkourAction
{
    SharedPtrNew<Entity>* shared_player;
    uint64 qword_8;
    uint32 dword_10;
    char pad_14[4];
};
assert_sizeof(PlayerRefInParkourAction, 0x18);
class AvailableParkourAction
{
public:
    // @vtbl
    virtual void Unk000();
    virtual void Unk008_Destroy(char a2);

    ParkourAction_FancyVTable* fancyVTable;

    // @members

    // If climbing wall, this is where your hands were before the move started. If on beam, this is where your feet were.
    // If vaulting, this is where your feet start.
    Vector4f locationAnchorSrc; //0x0010
    Vector4f direction_20; //0x0020
    // If climbing wall, this is where your hands grab after finishing the move. If on beam, this is where your feet land.
    // If vaulting, this is where you grab to begin the vault, not where you land.
    Vector4f locationAnchorDest; //0x0030
    Vector4f direction_40; //0x0040
    Vector4f handsLocationTo_right_mb; //0x0050
    Vector4f handsLocationTo_left_mb; //0x0060
    Vector4f dir70; //0x0070
    Vector4f dir80; //0x0080
    Vector4f directionFromTo; //0x0090
    Vector4f dirA0; //0x00A0
    char pad_00B0[16]; //0x00B0
    float heightDifference; //0x00C0
    float horizontalDifference; //0x00C4
    float distance; //0x00C8
    char pad_00CC[16]; //0x00CC
    uint32 dword_DC; //0x00DC
    char pad_00E0[8]; //0x00E0
    SharedPtrNew<EntityGroup>* shared_buildingEntityGroup_mb; //0x00E8
    char pad_00F0[16]; //0x00F0
    PlayerRefInParkourAction shared_player; //0x0100

    // @helper_functions
    EnumParkourAction GetEnumParkourAction();
};
assert_offsetof(AvailableParkourAction, locationAnchorDest, 0x30);
assert_offsetof(AvailableParkourAction, shared_player, 0x100);

// FancyVFunctions in `ParkourAction_EnterWindow.fancyVtable8`:
namespace ParkourActionKnownFancyVFuncs
{
#define DEFINE_FANCY_VF(idx, name, nameHashed, fnType) using name = FancyVFunctionDescription<fnType, idx>
    DEFINE_FANCY_VF(0x4, GetPosition, 0xB2C2D737, __m128* (*)(__m128* positionOut, AvailableParkourAction*));
    DEFINE_FANCY_VF(0x18, CONFIRMED_NAME(GetDistance), 0x8EB3C7D3, float (*)(AvailableParkourAction*));
    DEFINE_FANCY_VF(0x23, GetFitness, 0x7A9E7BE6, float (*)(AvailableParkourAction*));
    DEFINE_FANCY_VF(0x3D, Set2FloatsAfterCreation, 0xB6319102, float (*)(AvailableParkourAction*, float a2, float p_epsilon_mb));
    DEFINE_FANCY_VF(0x45, GetEnumParkourAction, 0x986EB60D, EnumParkourAction(*)(AvailableParkourAction*));
    DEFINE_FANCY_VF(0x4B, InitialTestIfActionFits, 0x3DD63101, bool (*)(AvailableParkourAction*, __int64 a2, __m128* a3, __m128* a4, __int64 a5, __int64 a6, __int64 a7, Entity* p_player, __int64 a9));
    // See 1401AD4E0: Multiple RTCP variables (like the "scalar TargetDistance; // 0x97a0d819/2543900697")
    // are being set.
    DEFINE_FANCY_VF(0x54, GetTargetDistance, 0x349D0508, float (*)(AvailableParkourAction*, Entity*));
    DEFINE_FANCY_VF(0x55, GetTargetDistanceX, 0x3C4F0070, float (*)(AvailableParkourAction*, Entity*));
    DEFINE_FANCY_VF(0x56, GetEdgeDistanceX, 0x92F471C3, float (*)(AvailableParkourAction*, Entity*));
    DEFINE_FANCY_VF(0x57, GetTargetHeight, 0xD5641CA6, float (*)(AvailableParkourAction*, Entity*));
    // Index 0x57 again. I don't have the correct reversed-hash name of the function, but its return value
    // was used to set both "TargetHeight" and "LandingHeight".
    DEFINE_FANCY_VF(0x57, GetLandingHeight, 0xD5641CA6, float (*)(AvailableParkourAction*, Entity*));
    DEFINE_FANCY_VF(0x58, GetTargetAngle, 0x77BC6A50, float (*)(AvailableParkourAction*, Entity*));
    DEFINE_FANCY_VF(0x5A, GetTopLedgeDistance, 0xD009E05C, float (*)(AvailableParkourAction*, Entity*));

#undef DEFINE_FANCY_VF
}
