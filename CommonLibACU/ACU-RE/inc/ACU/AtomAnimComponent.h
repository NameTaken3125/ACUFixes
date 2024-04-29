#pragma once

#include "Component.h"
#include "SharedPtr.h"
#include "SmallArray.h"

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


class AtomAnimComponent_D0_510
{
public:
	char pad_0000[4]; //0x0000
	uint8 byte_4; //0x0004
	char pad_0005[3]; //0x0005
	class AtomAnimComponent_D0_510_8* p8; //0x0008
	class AtomAnimComponent_D0_510_10* p10; //0x0010
	char pad_0018[8]; //0x0018
}; //Size: 0x0020

class GraphEvaluation;
class AtomAnimComponent_D0_400
{
public:
	char pad_0000[32]; //0x0000
	SmallArray<uint8> graphVarsBuffer; //0x0020
	char pad_002C[180]; //0x002C
	SmallArray<uint8> bitmaskOfUpdatedGraphVars; //0x00E0
	SmallArray<uint32> arrEC; //0x00EC
	char pad_00F8[24]; //0x00F8
	SmallArray<SmallArray<AtomAnimComponent_D0_510>> arr110; //0x0110
	SmallArray<uint8> someByteForEveryGraphVar; //0x011C
	SmallArray<SharedPtrNew<Entity>*> arrEntityrefGraphvars; //0x0128
	char pad_0134[280]; //0x0134
	uint32 dword_24C; //0x024C
	uint32 dword_250; //0x0250
	uint32 frameIdx_mb; //0x0254
	GraphEvaluation* graphEval; //0x0258
	SmallArray<uint8> arr260; //0x0260
	char pad_026C[4]; //0x026C
}; //Size: 0x0270
assert_offsetof(AtomAnimComponent_D0_400, dword_24C, 0x24C);
assert_sizeof(AtomAnimComponent_D0_400, 0x270);

class AtomGraph_RTCP;

class GraphEvaluation
{
public:
	char pad_0000[1024]; //0x0000
	AtomAnimComponent_D0_400 stru400; //0x0400
	AtomGraph* atomGraph; //0x0670
	AtomGraph_RTCP* rtcp; //0x0678
	char pad_0680[32]; //0x0680
	class AtomAnimComponent* animCpnt; //0x06A0
	char pad_06A8[416]; //0x06A8
	class AtomAnimComponent_D0_848 stru848; //0x0848
	char pad_1068[16]; //0x1068
	class AtomAnimComponent_D0_848 stru1078; //0x1078
	char pad_1898[16]; //0x1898
	class AtomAnimComponent_D0_848 stru18A8; //0x18A8
	char pad_20C8[20]; //0x20C8
	float recentFrameDeltaT_mb_20DC; //0x20DC
	char pad_20E0[448]; //0x20E0
}; //Size: 0x22A0
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
	AtomAnimComponent_D0_848 stru410; //0x0410
	char pad_0C30[40]; //0x0C30
	Human* human_c58; //0x0C58
	char pad_0C60[16]; //0x0C60
}; //Size: 0x0C70
assert_sizeof(AtomAnimComponent, 0xC70);
