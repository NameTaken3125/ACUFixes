#pragma once

#include "Component.h"
#include "SharedPtr.h"

class Human;

class IAnimationComponent : public Component
{
public:
    char pad_0020[8]; //0x0020
}; //Size: 0x0028
assert_sizeof(IAnimationComponent, 0x28);


class AtomAnimComponent_D0_1058
{
public:
	char pad_0000[8]; //0x0000
	uint32 id; //0x0008
	char pad_000C[4]; //0x000C
	float flt_10; //0x0010
	float flt_14; //0x0014
	float flt_18; //0x0018
	char pad_001C[4]; //0x001C
}; //Size: 0x0020
assert_sizeof(AtomAnimComponent_D0_1058, 0x20);

class AtomAnimComponent_D0_848
{
public:
	SmallArraySemistatic<AtomAnimComponent_D0_1058, 64> arr0; //0x0000
	class AtomAnimComponent_D0_1058(**paActiveAnims_mb)[20]; //0x0810
	uint8 byte_818; //0x0818
	char pad_0819[7]; //0x0819
}; //Size: 0x0820

class AtomGraph;
class ContactTable;
class SkeletonComponent;
class BipedComponent;
class EventSeed;
class Entity;
class Human;

class AtomStateTransitionTarget;

class AtomAnimComponent_D0;

class AtomAnimComponent : public IAnimationComponent
{
public:
	SharedPtrNew<AtomGraph>* shared_AtomGraph_NewDemo_DEV; //0x0028
	SharedPtrNew<ContactTable>* shared_ContactTable; //0x0030
	char pad_0038[16]; //0x0038
	void* arr48; //0x0048
	char pad_0050[2]; //0x0050
	uint16 arr48size; //0x0052
	char pad_0054[124]; //0x0054
	AtomAnimComponent_D0* pD0; //0x00D0
	SkeletonComponent* skeletonCpnt; //0x00D8
	BipedComponent* bipedCpnt; //0x00E0
	SmallArraySemistatic<EventSeed*, 16> evnSeedsE8; //0x00E8
	char pad_0178[160]; //0x0178
	SharedPtrNew<Entity>* shared_Entity; //0x0218
	char pad_0220[160]; //0x0220
	AtomStateTransitionTarget* atomStateTransitionTarget_2C0; //0x02C0
	char pad_02C8[248]; //0x02C8
	SharedPtrNew<SkeletonComponent>* shared_prototypeSkeletonCpnt_mb; //0x03C0
	char pad_03C8[64]; //0x03C8
	uint8 byte_408; //0x0408
	char pad_0409[7]; //0x0409
	AtomAnimComponent_D0_848 stru410; //0x0410
	char pad_0C30[40]; //0x0C30
	Human* human_c58; //0x0C58
	char pad_0C60[16]; //0x0C60
}; //Size: 0x0C70
assert_sizeof(AtomAnimComponent, 0xC70);
