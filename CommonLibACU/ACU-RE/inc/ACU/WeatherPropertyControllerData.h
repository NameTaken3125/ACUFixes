#pragma once

#include "ACU/ManagedObject.h"
#include "ACU/SmallArray.h"

class PropertyControllerData_10_1A
{
public:
    char pad_0000[40]; //0x0000
}; //Size: 0x0028
assert_sizeof(PropertyControllerData_10_1A, 0x28);
class PropertyControllerData_10
{
public:
    char pad_0000[26]; //0x0000
    SmallArray<PropertyControllerData_10_1A> arr1A; //0x001A
    char pad_0026[50]; //0x0026
}; //Size: 0x0058
assert_sizeof(PropertyControllerData_10, 0x58);
class PropertyControllerData : public ManagedObject
{
public:
    SmallArray<PropertyControllerData_10> arr10; //0x0010
    char pad_001C[20]; //0x0018
}; //Size: 0x0030
assert_sizeof(PropertyControllerData, 0x30);
class WeatherPropertyControllerData : public PropertyControllerData
{
public:
}; //Size: 0x0030
assert_sizeof(WeatherPropertyControllerData, 0x30);
