#include "pch.h"

#include "ACU/basic_types.h"
#include "MyLog.h"

DEFINE_LOGGER_CONSOLE_AND_FILE(CrashLog, "[CrashLog]");

class Module
{
public:
    uint64 m_baseAddr;
    uint32 m_size;
    std::wstring m_name;
    bool IsAddressInRange(uint64 addr) const { return addr >= m_baseAddr && addr < m_baseAddr + m_size; }
    std::wstring GetFilename() const
    {
        fs::path fullpath{ m_name };
        return fullpath.filename().wstring();
    }
};
const Module* FindModuleForAddress(uint64 addr, const std::vector<Module>& modules)
{
    for (const Module& module_ : modules)
    {
        if (module_.IsAddressInRange(addr))
        {
            return &module_;
        }
    }
    return nullptr;
}
using AllModules_t = std::vector<Module>;
#include <winternl.h>
#include <Psapi.h>
AllModules_t GetAllModules()
{
    AllModules_t result;
    result.reserve(128);
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;

    hProcess = GetCurrentProcess();
    if (NULL == hProcess)
        return result;

    // Get a list of all the modules in this process.
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        const unsigned int numModules = (cbNeeded / sizeof(HMODULE));
        for (unsigned int i = 0; i < numModules; i++) {
            // Get the full path to the module's file.
            Module newMod;
            newMod.m_baseAddr = (uint64)hMods[i];
            newMod.m_name.reserve(MAX_PATH);
            std::array<wchar_t, MAX_PATH> fullpathBuf = { 0 };
            if (GetModuleFileNameExW(hProcess, hMods[i], &fullpathBuf[0], MAX_PATH))
            {
                newMod.m_name = fullpathBuf.data();
                MODULEINFO modInfo;
                if (GetModuleInformation(hProcess, hMods[i], &modInfo, sizeof(modInfo)))
                {
                    newMod.m_size = modInfo.SizeOfImage;
                    result.push_back(std::move(newMod));
                }
            }
        }
    }
    return result;
}
void PrintAllModules(const AllModules_t& modules)
{
    LOG_DEBUG(CrashLog, L"[*] All modules:\n");
    for (size_t i = 0; i < modules.size(); i++)
    {
        const Module& mod = modules[i];
        LOG_DEBUG(
            CrashLog,
            L"%3d. %16llX - %16llX: %s\n"
            , i
            , mod.m_baseAddr
            , mod.m_baseAddr + mod.m_size
            , mod.m_name.c_str()
        );
    }
}
std::wstring MakeAddressReadable(uintptr_t addr, const AllModules_t& allModules)
{
    std::wstringstream ss;

    const Module* foundInModule = FindModuleForAddress(addr, allModules);
    if (!foundInModule)
    {
        ss << L"0x" << std::hex << addr;
    }
    else
    {
        ss << foundInModule->GetFilename();
        ss << L"+";
        ss << std::hex;
        ss << (addr - foundInModule->m_baseAddr);
    }
    return ss.str();
}
/*
Additionally, from https://wat.coobie.dev/semestr%204/pnak/PNAKL4iY3/Tools/IDAFreeware84/cfg/exceptions.cfg
0xC0000005   EXCEPTION_ACCESS_VIOLATION         The instruction at 0x%a referenced memory at 0x%a. The memory could not be %s
0x80000002   EXCEPTION_DATATYPE_MISALIGNMENT    A datatype misalignment error was detected in a load or store instruction
0x80000003   EXCEPTION_BREAKPOINT               Software breakpoint exception
0x80000004   EXCEPTION_SINGLE_STEP              Single step exception
0xC000008C   EXCEPTION_ARRAY_BOUNDS_EXCEEDED    Array bounds exceeded
0xC000008D   EXCEPTION_FLT_DENORMAL_OPERAND     Floating point denormal operand
0xC000008E   EXCEPTION_FLT_DIVIDE_BY_ZERO       Floating point divide by zero
0xC000008F   EXCEPTION_FLT_INEXACT_RESULT       Floating point inexact result
0xC0000090   EXCEPTION_FLT_INVALID_OPERATION    Floating point invalid operation
0xC0000091   EXCEPTION_FLT_OVERFLOW             Floating point overflow
0xC0000092   EXCEPTION_FLT_STACK_CHECK          Floating point stack check
0xC0000093   EXCEPTION_FLT_UNDERFLOW            Floating point underflow
0xC0000094   EXCEPTION_INT_DIVIDE_BY_ZERO       Integer divide by zero
0xC0000095   EXCEPTION_INT_OVERFLOW             Integer overflow
0xC0000096   EXCEPTION_PRIV_INSTRUCTION         Priveleged instruction
0xC0000006   EXCEPTION_IN_PAGE_ERROR            The instruction at "0x%a" referenced memory at "0x%a". The required data was not placed into memory because of an I/O error status of "0x%a"
0xC000001D   EXCEPTION_ILLEGAL_INSTRUCTION      An attempt was made to execute an illegal instruction
0xC0000025   EXCEPTION_NONCONTINUABLE_EXCEPTION Windows cannot continue from this exception
0xC00000FD   EXCEPTION_STACK_OVERFLOW           A new guard page for the stack cannot be created (stack overflow)
0xC0000026   EXCEPTION_INVALID_DISPOSITION      An invalid exception disposition was returned by an exception handler
0x80000001   EXCEPTION_GUARD_PAGE               A page of memory that marks the end of a data structure such as a stack or an array has been accessed
0xC0000008   EXCEPTION_INVALID_HANDLE           An invalid HANDLE was specified
0xEEFFACE    EXCEPTION_BCC_FATAL                Fatal unhandled exception in the BCC compiled program
0xEEDFAE6    EXCEPTION_BCC_NORMAL               Unhandled exception in the BCC compiled program
0x40010005   DBG_CONTROL_C                      CTRL+C was input to console process
0x40010008   DBG_CONTROL_BREAK                  CTRL+BREAK was input to console process
0xE06D7363   EXCEPTION_MSC_CPLUSPLUS            Microsoft C++ exception
0xE0434F4D   EXCEPTION_MANAGED_NET              Managed .NET exception
0xE0434352   EXCEPTION_MANAGED_NET_V4           Managed .NET exception (V4+)
0x4000001E   EXCEPTION_WX86_SINGLE_STEP         Single step exception (x86 emulation)
0x4000001F   EXCEPTION_WX86_BREAKPOINT          Software breakpoint exception (x86 emulation)
0x406D1388   MS_VC_EXCEPTION                    SetThreadName
*/
/*
https://web.archive.org/web/20070501045100/http://support.microsoft.com/kb/185294
The code is actually a cryptic mnemonic device, with the initial "E" standing for "exception" and the final 3 bytes (0x6D7363) representing the ASCII values of "msc".
https://devblogs.microsoft.com/oldnewthing/20100730-00/?p=1327

EXCEPTION_RECORD
+----------+
| E06D7363 |
+----------+
|  ~~~     |
+----------+
|* ~~~     |
+----------+
|* ~~~     |
+----------+
| 3 or 4   |
+----------+
|* ~~~     |
+----------+
|*Object   |
+----------+     +---+
|*       ------> |~~~|
+----------+     +---+
|*HINSTANCE|     |~~~|
+----------+     +---+
                 |~~~|
                 +---+    +---+
                 | -----> |~~~|
                 +---+    +---+    +---+
                          | -----> |~~~|
                          +---+    +---+    +----------+
                                   | -----> |*   ~~~   |
                                   +---+    +----------+
                                            |*   ~~~   |
                                            +----------+
                                            |Class name|
                                            +----------+
*/
#define EXCEPTION_MSC_CPLUSPLUS 0xE06D7363
#define MS_VC_EXCEPTION_SetThreadName 0x406D1388

