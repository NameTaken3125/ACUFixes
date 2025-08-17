#pragma once

#include "AbstractEntityAI.h"

class AbstractCharacterAI : public AbstractEntityAI
{
public:
    char pad_0170[16]; //0x0170
}; //Size: 0x0180
assert_sizeof(AbstractCharacterAI, 0x0180);

class CharacterAI : public AbstractCharacterAI
{
public:
    char pad_0180[96]; //0x0180
}; //Size: 0x01E0
assert_sizeof(CharacterAI, 0x01E0);
