#pragma once

#include "OLAbstract.h"

class OLBallisticProjectile : public OLAbstract
{
public:
	char pad_00A8[392]; //0x00A8
}; //Size: 0x0230
assert_sizeof(OLBallisticProjectile, 0x230);
