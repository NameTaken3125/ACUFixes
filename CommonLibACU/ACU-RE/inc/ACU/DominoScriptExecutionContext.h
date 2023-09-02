#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class DominoOperatorData;
class DominoScriptDefinition;
class AIScriptInstance;

class DominoScriptExecutionContext : public ManagedObject
{
public:
	DominoScriptDefinition* scriptDefinition_10; //0x0010
	char pad_0018[8]; //0x0018
	AIScriptInstance* scriptInstance; //0x0020
	char pad_0028[112]; //0x0028
	SmallArray<DominoOperatorData*> arrOperatorData; //0x0098
	char pad_00A4[148]; //0x00A4
}; //Size: 0x0138
assert_sizeof(DominoScriptExecutionContext, 0x138);
