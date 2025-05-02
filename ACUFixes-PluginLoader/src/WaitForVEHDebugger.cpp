#include "pch.h"

using ProcessID_t = DWORD;
using ThreadID_t = DWORD;

#include <TlHelp32.h>
#include <winternl.h>
#pragma comment(lib, "ntdll")

// Thanks to Sunbeam at https://fearlessrevolution.com/viewtopic.php?t=9605#post-92942.
std::vector<HANDLE> SuspendAllOtherThreads()
{
    auto thisProcessID = GetCurrentProcessId();
    auto thisThreadID = GetCurrentThreadId();
    std::vector<HANDLE> suspendedThreads;
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
                    if (te.th32OwnerProcessID == thisProcessID && te.th32ThreadID != thisThreadID)
                    {
                        HANDLE thread_handle = OpenThread(THREAD_ALL_ACCESS, 0, te.th32ThreadID);
                        DWORD prevSuspendCount = SuspendThread(thread_handle);
                        //LOG_DEBUG(DefaultLogger
                        //    , L"handle: %8llX, prevSuspendCount: %u\n"
                        //    , thread_handle
                        //    , prevSuspendCount
                        //);
                        suspendedThreads.push_back(thread_handle);
                    }
                }
            } while (Thread32Next(snapshot, &te));
        }
    }
    CloseHandle(snapshot);
    return suspendedThreads;
}
class SuspendAllOtherThreadsRAII
{
public:
    std::vector<HANDLE> m_SuspendedThreads;
    SuspendAllOtherThreadsRAII() : m_SuspendedThreads(SuspendAllOtherThreads()) {}
    ~SuspendAllOtherThreadsRAII() { for (HANDLE handle : m_SuspendedThreads) ResumeThread(handle); }
};
/*
I'm unable to launch ACU under, say, the Visual Studio debugger,
but early after injection (e.g. from the DllMain) I can suspend all threads
until I detect that Cheat Engine's VEH debugger is attached.
It isn't as good of an option, but it has uses for very-early-debugging.
*/
void SuspendThreadsUntilCheatEnginesVEHDebuggerIsAttached()
{
    SuspendAllOtherThreadsRAII _pause;
    while (true)
    {
        if (GetModuleHandleA("vehdebug-x86_64.dll") != NULL)
        {
            break;
        }
        Sleep(200);
    }
}
