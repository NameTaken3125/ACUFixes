#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"

class Mask
{
public:
	uint8 mask; //0x0000
}; //Size: 0x0001
assert_sizeof(Mask, 1);

class Mask16
{
public:
	uint16 mask; //0x0000
}; //Size: 0x0002
assert_sizeof(Mask16, 2);

class MaterialTemplate;
class BuildColumn_Components;

class Material : public ManagedObject
{
public:
	SharedPtrNew<MaterialTemplate>* shared_MaterialTemplate; //0x0010
	void* shared_textureSet; //0x0018
	char pad_0020[12]; //0x0020
	Mask mask_2C; //0x002C
	char pad_002D[7]; //0x002D
	Mask16 mask_34; //0x0034
	char pad_0036[2]; //0x0036
	BuildColumn_Components* Parameters; //0x0038
	void* textureSelector; //0x0040
	char pad_0048[48]; //0x0048
	uint8 byte_78; //0x0078
	char pad_0079[15]; //0x0079
	SharedPtrNew<Material>* selfSharedPtr; //0x0088
	char pad_0090[88]; //0x0090
}; //Size: 0x00E8
assert_sizeof(Material, 0xE8);
