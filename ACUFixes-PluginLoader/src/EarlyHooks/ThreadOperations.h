#pragma once


#include <winternl.h>
#include <TlHelp32.h>

using ThreadID_t = DWORD;

template<typename CallableOnThreadID>
void ForEachThreadIDOfThisProcess(CallableOnThreadID&& callableForEveryThreadID)
{
    auto thisProcessID = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(snapshot, &te))
        {
            do
            {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
                {
                    if (te.th32OwnerProcessID == thisProcessID)
                    {
                        const bool stopNow = callableForEveryThreadID(te.th32ThreadID);
                        if (stopNow) return;
                    }
                }
            } while (Thread32Next(snapshot, &te));
        }
    }
    CloseHandle(snapshot);
}
uintptr_t GetThreadStartAddress(HANDLE thread_handle);
uintptr_t GetCurrentThreadStartAddress();
struct FoundThread
{
    ThreadID_t m_ThreadID;
    HANDLE m_ThreadHandle;
};
std::optional<FoundThread> FindThreadForStartAddress(uintptr_t startAddress);

std::vector<HANDLE> SuspendAllOtherThreads();
class SuspendAllOtherThreadsRAII
{
public:
    std::vector<HANDLE> m_SuspendedThreads;
    SuspendAllOtherThreadsRAII() : m_SuspendedThreads(SuspendAllOtherThreads()) {}
    ~SuspendAllOtherThreadsRAII() { for (HANDLE handle : m_SuspendedThreads) { ResumeThread(handle); CloseHandle(handle); } }
};
