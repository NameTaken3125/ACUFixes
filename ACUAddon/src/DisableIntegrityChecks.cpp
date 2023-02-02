#include "pch.h"

#include <TlHelp32.h>
#include <winternl.h>
#pragma comment(lib, "ntdll")

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
        std::cout << ", start address: " << std::hex << dwStartAddress << std::endl;
        if (dwStartAddress == g_mainIntegrityCheckThreadStartAddress)
        {
            TerminateThread(thread_handle, 0);
            result = true;
        }
    }
    CloseHandle(thread_handle);
    return result;
}

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
                        std::cout << "Thread #" << std::dec << threadCounter++ << ", id: " << std::hex << te.th32ThreadID;
                        if (bool mainCheckIsTerminated = TerminateThreadIfRunsTheMainIntegrityCheck(te.th32ThreadID))
                        {
                            std::cout << "Thread 0x" << std::hex << te.th32ThreadID << " terminated." << std::endl;
                            break;
                        }
                    }
                }
            } while (Thread32Next(snapshot, &te));
        }
    }
    CloseHandle(snapshot);
}
