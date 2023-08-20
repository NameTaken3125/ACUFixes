#pragma once

#include "ManagedObject.h"


class UIString
{
public:
    uint32 stringID;
};
assert_sizeof(UIString, 4);
