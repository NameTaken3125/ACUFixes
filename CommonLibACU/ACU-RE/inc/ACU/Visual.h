#pragma once

#include "Component.h"
#include "SharedPtr.h"


class VisualComponent : public Component
{
public:
}; //Size: 0x0020
assert_sizeof(VisualComponent, 0x20);

class LODSelector;
class LODSelectorInstance;

struct Visual_Flags54
{
	bool isHidden : 1;
	unsigned char pad1[1];
};
assert_sizeof(Visual_Flags54, 2);
class Visual : public VisualComponent
{
public:

	float flt_20; //0x0020
	float flt_24; //0x0024
	float flt_28; //0x0028
	float flt_2C; //0x002C
	char pad_0030[8]; //0x0030
	SharedPtrNew<LODSelector>* shared_LODSelector; //0x0038
	LODSelectorInstance* LODSelectorInstance_; //0x0040
	char pad_0048[12]; //0x0048
	Visual_Flags54 flags; //0x0054
	char pad_0056[10]; //0x0056
}; //Size: 0x0060
assert_sizeof(Visual, 0x60);
