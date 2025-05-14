#include "pch.h"

#include "Hook_ZwRaiseException.h"
#include "MyLog.h"

#include "AutoAssemblerKinda/AutoAssemblerKinda.h"
void CrashLog_TestVariousWaysOfCrashing_DrawImGui()
{
    if (ImGui::Button("CRASH: Write to nullptr"))
    {
        // Depending on the method that Present() was hooked,
        // the UnhandledExceptionFilter() might not catch this,
        // but ZwRaiseException() hook can.
        volatile int* x = nullptr;
        *x = 2;
    }
    if (ImGui::Button("CRASH: Throw C++ exception"))
    {
        // Depending on the method that Present() was hooked,
        // the UnhandledExceptionFilter() might not catch this,
        // but ZwRaiseException() hook can.
        std::optional<int> x;
        int y = x.value();
    }
    if (ImGui::Button("CRASH GAMETHREAD: Write nullptr to game global"))
    {
        // Caught by UnhandledExceptionFilter() only if NOT under a debugger.
        // Caught by ZwRaiseException() only if under a debugger.
        *(uintptr_t*)0x14521AAD0 = 0;
    }
    if (ImGui::Button("CRASH GAMETHREAD: Set UEF and write nullptr to game global"))
    {
        SetUnhandledExceptionFilter([](::_EXCEPTION_POINTERS* exc) -> LONG
            {
                MessageBoxA(NULL, "Just set the UEF; crash from gamethread", "Crash Log", MB_OK | MB_ICONSTOP);
                return EXCEPTION_CONTINUE_SEARCH;
            });
        *(uintptr_t*)0x14521AAD0 = 0;
    }
    if (ImGui::Button("CRASH GAMETHREAD: Set UEF and write nullptr to game global; call previous handler too"))
    {
        static LPTOP_LEVEL_EXCEPTION_FILTER prevHandler = nullptr;
        prevHandler = SetUnhandledExceptionFilter([](::_EXCEPTION_POINTERS* exc) -> LONG
            {
                MessageBoxA(NULL, "Just set the UEF; crash from gamethread", "Crash Log", MB_OK | MB_ICONSTOP);
                return prevHandler(exc);
            });
        *(uintptr_t*)0x14521AAD0 = 0;
    }
    //if (ImGui::Button("CRASH ACUFIXES SPINTHREAD: Write to nullptr"))
    //{
    //    // In the PluginLoader thread that sleeps in a loop, write to nullptr if
    //    // if this bool is true.
    //    extern bool g_DoCrashSpinthread; g_DoCrashSpinthread = true;
    //}
    if (ImGui::Button("NO CRASH: Throw and catch C++ exception"))
    {
        try
        {
            int y = std::optional<int>().value();
        }
        catch (const std::bad_optional_access&)
        {
            LOG_DEBUG(DefaultLogger,
                L"[CrashLog_TestVariousWaysOfCrashing] Exception manually thrown and safely caught using C++."
            );
        }
    }
    if (ImGui::Button("NO CRASH: Throw and catch using SEH"))
    {
        auto SEHTest = []()
            {
                __try
                {
                    int* x = 0;
                    *x = 42;
                }
                __except (EXCEPTION_EXECUTE_HANDLER)
                {
                    LOG_DEBUG(DefaultLogger,
                        L"[CrashLog_TestVariousWaysOfCrashing] Exception manually thrown and safely caught using SEH."
                    );
                }
            };
        SEHTest();
    }
}

class PatchContainer_CrashLog
{
public:
    AutoAssembleWrapper<Hook_ZwRaiseException> zwExcHook;
};
std::optional<PatchContainer_CrashLog> g_PatchContainer_CrashLog;
void CrashLog_CodePatches_Start()
{
    g_PatchContainer_CrashLog.emplace();
    g_PatchContainer_CrashLog->zwExcHook.Activate();
}
