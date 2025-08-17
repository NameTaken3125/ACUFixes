#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"
#include "DangerEvent.h"

class Entity;

class ContactTable : public ManagedObject
{
public:
	char pad_0010[16]; //0x0010
}; //Size: 0x0020
assert_sizeof(ContactTable, 0x20);

class BallisticProjectileDataPack : public ManagedObject
{
public:
	char pad_0010[24]; //0x0010
}; //Size: 0x0028
assert_sizeof(BallisticProjectileDataPack, 0x28);

class BallisticProjectileSettings : public ManagedObject
{
public:
	char pad_0010[32]; //0x0010
	ContactTable contactTable; //0x0030
	DangerEvent::SubType dangerEventSubtype; //0x0050
	char pad_0054[4]; //0x0054
	BallisticProjectileDataPack projectileDataPack; //0x0058
	SharedPtrNew<Entity>* bombEntityPrototype_mb; //0x0080
	char pad_0088[8]; //0x0088
	float nearAndMidModeMaxDistance_sorta; //0x0090
	float nearModeMinDistance_sorta; //0x0094
	float speedAffectsNearModeDistance; //0x0098
	float flt_9c; //0x009C
	char pad_00A0[8]; //0x00A0
	float collisionSizeInv_mb; //0x00A8
	float flt_ac; //0x00AC
	float flt_b0; //0x00B0
	float flt_b4; //0x00B4
	uint64 qword_b8; //0x00B8
	char pad_00C0[20]; //0x00C0
	float flt_d4; //0x00D4
	char pad_00D8[32]; //0x00D8
	uint64 qword_F8; //0x00F8
	char pad_0100[24]; //0x0100
	float circleWidth; //0x0118
	float flt_11c; //0x011C
	float flt_120; //0x0120
	float flt_124; //0x0124
	char pad_0128[16]; //0x0128
}; //Size: 0x0138
assert_sizeof(BallisticProjectileSettings, 0x138);
