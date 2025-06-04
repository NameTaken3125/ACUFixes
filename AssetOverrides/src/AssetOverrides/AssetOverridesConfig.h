#pragma once

namespace AssetOverridesConfig {

void FindAndLoadConfigFileOrCreateDefault(const fs::path& filename);
void WriteToFile();

} // namespace AssetOverridesConfig

#include "OLYAFSer/OLYAFSer.h"

#define ACM(varName, VarType, AdapterCls, optionalDefaultValue) ADD_CONFIG_MEMBER(varName, VarType, AdapterCls, optionalDefaultValue)
#define YACSTOR(SubclsName) YACONFIGSECTION_SUBCLASS_CTOR(SubclsName)
struct AssetOverridesConfigTop : YAConfigSection {
    YACSTOR(AssetOverridesConfigTop);

    ACM(enableOnStartup, bool, BooleanAdapter, false);
};
extern AssetOverridesConfigTop g_AssetOverridesConfig;
