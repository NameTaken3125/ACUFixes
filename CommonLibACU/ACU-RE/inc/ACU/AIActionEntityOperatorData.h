#pragma once

#include "DominoOperatorData.h"

class DominoAIOperatorData : public DominoOperatorData
{
public:
}; //Size: 0x0060
assert_sizeof(DominoAIOperatorData, 0x60);

class DominoDynamicPtr : public Object
{
public:
	char pad_0008[8]; //0x0008
	float flt_10; //0x0010
	float flt_14; //0x0014
	char pad_0018[16]; //0x0018
}; //Size: 0x0028
assert_sizeof(DominoDynamicPtr, 0x28);

class DominoDynamic : public DominoDynamicPtr
{
public:
	char pad_0028[32]; //0x0028
}; //Size: 0x0048
assert_sizeof(DominoDynamic, 0x48);

class AIAction;

class AIActionEntityOperatorData : public DominoAIOperatorData
{
public:
	DominoDynamic dominoDynamic_60; //0x0060
	DominoDynamic dominoDynamic_A8; //0x00A8
	AIAction* aiAction; //0x00F0
	DominoDynamic dominoDynamic_F8; //0x00F8
	char pad_0140[40]; //0x0140
}; //Size: 0x0168
assert_sizeof(AIActionEntityOperatorData, 0x168);
