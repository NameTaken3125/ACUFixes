#pragma once

#include "Statechart.h"
class EntityAIProcess : public Statechart
{
public:
}; //Size: 0x00A8
assert_sizeof(EntityAIProcess, 0xA8);

class ESrvAbstract : public EntityAIProcess
{
public:
}; //Size: 0x00A8
assert_sizeof(ESrvAbstract, 0xA8);

class CSrvAbstract : public ESrvAbstract
{
public:
}; //Size: 0x00A8
assert_sizeof(CSrvAbstract, 0xA8);
