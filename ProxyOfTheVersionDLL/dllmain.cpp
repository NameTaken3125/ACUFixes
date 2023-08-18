#include "version/version.h"

#include <filesystem>
namespace fs = std::filesystem;

HMODULE g_thisDLL = nullptr;
fs::path g_injectedDLLPath;

fs::path& GetThisDLLAbsolutePath()
{
    static fs::path dllPath = []()
    {
        char path[MAX_PATH];
        GetModuleFileNameA(g_thisDLL, path, (DWORD)std::size(path));
        return fs::path(path);
    }();
    return dllPath;
}
static void FindOutWhereThisDLLIs(HMODULE hModule)
{
    g_thisDLL = hModule;
    (void)GetThisDLLAbsolutePath();
}

static void LoadTheActualTargetDLL()
{
    g_injectedDLLPath = GetThisDLLAbsolutePath().parent_path() / "ACUFixes/ACUFixes-PluginLoader.dll";
    std::wstring pathString = g_injectedDLLPath.native();
    HMODULE targetDLL = LoadLibraryW(pathString.c_str());
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        setupWrappers();
        FindOutWhereThisDLLIs(hModule);
        LoadTheActualTargetDLL();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
