#pragma once

#include "basic_types.h"

enum class LODLevel : uint64
{
    LODLEVEL_NOTVISIBLE = 0,
    LODLEVEL_VERYFAR = 1,
    LODLEVEL_FAR = 2,
    LODLEVEL_MID = 3,
    LODLEVEL_NEAR = 4,
    LODLEVEL_VERYNEAR = 5,
    LODLEVELCOUNT = 6,
};
