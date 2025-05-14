#include "pch.h"

#include "ThreadOperations.h"
#pragma comment(lib, "ntdll")

#define ThreadQuerySetWin32StartAddress 0x9
uintptr_t GetThreadStartAddress(HANDLE thread_handle)
{
    uintptr_t dwStartAddress;
    NTSTATUS nt_status = NtQueryInformationThread(thread_handle, (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &dwStartAddress, 0x8, 0);
    if (nt_status == 0)
    {
        return dwStartAddress;
    }
    return 0;
}
uintptr_t GetCurrentThreadStartAddress()
{
    uintptr_t dwStartAddress = 0;
    NTSTATUS nt_status = NtQueryInformationThread(GetCurrentThread(), (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &dwStartAddress, 0x8, 0);
    return dwStartAddress;
}

std::optional<FoundThread> FindThreadForStartAddress(uintptr_t startAddress)
{
    std::optional<FoundThread> matchingThread;
    ForEachThreadIDOfThisProcess([&](DWORD threadID)
        {
            HANDLE thread_handle = OpenThread(THREAD_ALL_ACCESS, 0, threadID);
            uintptr_t dwStartAddress;
            NTSTATUS nt_status = NtQueryInformationThread(thread_handle, (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &dwStartAddress, 0x8, 0);
            if (nt_status == 0)
            {
                if (dwStartAddress == startAddress)
                {
                    matchingThread = { threadID, thread_handle };
                    return true;
                }
            }
            CloseHandle(thread_handle);
            return false;
        });
    return matchingThread;
}
