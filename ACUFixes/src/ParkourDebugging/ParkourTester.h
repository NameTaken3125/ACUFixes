#pragma once

#include "ACU/basic_types.h"
#include "ACU/Entity.h"

#include "ParkourDebugging/FancyVFunctionDescription.h"


class Entity;
class Climb_140;

struct ParkourTester_FancyVTable
{
	std::array<FancyVFunction, 0x29> fancyVirtuals;
};

class ParkourTester
{
public:
	// @vtbl
	virtual void Unk000();

	ParkourTester_FancyVTable* fancyVTable;

	Vector4f handsPosition_mb; //0x0010
	Vector4f inputMovementDirectionOnGroundPlane; //0x0020
	float currentWASDMagnitude; //0x0030
	uint32 eq6fullLean; //0x0034
	char pad_0038[8]; //0x0038
	Vector4f vec40; //0x0040
	Vector4f movementVecWorldNormalized_mb; //0x0050
	Vector4f vec60; //0x0060
	Vector4f positionAwayFromWall_longer; //0x0070
	char pad_0080[16]; //0x0080
	Vector4f positionAwayFromWall_short; //0x0090
	Vector4f wallNormal_A0; //0x00A0
	void* pa_B0; //0x00B0
	char pad_00B8[4]; //0x00B8
	float flt_bc; //0x00BC
	float flt_c0; //0x00C0
	float flt_c4; //0x00C4
	float flt_c8; //0x00C8
	float flt_cc; //0x00CC
	float flt_d0; //0x00D0
	float flt_d4; //0x00D4
	float flt_d8; //0x00D8
	float flt_dc; //0x00DC
	float flt_e0; //0x00E0
	float flt_e4; //0x00E4
	float flt_e8; //0x00E8
	float flt_ec; //0x00EC
	char pad_00F0[40]; //0x00F0
	Entity* entity; //0x0118
	void* windowLedgeIfHangingOntoOne_mb; //0x0120
	char pad_0128[24]; //0x0128
	Climb_140* p140; //0x0140
	char pad_0148[24]; //0x0148
	Vector4f inputMovementDirectionOnWallVerticalPlane_mb; //0x0160
};
assert_offsetof(ParkourTester, entity, 0x118);

class AvailableParkourAction;

// FancyVFunctions in `ParkourTester.fancyVtable8`:
namespace ParkourTesterKnownFancyVFuncs
{
#define DEFINE_FANCY_VF(idx, name, nameHashed, fnType) using name = FancyVFunctionDescription<fnType, idx>
	DEFINE_FANCY_VF(0x16, ParkourTester_FancyVFunc_0x16, 0xC85ED05A, void (*)(__m128* smthOut, ParkourTester& parkourTester, __m128* p_locationOfOrigin, __m128* p_directionOfMovementInputWorldSpace));
	DEFINE_FANCY_VF(0x28, GetDefaultWeightForAction, 0xFEDB2C4C, float (*)(ParkourTester& parkourTester, AvailableParkourAction& action));

#undef DEFINE_FANCY_VF
}
