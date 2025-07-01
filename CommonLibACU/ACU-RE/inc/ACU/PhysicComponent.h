#pragma once

#include "Component.h"
#include "SmallArray.h"

enum class Damage : uint32
{
	Damage_Medium = 1,
};

enum class CharacterInteractionMode : uint32
{
	CharacterInteraction_Blocking = 0,
};
class RagdollComponent_2A
{
public:
	char pad_0000[96]; //0x0000
}; //Size: 0x0060
assert_sizeof(RagdollComponent_2A, 0x60);

class PhysicComponent : public Component
{
public:
	Damage Damage_; //0x0020
	CharacterInteractionMode CharacterInteractionMode_; //0x0024
	char pad_0028[2]; //0x0028
	SmallArray<RagdollComponent_2A> arr2A; //0x002A
	char pad_0036[26]; //0x0036
}; //Size: 0x0050
assert_sizeof(PhysicComponent, 0x50);
