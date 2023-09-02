#pragma once

#include "SharedPtr.h"

class FXInstance;
class Entity;

class WhenHighlightedNPCChanges
{
public:
	char pad_0000[16]; //0x0000
	SharedPtrNew<Entity>* shared_10; //0x0010
	SharedPtrNew<Entity>* highlightedNPC; //0x0018
	char pad_0020[8]; //0x0020
	SharedPtrNew<FXInstance>* highlightedNPCFxInstance; //0x0028
	char pad_0030[96]; //0x0030
	Entity* player; //0x0090
	char pad_0098[160]; //0x0098
	SharedPtrNew<Entity>* shared_138; //0x0138
	SharedPtrNew<Entity>* shared_140; //0x0140
	char pad_0148[248]; //0x0148
}; //Size: 0x0240
assert_offsetof(WhenHighlightedNPCChanges, highlightedNPC, 0x18);
assert_offsetof(WhenHighlightedNPCChanges, player, 0x90);
