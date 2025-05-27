#include "pch.h"

#include "ACU/Threads/KnownThreads.h"
#include "EarlyHooks/ThreadOperations.h"

#include "MyLog.h"


bool TerminateThreadIfRunsTheMainIntegrityCheck(ThreadID_t thread_id)
{
    bool isFoundAndTerminated = false;
    HANDLE thread_handle = OpenThread(THREAD_ALL_ACCESS, 0, thread_id);
    if (!thread_handle) return false;
    uintptr_t startAddress = GetThreadStartAddress(thread_handle);
    LOG_DEBUG(DefaultLogger, "ThreadID: %4X, start address: %llx\n", thread_id, startAddress);
    if (startAddress == ACU::Threads::ThreadStartAddr_MainIntegrityCheck)
    {
        TerminateThread(thread_handle, 0);
        isFoundAndTerminated = true;
    }
    CloseHandle(thread_handle);
    return isFoundAndTerminated;
}

bool g_MainIntegrityCheckWasTerminatedAfterStarting = false;
// Thanks to Sunbeam at https://fearlessrevolution.com/viewtopic.php?t=9605#post-92942.
void DisableMainIntegrityCheck()
{
    ForEachThreadIDOfThisProcess([](ThreadID_t thread_id) {
        const bool isFoundAndTerminated = TerminateThreadIfRunsTheMainIntegrityCheck(thread_id);
        if (isFoundAndTerminated)
        {
            g_MainIntegrityCheckWasTerminatedAfterStarting = true;
            LOG_DEBUG(DefaultLogger,
                "\n"
                "[+] Main Integrity Check was terminated _after_ having started.\n"
                "    Normally I would be able to _prevent_ it from starting, but something went differently.\n"
                "    Did you install any hardware breakpoints (like e.g. using Cheat Engine's VEH debugger)?\n"
                "    During the game's startup I need to manage some of the hardware breakpoints myself\n"
                "    in order to place some early hooks.\n"
                "    Regardless, most of the mod's features don't require these very early hooks\n"
                "    and should be able to work fine.\n"
            );
        }
        const bool stopSearch = isFoundAndTerminated;
        return stopSearch;
        });
}
#include "ACU/basic_types.h"
class ACUPlayerCameraComponent;
ACUPlayerCameraComponent* Get_ACUPlayerCameraComponent()
{
    uint64 cameraManager = *(uint64*)0x14521AAD0;
    if (!cameraManager) { return nullptr; }
    const uint64 arrayOfCameraComponents = (uint64)(cameraManager + 0x40);
    const uint16 arrayOfCameraComponents_size = *(uint16*)(arrayOfCameraComponents + 0xA);
    if (!arrayOfCameraComponents_size) { return nullptr; }
    return *(ACUPlayerCameraComponent**)arrayOfCameraComponents;
}
void WaitUntilGameIsInitializedEnoughSoThatTheMainIntegrityCheckCanBeDisabled()
{
    while (true)
    {
        if (!Get_ACUPlayerCameraComponent())
        {
            Sleep(100);
            continue;
        }
        break;
    }
}
bool g_IsMICDisabled = false;
extern bool g_IsMICDisabled;
void WaitUntilMICIsDisabled()
{
    while (true)
    {
        if (g_IsMICDisabled) return;
        bool micWouldDefinitelyHaveStarted = Get_ACUPlayerCameraComponent() != nullptr;
        if (micWouldDefinitelyHaveStarted)
        {
            DisableMainIntegrityCheck();
            g_IsMICDisabled = true;
            break;
        }
        Sleep(500);
    }
}

#include <winternl.h>
void ClearThe_BeingDebugged_Flag()
{
    NtCurrentTeb()->ProcessEnvironmentBlock->BeingDebugged = 0;
}
