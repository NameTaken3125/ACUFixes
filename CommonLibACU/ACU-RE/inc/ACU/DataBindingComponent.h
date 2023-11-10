#pragma once

#include "Component.h"
#include "SmallArray.h"

class DataBinder;

class DataBindingComponent_Base20
{
public:
	virtual ManagedObject* Unk_000_GetCurrentlySelectedGearUIInfo_mb(void);
	// More virtuals, don't know how many...
};
assert_sizeof(DataBindingComponent_Base20, 8);

class DataBindingComponent : public Component
{
public:
	DataBindingComponent_Base20 base20;
	char pad_0028[16]; //0x0028
	SmallArray<DataBinder*> dataBinders; //0x0038
	char pad_0044[84]; //0x0040
}; //Size: 0x0098
assert_sizeof(DataBindingComponent, 0x98);
