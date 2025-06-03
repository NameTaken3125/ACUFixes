#pragma once

#include "ACU/basic_types.h"

struct ScannedLooseFile
{
    uint64 m_Handle;
    uint32 m_TypeInfoHash;
    std::reference_wrapper<TypeInfo> m_CorrespondingTypeInfo;
};
std::optional<ScannedLooseFile> ScanGameObjectLooseFile(const fs::path& targetFilepath);
