#pragma once

#include "PhysicComponent.h"
#include "SharedPtr.h"

class RagdollNew;

class RagdollComponent : public PhysicComponent
{
public:
	char pad_0050[8]; //0x0050
	  SharedPtrNew<RagdollNew>* Ragdoll; //0x0058
	  char pad_0060[688]; //0x0060
}; //Size: 0x0310
assert_sizeof(RagdollComponent, 0x310);
