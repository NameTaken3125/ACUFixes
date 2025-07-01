#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "AnimTrackEvent.h"

class FXConstantTable : public Object
{
public:
	char pad_0008[48]; //0x0008
}; //Size: 0x0038
assert_sizeof(FXConstantTable, 0x38);

class FXLinkTable : public Object
{
public:
	char pad_0008[16]; //0x0008
}; //Size: 0x0018
assert_sizeof(FXLinkTable, 0x18);

class FXAnimTable : public Object
{
public:
	char pad_0008[16]; //0x0008
}; //Size: 0x0018
assert_sizeof(FXAnimTable, 0x18);

class FXDefaultTable : public Object
{
public:
	char pad_0008[48]; //0x0008
}; //Size: 0x0038
assert_sizeof(FXDefaultTable, 0x38);

class FXOutputTable : public Object
{
public:
	char pad_0008[48]; //0x0008
}; //Size: 0x0038
assert_sizeof(FXOutputTable, 0x38);

class DiagramCode
{
public:
	char pad_0000[88]; //0x0000
}; //Size: 0x0058
assert_sizeof(DiagramCode, 0x58);

class FXElement;

class FX : public ManagedObject
{
public:
	uint64 Loop : 1;
	uint64 RunWhileGameIsPaused : 1;
	uint64 StartWhileNotVisible : 1;
	uint64 RunWhileNotVisible : 1;
	uint64 StartWhileOutOfRange : 1;
	uint64 RunWhileOutOfRange : 1;
	uint64 IgnoreSlowdown : 1;
	uint64 IsGlobal : 1;

	uint64 IsGlobalForceUnique : 1;
	uint64 MaxInstanceCount : 10;
	uint64 MaxInstanceStartPerFrame : 6;
	uint64 MaxInstanceStopPerFrame : 6;
	uint64 Bloody : 1;
	uint64 Highlighting : 1;
	uint64 PlayFXOnHierarchyChild : 1;
	uint64 PlayFXOnBoneHierarchyChild : 1;
	uint64 SynchronizeElements : 1;

	uint64 pad_0x10_bits36_58 : 22;

	uint64 ForceSingleThread : 1;
	uint64 UIFlag : 1;
	uint64 UpdateLast : 1;
	SmallArray<FXElement*> FXElements; //0x0018
	char pad_0024[4]; //0x0024
	FXConstantTable FXConstantTable_; //0x0028
	FXLinkTable FXLinkTable_; //0x0060
	FXAnimTable FXAnimTable_; //0x0078
	FXDefaultTable FXDefaultTable_; //0x0090
	FXOutputTable FXOutputTable_; //0x00C8
	AnimTrackEvent AnimTrackEvent_; //0x0100
	AnimTrackEvent AnimTrackAlwaysCalledEvent; //0x0130
	float PhaseLengths[3]; //0x0160
	float Range; //0x016C
	DiagramCode FXCode; //0x0170
	char pad_01C8[16]; //0x01C8
};
assert_sizeof(FX, 0x1D8);