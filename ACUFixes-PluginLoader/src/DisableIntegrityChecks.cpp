#include "pch.h"

#include <TlHelp32.h>
#include <winternl.h>
#pragma comment(lib, "ntdll")

#include "MyLog.h"

uintptr_t g_mainIntegrityCheckThreadStartAddress = 0x14275DE50;

using ProcessID_t = DWORD;
using ThreadID_t = DWORD;

#define ThreadQuerySetWin32StartAddress 0x9

bool TerminateThreadIfRunsTheMainIntegrityCheck(ThreadID_t thread_id)
{
    bool result = false;
    uintptr_t dwStartAddress;
    HANDLE thread_handle = OpenThread(THREAD_ALL_ACCESS, 0, thread_id);
    NTSTATUS nt_status = NtQueryInformationThread(thread_handle, (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &dwStartAddress, 0x8, 0);
    if (nt_status == 0)
    {
        LOG_DEBUG(L", start address: %llx\n", dwStartAddress);
        if (dwStartAddress == g_mainIntegrityCheckThreadStartAddress)
        {
            TerminateThread(thread_handle, 0);
            result = true;
        }
    }
    CloseHandle(thread_handle);
    return result;
}

// Thanks to Sunbeam at https://fearlessrevolution.com/viewtopic.php?t=9605#post-92942.
void DisableMainIntegrityCheck()
{
    auto thisProcessID = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(snapshot, &te))
        {
            size_t threadCounter = 0;
            do
            {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
                {
                    if (te.th32OwnerProcessID == thisProcessID)
                    {
                        LOG_DEBUG(L"Thread #%d, id: %x", threadCounter++, te.th32ThreadID);
                        if (bool mainCheckIsTerminated = TerminateThreadIfRunsTheMainIntegrityCheck(te.th32ThreadID))
                        {
                            LOG_DEBUG(L"Thread 0x%x terminated.\n", te.th32ThreadID);
                            break;
                        }
                    }
                }
            } while (Thread32Next(snapshot, &te));
        }
    }
    CloseHandle(snapshot);
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
