#pragma once

#include "basic_types.h"

class RandomNumberGenerator
{
public:
    // @members
    char pad_0[4];
    uint32 m_seed;

    // @helper_functions
    static RandomNumberGenerator* GetSingleton() { return *(RandomNumberGenerator**)0x14525FAE0; }
    uint32 GetNext()
    {
        uint32 newRand = m_seed * 0x19660D + 0x3C6EF35F;
        m_seed = newRand;
        return newRand;
    }
};
assert_offsetof(RandomNumberGenerator, m_seed, 4);
