#pragma once

#include "ManagedObject.h"


class DominoOperator;
class DominoScriptExecutionContext;

class DominoDataInterface : public Object
{
public:
	DominoOperator* operator_; //0x0008
	DominoScriptExecutionContext* dominoScriptExecutionContext; //0x0010
}; //Size: 0x0018
assert_sizeof(DominoDataInterface, 0x18);

class DominoOperatorData : public DominoDataInterface
{
public:
	char pad_0018[72]; //0x0018
}; //Size: 0x0060
assert_sizeof(DominoOperatorData, 0x60);
