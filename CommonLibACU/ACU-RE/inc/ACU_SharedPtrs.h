#pragma once

#include "ACU/ManagedObject.h"
#include "ACU/SharedPtr.h"


SharedBlock& FindOrMakeSharedBlockByHandleAndIncrementWeakrefcount(uint64 handle);
SharedBlock& FindOrMakeSharedBlockByHandleAndIncrementStrongRefcount(uint64 handle);
