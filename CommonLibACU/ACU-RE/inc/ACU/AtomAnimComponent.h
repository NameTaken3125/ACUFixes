#pragma once

#include "Component.h"
#include "SharedPtr.h"
#include "SmallArray.h"
#include "AtomGraphEvaluation.h"

class Human;

class IAnimationComponent : public Component
{
public:
    char pad_0020[8]; //0x0020
}; //Size: 0x0028
assert_sizeof(IAnimationComponent, 0x28);

class AtomGraph;
class ContactTable;
class SkeletonComponent;
class BipedComponent;
class EventSeed;
class Entity;
class Human;

class AtomStateTransitionTarget;

class AtomGraph_RTCP;

class GraphEvaluation;
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
	GraphEvaluation* pD0; //0x00D0
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
	GraphEvaluation_848 stru410; //0x0410
	char pad_0C40[24]; //0x0C40
	Human* human_c58; //0x0C58
	char pad_0C60[16]; //0x0C60
}; //Size: 0x0C70
assert_sizeof(AtomAnimComponent, 0xC70);
