
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>
#include <optional>
#include <filesystem>
namespace fs = std::filesystem;

const wchar_t* g_targetProcessName = L"ACU.exe";
const char* g_DLLPathRelativeToThisExecutable = "ACUAddon.dll";
const char* g_configFileName = "acufixesinjector-config.json";

struct LoaderConfig
{
    std::string gameExeFilepath;
    bool automaticallyCloseConsoleWindow = false;

public:
    void ReadFile();
    void UpdateFile();
private:
    fs::path GetConfigFilepath();
};
LoaderConfig g_Config;

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
fs::path AbsolutePathInMyDirectory(const fs::path& pathRel) {
    return GetThisExecutablePath().parent_path() / pathRel;
};
void StartAnExecutable(const fs::path& absPath)
{
    ShellExecuteW(NULL
        , L"open"
        , absPath.wstring().c_str()
        , L""
        , absPath.parent_path().wstring().c_str()
        , SW_SHOW
    );
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
#include "SimpleJSON/json.hpp"
#include "Serialization/Serialization.h"
#include "Serialization/BooleanAdapter.h"
using json::JSON;
#include <fstream>
#include <sstream>
namespace json {
JSON FromFile(const fs::path& path)
{
    std::ifstream ifs(path);
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string loadedString = ss.str();
    return JSON::Load(loadedString);
}
void ToFile(const JSON& obj, const fs::path& path)
{
    std::ofstream ofs(path);
    ofs << obj.dump();
}
}
void LoaderConfig::ReadFile()
{
    fs::path configPath = GetConfigFilepath();
    printf("[*] Trying to read saved executable path from config file '%s'\n", configPath.string().c_str());
    JSON cfg = json::FromFile(GetConfigFilepath());
    printf("[*] Read from config:\n%s\n", cfg.dump().c_str());
    READ_JSON_VARIABLE(cfg, gameExeFilepath, StringAdapterNoEscape);
    READ_JSON_VARIABLE(cfg, automaticallyCloseConsoleWindow, BooleanAdapter);
}
void LoaderConfig::UpdateFile()
{
    JSON obj;
    WRITE_JSON_VARIABLE(obj, gameExeFilepath, StringAdapterNoEscape);
    WRITE_JSON_VARIABLE(obj, automaticallyCloseConsoleWindow, BooleanAdapter);
    fs::path configFilepath = GetConfigFilepath();
    printf("[*] Updated config file at '%s':\n%s\n", configFilepath.string().c_str(), obj.dump().c_str());
    json::ToFile(obj, configFilepath);
}
fs::path LoaderConfig::GetConfigFilepath()
{
    fs::path configFullPath = AbsolutePathInMyDirectory(g_configFileName);
    return configFullPath;
}
#include <shobjidl.h>
/*
Thanks
https://learn.microsoft.com/en-us/windows/win32/learnwin32/example--the-open-dialog-box
https://cplusplus.com/forum/windows/275617/
*/
std::optional<fs::path> OpenDialog_FindSingleFile(const fs::path& executableName)
{
    std::optional<fs::path> result;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            std::wstring filetypeSpec = executableName.wstring();
            COMDLG_FILTERSPEC ComDlgFS[1] = { {L"Game Executable", filetypeSpec.c_str()}};
            hr = pFileOpen->SetFileTypes((UINT)std::size(ComDlgFS), ComDlgFS);
            if (SUCCEEDED(hr))
            {
                std::wstring windowTitle = L"Please find the game executable (" + executableName.wstring() + L')';
                hr = pFileOpen->SetTitle(windowTitle.c_str());
                if (SUCCEEDED(hr)) {
                    // Show the Open dialog box.
                    hr = pFileOpen->Show(NULL);

                    // Get the file name from the dialog box.
                    if (SUCCEEDED(hr))
                    {
                        IShellItem* pItem;
                        hr = pFileOpen->GetResult(&pItem);
                        if (SUCCEEDED(hr))
                        {
                            PWSTR pszFilePath;
                            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                            // Display the file name to the user.
                            if (SUCCEEDED(hr))
                            {
                                //MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
                                result = pszFilePath;
                                CoTaskMemFree(pszFilePath);
                            }
                            pItem->Release();
                        }
                    }
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return result;
}
class FindExeFilepath
{
public:
    std::optional<fs::path> DoAndReturnFilepath(const fs::path& executableName)
    {
        Do(executableName);
        return std::move(m_foundFilepath);
    }
private:
    void Do(const fs::path& executableName)
    {
        ReadFromConfig();
        if (m_foundFilepath)
        {
            printf("[+] Found EXE filepath in config: '%s'\n", m_foundFilepath->string().c_str());
            return;
        }
        else
        {
            printf("[x] EXE filepath not found in config.\n");
        }
        LookInThisExecutablesDirectory(executableName);
        if (m_foundFilepath)
        {
            printf("[+] Found a matching EXE in this executable's directory (%s)\n", m_foundFilepath->string().c_str());
            return;
        }
        else
        {
            printf("[x] Also didn't find a matching EXE in this executable's directory (%s)\n", GetThisExecutablePath().parent_path().string().c_str());
        }
        AskUserToProvideEXEFilepath(executableName);
        if (m_foundFilepath)
        {
            printf("[+] Provided new EXE filepath: '%s'\n", m_foundFilepath->string().c_str());
        }
    }
    void ReadFromConfig()
    {
        if (!g_Config.gameExeFilepath.empty())
        {
            this->m_foundFilepath = g_Config.gameExeFilepath;
        }
    }
    void LookInThisExecutablesDirectory(const fs::path& executableName)
    {
        fs::path pathToTest = GetThisExecutablePath().parent_path() / executableName;
        if (fs::exists(pathToTest))
        {
            m_foundFilepath = std::move(pathToTest);
        }
    }
    void AskUserToProvideEXEFilepath(const fs::path& executableName)
    {
        m_foundFilepath = OpenDialog_FindSingleFile(executableName);
    }
private:
    fs::path AbsolutePathInMyDirectory(const fs::path& pathRel) {
        return GetThisExecutablePath().parent_path() / pathRel;
    };
    fs::path GetConfigFilepath()
    {
        fs::path configFullPath = AbsolutePathInMyDirectory(g_configFileName);
        return configFullPath;
    }
    std::optional<fs::path> m_foundFilepath;
};
std::optional<fs::path> TryToFindGameExecutable(const fs::path& executableName)
{
    return FindExeFilepath().DoAndReturnFilepath(executableName);
}
bool g_attemptedToLaunchGameProcess = false;
int main_procedure()
{
    std::optional<ProcessID_t> pid = FindPID(g_targetProcessName);
    if (!pid) {
        wprintf(L"[x] Failed to find process for executable named \"%s\"\n", g_targetProcessName);
        std::optional<fs::path> exeToStart = TryToFindGameExecutable(g_targetProcessName);
        if (!exeToStart)
        {
            wprintf(L"[X] Can't find an open process, and can't start a process. Quitting then.");
            return -1;
        }
        g_Config.gameExeFilepath = exeToStart->string();
        StartAnExecutable(*exeToStart);
        wprintf(L"[*] Trying to start \"%s\"\n", exeToStart->wstring().c_str());
        g_attemptedToLaunchGameProcess = true;
        pid = FindPID(g_targetProcessName);
        if (!pid) {
            wprintf(L"[X] Failed again to find process for \"%s\"\n", g_targetProcessName);
            return -1;
        }
        wprintf(L"[+] Process started.\n");
    }
    wprintf(L"[+] %s PID: %x\n", g_targetProcessName, pid.value());
    fs::path thisExecutablePath = GetThisExecutablePath();
    fs::path injectedDLLPath = thisExecutablePath.parent_path() / g_DLLPathRelativeToThisExecutable;
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
    if (!fs::exists(injectedDLLPath))
    {
        wprintf(L"[x] Warning: '%s' doesn't seem to exist, but I'll try to inject anyways.\n", injectedDLLPath.wstring().c_str());
    }
    bool seeminglySucceeded = Inject(injectedDLLPath, pid.value());
    if (!seeminglySucceeded) {
        wprintf(L"[X] Failed to inject.");
        return -1;
    }
    wprintf(L"[+] Did my best to inject the DLL.\n");

    return 0;
}
#include <conio.h>
void WaitForAnyKeypress() {
    while (_kbhit() == 0) {
        Sleep(200);
    }
}
int main()
{
    g_Config.ReadFile();
    int result = main_procedure();
    g_Config.UpdateFile();
    if (g_attemptedToLaunchGameProcess) {
        printf(
            "\n\nIf the game process fails to start, you can try to delete the config file\n"
            "(%s)\n"
            "in this executable's directory. It will be regenerated\n"
            "automatically. If that doesn't help, try to start this program\n"
            "with the game already running.\n"
            , g_configFileName
        );
    }
    printf("Press any key to quit...");
    if (!g_Config.automaticallyCloseConsoleWindow) {
        WaitForAnyKeypress();
    }
    return result;
}
