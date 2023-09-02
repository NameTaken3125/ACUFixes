#pragma once

#include "EntityAIProcess.h"

class ELAbstract : public EntityAIProcess
{
public:
}; //Size: 0x00A8
assert_sizeof(ELAbstract, 0xA8);

class CLAbstract : public ELAbstract
{
public:
}; //Size: 0x00A8
assert_sizeof(CLAbstract, 0xA8);
