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

// Thanks to Sunbeam at https://fearlessrevolution.com/viewtopic.php?t=9605#post-92942.
void DisableMainIntegrityCheck()
{
    ForEachThreadIDOfThisProcess(TerminateThreadIfRunsTheMainIntegrityCheck);
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

#include <winternl.h>
void ClearThe_BeingDebugged_Flag()
{
    NtCurrentTeb()->ProcessEnvironmentBlock->BeingDebugged = 0;
}
