#pragma once

#include "Component.h"
#include "SmallArray.h"

class DataBinder;
class DataBindingComponent : public Component
{
public:
	char pad_0020[24]; //0x0020
	SmallArray<DataBinder*> dataBinders; //0x0038
	char pad_0044[84]; //0x0040
}; //Size: 0x0098
assert_sizeof(DataBindingComponent, 0x98);
