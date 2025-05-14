#include "pch.h"

#include "EarlyHooks/ThreadOperations.h"

std::vector<HANDLE> SuspendAllOtherThreads()
{
    auto thisThreadID = GetCurrentThreadId();
    std::vector<HANDLE> suspendedThreads;
    ForEachThreadIDOfThisProcess([&](ThreadID_t threadID)
        {
            if (threadID == thisThreadID) return false;
            HANDLE thread_handle = OpenThread(THREAD_ALL_ACCESS, 0, threadID);
            if (!thread_handle) return false;
            DWORD prevSuspendCount = SuspendThread(thread_handle);
            suspendedThreads.push_back(thread_handle);
            return false;
        });
    return suspendedThreads;
}
/*
I'm unable to launch ACU under, say, the Visual Studio debugger,
but early after injection I can suspend all threads
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
