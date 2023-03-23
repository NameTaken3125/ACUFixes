#pragma once

#include "Component.h"
#include "SmallArray.h"
#include "SharedPtr.h"


class AbstractDominoComponent : public Component
{
public:
}; //Size: 0x0020
assert_sizeof(AbstractDominoComponent, 0x20);

class DominoScriptDefinition;
class DominoScriptExecutionContext;
class EntityGroup;
class AIScriptInstance;

class DominoScriptInstance : public ManagedObject
{
public:
	char pad_0010[16]; //0x0010
	DominoScriptDefinition* dominoScriptDefinition; //0x0020
	char pad_0028[16]; //0x0028
	DominoScriptExecutionContext* dominoScriptExecutionContext; //0x0038
	EntityGroup* lanternEntity; //0x0040
	char pad_0048[40]; //0x0048
	SharedPtrNew<AIScriptInstance>* aiScriptInstance; //0x0070
	char pad_0078[8]; //0x0078
}; //Size: 0x0080
assert_sizeof(DominoScriptInstance, 0x80);

class EntityAliasContext;

class EntityDictionary : public Object
{
public:
	SmallArray<EntityAliasContext*> arrEntityAliasContext; //0x0008
	char pad_0014[4]; //0x0014
}; //Size: 0x0018
assert_sizeof(EntityDictionary, 0x18);

class EntitySpecification : public ManagedObject
{
public:
	char pad_0010[64]; //0x0010
}; //Size: 0x0050
assert_sizeof(EntitySpecification, 0x50);

class ReferencingSpecification : public EntitySpecification
{
public:
	char pad_0050[224]; //0x0050
}; //Size: 0x0130
assert_sizeof(ReferencingSpecification, 0x130);

class DominoReferencingSpecification : public ReferencingSpecification
{
public:
	char pad_0130[8]; //0x0130
}; //Size: 0x0138
assert_sizeof(DominoReferencingSpecification, 0x138);

class AIScriptInstance : public DominoScriptInstance
{
public:
	char pad_0080[80]; //0x0080
	EntityDictionary entityDictionary; //0x00D0
	char pad_00E8[96]; //0x00E8
	DominoReferencingSpecification dominoReferencingSpecification; //0x0148
	char pad_0280[32]; //0x0280
}; //Size: 0x02A0
assert_sizeof(AIScriptInstance, 0x2A0);

class DominoExecutionContextSnapshot : public ManagedObject
{
public:
	char pad_0010[96]; //0x0010
}; //Size: 0x0070
assert_sizeof(DominoExecutionContextSnapshot, 0x70);

class RefCountedObject : public Object
{
public:
	char pad_0008[8]; //0x0008
}; //Size: 0x0010
assert_sizeof(RefCountedObject, 0x10);

class DominoComponent : public AbstractDominoComponent
{
public:
	char pad_0020[8]; //0x0020
	AIScriptInstance aiScriptInstance_28; //0x0028
	DominoExecutionContextSnapshot dominoExecutionContextSnapshot_2c8; //0x02C8
	RefCountedObject refCountedObject_338; //0x0338
	char pad_0348[48]; //0x0348
}; //Size: 0x0378
assert_sizeof(DominoComponent, 0x378);
