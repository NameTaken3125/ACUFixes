
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
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

// Thanks https://learn.microsoft.com/en-us/windows/win32/psapi/enumerating-all-modules-for-a-process
bool IsDLLLoadedInProcess(DWORD processID, const fs::path& dllAbsolutePath)
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    // Get a handle to the process.
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);
    if (NULL == hProcess)
        return false;

    bool isDLLAlreadyLoaded = false;

    // Get a list of all the modules in this process.
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            wchar_t szModName[MAX_PATH];
            // Get the full path to the module's file.
            if (GetModuleFileNameExW(hProcess, hMods[i], szModName, MAX_PATH))
            {
                if (dllAbsolutePath == szModName)
                {
                    isDLLAlreadyLoaded = true;
                    break;
                }
            }
        }
    }
    // Release the handle to the process.
    CloseHandle(hProcess);
    return isDLLAlreadyLoaded;
}

fs::path GetThisExecutablePath()
{
    wchar_t pathBuf[MAX_PATH];
    GetModuleFileName(NULL, pathBuf, MAX_PATH);
    return pathBuf;
}

// Thanks https://cocomelonc.github.io/tutorial/2021/09/20/malware-injection-2.html
bool Inject(const fs::path& injectedDLLPath, ProcessID_t pid)
{
    // handle to kernel32 and pass it to GetProcAddress
    HMODULE hKernel32 = GetModuleHandleA("Kernel32");
    if (hKernel32 == NULL) { return false; }
    VOID* lb = GetProcAddress(hKernel32, "LoadLibraryA");
    if (lb == NULL) { return false; }

    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    HANDLE remoteThread;
    LPVOID remoteBuffer;

    std::string pathString = injectedDLLPath.string();
    size_t howManyBytesToAllocateInProcess = pathString.length() + 1;

    // allocate memory buffer for remote process
    remoteBuffer = VirtualAllocEx(processHandle, NULL, howManyBytesToAllocateInProcess, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
    if (remoteBuffer == NULL) { return false; }

    // "copy" DLL path between processes
    WriteProcessMemory(processHandle, remoteBuffer, pathString.c_str(), howManyBytesToAllocateInProcess, NULL);

    // Start new thread from the target process.
    remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)lb, remoteBuffer, 0, NULL);
    CloseHandle(processHandle);

    return true;
}

int main()
{
    std::optional<ProcessID_t> pid = FindPID(g_targetProcessName);
    if (!pid) {
        wprintf(L"[X] Failed to find process for executable named \"%s\"", g_targetProcessName);
        return -1;
    }
    wprintf(L"[+] %s PID: %x\n", g_targetProcessName, pid.value());
    fs::path injectedDLLPath = GetThisExecutablePath().parent_path() / g_DLLPathRelativeToThisExecutable;
    // If the DLL is already injected, and I accidentally inject it again, the `DLL_PROCESS_ATTACH`
    // signal will not be received again, but the DLL's refcount will be >= 2.
    // If I then "quit" the injected DLL "from inside", the refcount will be left >= 1.
    // This will prevent the DLL from receiving `DLL_PROCESS_ATTACH` on consequent injections,
    // meaning that these consequent injections will have no result until the target process is restarted.
    // Here I check whether or not the DLL is already loaded in the process,
    // and don't reinject if it is.
    bool isDLLAlreadyLoaded = IsDLLLoadedInProcess(*pid, injectedDLLPath);
    if (isDLLAlreadyLoaded) {
        wprintf(L"[X] \"%s\" is already loaded in the process. There is no use injecting it again.\n", injectedDLLPath.wstring().c_str());
        return -1;
    }
    wprintf(L"[*] Trying to find and inject \"%s\"\n", injectedDLLPath.wstring().c_str());
    bool seeminglySucceeded = Inject(injectedDLLPath, pid.value());
    if (!seeminglySucceeded) {
        wprintf(L"[X] Failed to inject.");
        return -1;
    }
    wprintf(L"[+] The DLL should now be injected.");

    return 0;
}
