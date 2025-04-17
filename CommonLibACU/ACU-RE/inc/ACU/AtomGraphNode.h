#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "AtomNodeID.h"


class AtomGraphNode;
class AtomOutputPort;

enum class AtomInputPort_BindingType
{
	BT_RTCP = 0x0,
	BT_ConstantBool = 0x2,
	BT_ConstantFloat = 0x3,
	BT_OutputPortPointer = 0x9,
	BT_Unused = 12,
};
class AtomInputPort
{
public:
	// @members
	uint32 BindingType; //0x0000
	char pad_0004[12]; //0x0004
	AtomOutputPort* outputPortIfTypeEq9_graphVarIdxIfEq0; //0x0010
	char pad_0018[8]; //0x0018

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D9C90; }
}; //Size: 0x0020
assert_sizeof(AtomInputPort, 0x20);

class AtomOutputPort
{
public:
	// @members
	AtomGraphNode* graphNode; //0x0000
	uint32 InstanceDataOffset; //0x0008
	char pad_000C[4]; //0x000C

	// @helper_functions
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D9D40; }
}; //Size: 0x0010
assert_sizeof(AtomOutputPort, 0x10);

class AnimationDataNodeEvaluation_Stack_a2;

class AtomGraphNode : public Object
{
public:
	// @vtbl
	virtual void Unk000();
	virtual void Unk008();
	virtual void Unk010_Deserialize(DeserializationState* deserializationStream);
	virtual Object* Unk018_Clone(uint64 a2, uint32 a3);
	virtual void Unk020();
	virtual TypeInfo& Unk028_GetTI();
	virtual void Unk030();
	virtual void Unk038();
	virtual void Unk040();
	virtual void Unk048();
	virtual void Unk050();
	virtual void Unk058();
	virtual void Unk060();
	virtual void Unk068();
	virtual void Unk070();
	virtual void Unk078();
	virtual void Unk080();
	virtual void Unk088();
	virtual void Unk090();
	virtual void Unk098(AnimationDataNodeEvaluation_Stack_a2* evaluationOnStack);

	// @members
	SmallArray<AtomInputPort> InputPorts; //0x0008
	char pad_0014[4]; //0x0014
	SmallArray<AtomOutputPort> OutputPorts; //0x0018
	char pad_0024[12]; //0x0024
	uint32 outputInstanceDataOffset_mb; //0x0030
	char pad_0034[4]; //0x0034
	AtomNodeID AtomTaskID; //0x0038
	uint16 PreUpdateID_reverseGraphNodeIdxInGraph; //0x0048
	char pad_004A[6]; //0x004A
}; //Size: 0x0050
assert_sizeof(AtomGraphNode, 0x50);
