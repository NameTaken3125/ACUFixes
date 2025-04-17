#pragma once

#include "basic_types.h"
#include "SmallArray.h"
#include "SharedPtr.h"


class GraphEvaluation_1058
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
assert_sizeof(GraphEvaluation_1058, 0x20);

class GraphEvaluation_848
{
public:
	SmallArraySemistatic<GraphEvaluation_1058, 64> arr0; //0x0000
	SmallArray<GraphEvaluation_1058>* paActiveAnims_mb; //0x0810
	uint8 byte_818; //0x0818
	char pad_0819[23]; //0x0819
}; //Size: 0x0830
assert_sizeof(GraphEvaluation_848, 0x830);
class GraphEvaluation_510_8;
class GraphEvaluation_510_10;
class GraphEvaluation_510
{
public:
	char pad_0000[4]; //0x0000
	uint8 byte_4; //0x0004
	char pad_0005[3]; //0x0005
	GraphEvaluation_510_8* p8; //0x0008
	GraphEvaluation_510_10* p10; //0x0010
	char pad_0018[8]; //0x0018
}; //Size: 0x0020

class Entity;
class GraphEvaluation;
class GraphEvaluation_400
{
public:
	char pad_0000[32]; //0x0000
	SmallArray<uint8> graphVarsBuffer; //0x0020
	char pad_002C[180]; //0x002C
	SmallArray<uint8> bitmaskOfUpdatedGraphVars; //0x00E0
	SmallArray<uint32> arrEC; //0x00EC
	char pad_00F8[24]; //0x00F8
	SmallArray<SmallArray<GraphEvaluation_510>> arr110; //0x0110
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
assert_offsetof(GraphEvaluation_400, dword_24C, 0x24C);
assert_sizeof(GraphEvaluation_400, 0x270);

class GraphEvaluation_StateNodeEval_StateMachine;
class GraphEvaluation_StateNodeEval_TransitionDesc
{
public:
	int32 int_0; //0x0000
	uint16 word4; //0x0004
	char pad_0006[2]; //0x0006
	GraphEvaluation_StateNodeEval_StateMachine* withinWhichStateMachine; //0x0008
	char pad_0010[8]; //0x0010
}; //Size: 0x0018
assert_sizeof(GraphEvaluation_StateNodeEval_TransitionDesc, 0x18);

struct GraphEval_StateNodeOrTransitionEval
{
public:
	virtual void Unk000_Destroy();
};


class AtomStateNode_Base8;
class GraphEvaluation_StateNodeEval : public GraphEval_StateNodeOrTransitionEval
{
public:
	AtomStateNode_Base8* pAtomStateNodePlus8; //0x0008
	char pad_0010[2]; //0x0010
	uint16 word_12; //0x0012
	char pad_0014[12]; //0x0014
	SmallArraySemistatic<uint8, 160> instanceDataBuffer; //0x0020
	SmallArraySemistatic<GraphEvaluation_StateNodeEval_TransitionDesc, 0x10> transitionDescs; //0x00D0
}; //Size: 0x0260
assert_sizeof(GraphEvaluation_StateNodeEval, 0x260);

class GraphEvaluation_StateNodeEval_StateMachine : public GraphEvaluation_StateNodeEval
{
public:
	char pad_0260[1]; //0x0260
	uint8 byte_261; //0x0261
	char pad_0262[6]; //0x0262
	GraphEvaluation_StateNodeEval* currentStateOrTransition; //0x0268
	void* transitionTargetState_mb; //0x0270
	char pad_0278[32]; //0x0278
	SmallArray<GraphEvaluation_StateNodeEval_TransitionDesc> globalTransitionDescs; //0x0298
	char pad_02A4[108]; //0x02A4
}; //Size: 0x0310
assert_sizeof(GraphEvaluation_StateNodeEval_StateMachine, 0x310); // See at 1400A11D5


class AtomAnimComponent;
class AtomGraph;
class AtomGraph_RTCP;

class GraphEvaluation
{
public:
	char pad_0000[16]; //0x0000
	// You can see p268->instanceDataBufferSize change from 144 (like InstanceDataSize of first state)
	// to 0 (like the second, "cinematic" state) when switching from normal gameplay to cinematic state.
	// Switching back is not instant, so p268 points to some kind of different, "transition evaluation" structure,
	// I think.
	GraphEvaluation_StateNodeEval_StateMachine rootStateMachineEval; //0x0010
	char pad_0320[224]; //0x0320
	GraphEvaluation_400 stru400; //0x0400
	AtomGraph* atomGraph; //0x0670
	AtomGraph_RTCP* rtcp; //0x0678
	char pad_0680[32]; //0x0680
	AtomAnimComponent* animCpnt; //0x06A0
	char pad_06A8[416]; //0x06A8
	GraphEvaluation_848 stru848; //0x0848
	GraphEvaluation_848 stru1078; //0x1078
	GraphEvaluation_848 stru18A8; //0x18A8
	char pad_20D8[4]; //0x20D8
	float recentFrameDeltaT_mb_20DC; //0x20DC
	char pad_20E0[448]; //0x20E0
}; //Size: 0x22A0
assert_sizeof(GraphEvaluation, 0x22A0);
