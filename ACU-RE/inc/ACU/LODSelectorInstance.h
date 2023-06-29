#pragma once

#include "ManagedObject.h"

class GraphicObjectInstanceData : public Object
{
public:
	char pad_0008[32]; //0x0008
}; //Size: 0x0028
assert_sizeof(GraphicObjectInstanceData, 0x28);

class GraphicObjectInstanceDataContainer : public GraphicObjectInstanceData
{
public:
}; //Size: 0x0028
assert_sizeof(GraphicObjectInstanceDataContainer, 0x28);

class LODSelector;
class MeshInstanceData;

class LODSelectorInstance : public GraphicObjectInstanceDataContainer
{
public:
	LODSelector* lodSelector; //0x0028
	MeshInstanceData* meshInstDatas[4]; //0x0030
	char pad_0050[40]; //0x0050
}; //Size: 0x0078
assert_sizeof(LODSelectorInstance, 0x78);
