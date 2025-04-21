#include "pch.h"

#include "Hack_NoWaitForUnsafeEject.h"

NoWaitForUnsafeEject::NoWaitForUnsafeEject()
{
    uintptr_t whenWhileOnWallSystemIsCreated_setWaitDurationForUnsafeEject = 0x141A4DE9B;
    DEFINE_ADDR(whenWhileOnWallSystemIsCreated_setWaitDurationForUnsafeEject_InlineFloat4bytes,
        whenWhileOnWallSystemIsCreated_setWaitDurationForUnsafeEject + 6);
    whenWhileOnWallSystemIsCreated_setWaitDurationForUnsafeEject_InlineFloat4bytes = {
        dd(0)
    };
}
