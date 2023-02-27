
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;
const wchar_t* g_targetProcessName = L"ACU.exe";
fs::path g_DLLPathRelativeToThisExecutable = "ACUAddon.dll";

void EnableDebugPriv()
{
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tkp;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

    CloseHandle(hToken);
}


using ProcessID_t = DWORD;
std::optional<ProcessID_t> FindPID(const wchar_t* processName)
{
    EnableDebugPriv();

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    std::optional<ProcessID_t> result;
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (_wcsicmp(entry.szExeFile, processName) == 0)
            {
                result = entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);
    return result;
}

fs::path GetThisExecutablePath()
{
    wchar_t pathBuf[MAX_PATH];
    GetModuleFileName(NULL, pathBuf, MAX_PATH);
    return pathBuf;
}

void Inject(const fs::path& injectedDLLPath, ProcessID_t pid)
{
    // handle to kernel32 and pass it to GetProcAddress
    HMODULE hKernel32 = GetModuleHandleA("Kernel32");
    VOID* lb = GetProcAddress(hKernel32, "LoadLibraryA");

    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    HANDLE remoteThread;
    LPVOID remoteBuffer;

    std::string pathString = injectedDLLPath.string();
    size_t howManyBytesToAllocateInProcess = pathString.length() + 1;

    // allocate memory buffer for remote process
    remoteBuffer = VirtualAllocEx(processHandle, NULL, howManyBytesToAllocateInProcess, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

    // "copy" DLL path between processes
    WriteProcessMemory(processHandle, remoteBuffer, pathString.c_str(), howManyBytesToAllocateInProcess, NULL);

    // Start new thread from the target process.
    remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)lb, remoteBuffer, 0, NULL);
    CloseHandle(processHandle);
}

int main()
{
    std::optional<ProcessID_t> pid = FindPID(g_targetProcessName);
    if (!pid)
    {
        wprintf(L"Failed to find process for executable named \"%s\"", g_targetProcessName);
        return -1;
    }
    wprintf(L"%s PID: %x\n", g_targetProcessName, pid.value());

    fs::path injectedDLLPath = GetThisExecutablePath().parent_path() / g_DLLPathRelativeToThisExecutable;
    wprintf(L"Trying to find and inject DLL at \"%s\"\n", injectedDLLPath.wstring().c_str());
    Inject(injectedDLLPath, pid.value());

    return 0;
}
