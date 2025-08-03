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
    Vector4f directionSrcToDestXYPlane; //0x0070
    Vector4f dir80; //0x0080
    Vector4f directionSrcToDest; //0x0090
    Vector4f directionSrcToDestXYPlane_A0; //0x00A0
    Vector4f locationTopLedge; //0x00B0 Z==infinity if there's no ledge above Arno
    float heightDifference; //0x00C0
    float horizontalDifference; //0x00C4
    float distance; //0x00C8
    float flt_CC; //0x00CC
    float flt_D0; //0x00D0
    float flt_D4; //0x00D4
    float flt_D8; //0x00D8
    uint32 dword_DC; //0x00DC
    char pad_00E0[8]; //0x00E0
    SharedPtrNew<Entity>* shared_buildingEntity; //0x00E8
    char pad_00F0[16]; //0x00F0
    PlayerRefInParkourAction playerRef; //0x0100
    char pad_0118[104]; //0x0118
    Vector4f vec180; //0x0180
    Vector4f vec190; //0x0190
    Vector4f vec1A0; //0x01A0
    char pad_01B0[8]; //0x01B0
    float flt_1B8; //0x01B8
    float flt_1BC; //0x01BC
    char pad_01C0[8]; //0x01C0
    float flt_1C8; //0x01C8
    char pad_01CC[4]; //0x01CC
    float heightDifferenceMin; //0x01D0
    float heightDifferenceMax; //0x01D4
    float horizontalDifferenceMin; //0x01D8
    float horizontalDifferenceMax; //0x01DC
    float distanceMin; //0x01E0
    float distanceMax; //0x01E4
    float minCosAngle; //0x01E8
    float rangeForFltD0_max; //0x01EC
    float rangeForFltD0_min; //0x01F0
    float minForFloatD4; //0x01F4
    float flt_1F8; //0x01F8
    float epsilonForDistanceRange; //0x01FC
    float epsilonForDotProd_mb; //0x0200
    float fitness; //0x0204
    char pad_0208[8]; //0x0208
    float expectedDurationMin_mb; //0x0210
    float expectedHeightDiff_mb; //0x0214
    float expectedHorizontalSpeed; //0x0218
    float expectedVerticalDefaultDisplace; //0x021C
    float expectedVerticalSpeed_mb; //0x0220
    float expectedDurationMaxExtension_mb; //0x0224
    float flt_228; //0x0228
    float expectedCurveRangeMin; //0x022C
    float curveAllowedRangeMax; //0x0230
    float curveAllowedRangeMin; //0x0234
    float Side_min; //0x0238 expectedHorizontalDiffMin
    float Side_max; //0x023C expectedHorizontalDiffMax
    Vector4f vec240; //0x0240
    Vector4f directionSrcToDestXYPlane_250; //0x0250
    Vector4f dir_260; //0x0260
    char pad_0270[32]; //0x0270

    // @helper_functions
    EnumParkourAction GetEnumParkourAction();
};
assert_offsetof(AvailableParkourAction, locationAnchorDest, 0x30);
assert_offsetof(AvailableParkourAction, playerRef, 0x100);
assert_offsetof(AvailableParkourAction, fitness, 0x204);

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
    DEFINE_FANCY_VF(0x64, SetExpectedDistanceRangesAccordingToParkourMode, 0xF914A46F, void (*)(AvailableParkourAction*, __int64 a2, int p_parkourMode));
    DEFINE_FANCY_VF(0x69, CalculateFitness, 0x872EACC8, void (*)(AvailableParkourAction*, __m128* rdx0, __int64 a3, __m128* a4, __int64 a5, __int64 a6, unsigned int p_parkourMode, Entity* p_player));
    DEFINE_FANCY_VF(0x6C, FancyVFunc_0x6C, 0x9381A468, __m128* (*)(__m128* p_totalExpectedDisplacementWithVelocityAndAcceleration_out, AvailableParkourAction*, float p_expectedDuration));
    DEFINE_FANCY_VF(0x6E, FancyVFunc_0x6E, 0x1828E7B7, __m128* (*)(__m128* p_totalExpectedDisplacementWithVelocityAndAcceleration_out, AvailableParkourAction*, float p_expectedDuration));
    DEFINE_FANCY_VF(0x70, QuickCheckAcceptableDistanceRanges, 0x228CB41C, bool (*)(AvailableParkourAction*, int p_parkourMode));
    DEFINE_FANCY_VF(0x72, QuickCheckAcceptableTargetObjectType_mb, 0xB7D5270B, bool (*)(AvailableParkourAction*, __int64 p_parkourProbe));
    DEFINE_FANCY_VF(0x73, QuickCheckAcceptableParkourMode, 0x081284CA, bool (*)(AvailableParkourAction*, int p_parkourMode));

#undef DEFINE_FANCY_VF
}