const wchar_t* GetReadableExceptionCode(const ::EXCEPTION_RECORD& exceptionRecord)
{
    const wchar_t* readableExceptionCode = [&]() {
#define EXCEPTION_CASE(exceptionCode) \
	case exceptionCode:               \
		return L" (" L#exceptionCode L")"
        switch (exceptionRecord.ExceptionCode) {
            EXCEPTION_CASE(EXCEPTION_ACCESS_VIOLATION);
            EXCEPTION_CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
            EXCEPTION_CASE(EXCEPTION_BREAKPOINT);
            EXCEPTION_CASE(EXCEPTION_DATATYPE_MISALIGNMENT);
            EXCEPTION_CASE(EXCEPTION_FLT_DENORMAL_OPERAND);
            EXCEPTION_CASE(EXCEPTION_FLT_DIVIDE_BY_ZERO);
            EXCEPTION_CASE(EXCEPTION_FLT_INEXACT_RESULT);
            EXCEPTION_CASE(EXCEPTION_FLT_INVALID_OPERATION);
            EXCEPTION_CASE(EXCEPTION_FLT_OVERFLOW);
            EXCEPTION_CASE(EXCEPTION_FLT_STACK_CHECK);
            EXCEPTION_CASE(EXCEPTION_FLT_UNDERFLOW);
            EXCEPTION_CASE(EXCEPTION_ILLEGAL_INSTRUCTION);
            EXCEPTION_CASE(EXCEPTION_IN_PAGE_ERROR);
            EXCEPTION_CASE(EXCEPTION_INT_DIVIDE_BY_ZERO);
            EXCEPTION_CASE(EXCEPTION_INT_OVERFLOW);
            EXCEPTION_CASE(EXCEPTION_INVALID_DISPOSITION);
            EXCEPTION_CASE(EXCEPTION_NONCONTINUABLE_EXCEPTION);
            EXCEPTION_CASE(EXCEPTION_PRIV_INSTRUCTION);
            EXCEPTION_CASE(EXCEPTION_SINGLE_STEP);
            EXCEPTION_CASE(EXCEPTION_STACK_OVERFLOW);

            EXCEPTION_CASE(EXCEPTION_MSC_CPLUSPLUS);
            EXCEPTION_CASE(MS_VC_EXCEPTION_SetThreadName);
        default:
            return L"";
        }
        }();
#undef EXCEPTION_CASE
    return readableExceptionCode;
}
void LogExceptionCode(const ::EXCEPTION_RECORD& exceptionRecord, const AllModules_t& allModules)
{
    const wchar_t* readableExceptionCode = GetReadableExceptionCode(exceptionRecord);

    LOG_DEBUG(
        CrashLog,
        L"[error][X] CRASH? Exception %llX%s at %s.\n"
        , exceptionRecord.ExceptionCode
        , readableExceptionCode
        , MakeAddressReadable(reinterpret_cast<uintptr_t>(exceptionRecord.ExceptionAddress), allModules).c_str()
    );
}
void LogException(::EXCEPTION_POINTERS& excPointers)
{
    if (excPointers.ExceptionRecord->ExceptionCode < 0x80000000)
    {
        // Supposedly the exceptions in this range aren't indicating failure
        // https://learn.microsoft.com/en-us/windows/win32/learnwin32/error-codes-in-com?redirectedfrom=MSDN
        // For example, 0x40010006 is being thrown several times without noticeable reason or harm.
        // https://stackoverflow.com/questions/12298406/how-to-treat-0x40010006-exception-in-vectored-exception-handler
        // UPD: Might be from attaching the VEH debugger of the Cheat Engine.
        return;
        //if (excPointers.ExceptionRecord->ExceptionCode == 0x406D1388)
        //{
        //    // Seems like SetThreadName() (and possibly steam_api64.dll?) is throwing this like 25 times at startup:
        //    // From https://github.com/go-delve/delve/issues/1383#issuecomment-431282991 :
        //    //    steam_api64.dll is trying to use a weird MS protocol to set the thread name in the debugger
        //    //    by raising a special exception (code: 0x406d1388).
        //    //    In theory steam_api64.dll should be setting an exception handler and it should be fine
        //    //    for delve to send back the exception. In practice if we do the program crashes.
        //    return;
        //}
    }
    std::vector<Module> modules = GetAllModules();
    PrintAllModules(modules);
    LogExceptionCode(*excPointers.ExceptionRecord, modules);
}
static std::optional<LPTOP_LEVEL_EXCEPTION_FILTER> g_previousTopLevelSEHandler;
LONG __stdcall CrashLogUnhandledExceptionHandler(::EXCEPTION_POINTERS* exception) noexcept
{
    // I fail to trigger this. Maybe this means that there are more vectored exception handlers
    // and they prevent this code from being reached.
    LOG_DEBUG(CrashLog
        , L"[error][X] Unhandled Exception. ExceptionCode %llX%s:\n"
        , exception->ExceptionRecord->ExceptionCode
        , GetReadableExceptionCode(*exception->ExceptionRecord)
    );
    LogException(*exception);
    MessageBoxA(NULL, "CrashLog: Unhandled exception", "Crash Log", MB_OK | MB_ICONSTOP);
    if (g_previousTopLevelSEHandler && *g_previousTopLevelSEHandler)
        return (*g_previousTopLevelSEHandler)(exception);
    return EXCEPTION_CONTINUE_SEARCH;
}
void HandleExc_SetThreadName(::EXCEPTION_POINTERS* exception)
{
    DWORD numParams = exception->ExceptionRecord->NumberParameters;
    LOG_DEBUG(CrashLog
        , L"    ThreadID: %x, NumParams: %d\n"
        , GetCurrentThreadId()
        , numParams
    );
    if (numParams == 6)
    {
        LOG_DEBUG(CrashLog
            , L"    Name    : \"%s\", Addr: %llX\n"
            , utf8_and_wide_string_conversion::utf8_decode((const char*)exception->ExceptionRecord->ExceptionInformation[1]).c_str()
            , exception->ExceptionRecord->ExceptionInformation[5]
        );
    }
    else if (numParams == 3)
    {
        LOG_DEBUG(CrashLog
            , L"    Name    : %s\n"
            , utf8_and_wide_string_conversion::utf8_decode((const char*)exception->ExceptionRecord->ExceptionInformation[1]).c_str()
        );
    }
    else
    {
        for (size_t i = 0; i < numParams; i++)
        {
            LOG_DEBUG(CrashLog
                , L"    %d. %llX\n"
                , i
                , exception->ExceptionRecord->ExceptionInformation[i]
            );
        }
    }
}
LONG _stdcall CrashLogVectoredExceptionHandler(::EXCEPTION_POINTERS* exception) noexcept
{
    LOG_DEBUG(CrashLog
        , L"[*] Vectored Exception Handler. ExceptionCode %llX%s:\n"
        , exception->ExceptionRecord->ExceptionCode
        , GetReadableExceptionCode(*exception->ExceptionRecord)
    );
    const bool isShouldLogCPPExceptions = false;
    if (exception->ExceptionRecord->ExceptionCode == MS_VC_EXCEPTION_SetThreadName)
    {
        HandleExc_SetThreadName(exception);
    }
    else if (exception->ExceptionRecord->ExceptionCode == 0x40010006)
    {
        LOG_DEBUG(CrashLog
            , L"[*] Not logging, looks like Cheat Engine's VEH debugger just attached..."
        );
    }
    else if (exception->ExceptionRecord->ExceptionCode == EXCEPTION_MSC_CPLUSPLUS && !isShouldLogCPPExceptions)
    {
        LOG_DEBUG(CrashLog
            , L"[*] Not logging the C++ exception..."
        );
    }
    else
    {
        LogException(*exception);
    }
    // Returns `0x14290BCF4`.
    // However, I'm unable to trigger neither that function nor the one I set.
    if (!g_previousTopLevelSEHandler)
        g_previousTopLevelSEHandler = ::SetUnhandledExceptionFilter(&CrashLogUnhandledExceptionHandler);
    return EXCEPTION_CONTINUE_SEARCH;
}
PVOID g_CrashLogVEHhandle = nullptr;
void InstallCrashLog()
{
    PVOID vehHandle = ::AddVectoredExceptionHandler(0, &CrashLogVectoredExceptionHandler);
    g_CrashLogVEHhandle = vehHandle;
}
