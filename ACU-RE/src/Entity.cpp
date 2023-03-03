#include "pch.h"
#include "ACU/Entity.h"

Component* Entity::FindComponentByVTBL(uint64 vtbl)
{
	for (Component* cpnt : this->cpnts_mb)
	{
		uint64 thisCpntVtbl = *(uint64*)cpnt;
		if (thisCpntVtbl == vtbl) { return cpnt; }
	}
	return nullptr;
}
