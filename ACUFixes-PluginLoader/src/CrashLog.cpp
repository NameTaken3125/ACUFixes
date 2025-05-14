#include "pch.h"

#include "ACU/basic_types.h"
#include "MyLog.h"
#include "EarlyHooks/ThreadOperations.h"
#include "EarlyHooks/VEH.h"

DEFINE_LOGGER_CONSOLE_AND_FILE(CrashLog, "[CrashLog]");

static void CrashLog_MessageBoxAF(const char* fmt, ...)
{
    ImGuiTextBuffer buf;
    va_list args;
    va_start(args, fmt);
    buf.appendfv(fmt, args);
    va_end(args);
    MessageBoxA(NULL, buf.c_str(), "CrashLog", MB_OK | MB_ICONSTOP);
}

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
static const char* GetReadableExceptionCodeA(const ::EXCEPTION_RECORD& exceptionRecord)
{
    const char* readableExceptionCode = [&]() {
#define EXCEPTION_CASE(exceptionCode) \
	case exceptionCode:               \
		return " (" #exceptionCode ")"
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
            return "";
        }
        }();
#undef EXCEPTION_CASE
    return readableExceptionCode;
}
void VEHLogExceptionCode(const ::EXCEPTION_RECORD& exceptionRecord, const AllModules_t& allModules)
{
    const wchar_t* readableExceptionCode = GetReadableExceptionCode(exceptionRecord);

    LOG_DEBUG(
        CrashLog,
        L"[x] Exception %llX%s at %s.\n"
        , exceptionRecord.ExceptionCode
        , readableExceptionCode
        , MakeAddressReadable(reinterpret_cast<uintptr_t>(exceptionRecord.ExceptionAddress), allModules).c_str()
    );
}
void LogVEHException(::EXCEPTION_POINTERS& excPointers)
{
    const bool doDumpOnVEHException = false;
    if (!doDumpOnVEHException) return;
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
    VEHLogExceptionCode(*excPointers.ExceptionRecord, modules);
}
class TopLevelExceptionLoggerTool
{
public:
    std::string m_WhereCaught;
    ::EXCEPTION_RECORD& ExceptionRecord;
    ::CONTEXT& ContextRecord;
    TopLevelExceptionLoggerTool(
        const std::string_view& whereCaught
        , ::EXCEPTION_RECORD& ExceptionRecord
        , ::CONTEXT& ContextRecord
    ) : m_WhereCaught(whereCaught)
        , ExceptionRecord(ExceptionRecord)
        , ContextRecord(ContextRecord)
    {}
    void DoLog()
    {
        LOG_DEBUG(CrashLog
            , L"[error][X] %s: uncaught exception! Code: %X%s At: %llX\n"
            , utf8_and_wide_string_conversion::utf8_decode(m_WhereCaught).c_str()
            , ExceptionRecord.ExceptionCode
            , GetReadableExceptionCode(ExceptionRecord)
            , ExceptionRecord.ExceptionAddress
        );
        m_AllModules = GetAllModules();
        PrintAllModules(m_AllModules);
        LOG_DEBUG(CrashLog,
            L"Readable ExceptionAddress: %s\n"
            , MakeAddressReadable((uintptr_t)ExceptionRecord.ExceptionAddress, m_AllModules).c_str()
        );
        DoStacktrace();
        LOG_DEBUG(CrashLog,
            utf8_and_wide_string_conversion::utf8_decode(m_StackBackTrace.c_str()).c_str()
        );
        CrashLog_MessageBoxAF(
            "[CrashLog] %s: uncaught exception!\n"
            "Code: %X%s\n"
            "At: %llX\n"
            "%s\n"
            "A crash log has been generated."
            , m_WhereCaught.c_str()
            , ExceptionRecord.ExceptionCode
            , GetReadableExceptionCodeA(ExceptionRecord)
            , ExceptionRecord.ExceptionAddress
            , m_StackBackTrace.c_str()
        );
    }
public:
    std::vector<Module> m_AllModules;
    ImGuiTextBuffer m_StackBackTrace;
    void DoStacktrace()
    {
        uintptr_t fromAddr = (uintptr_t)ExceptionRecord.ExceptionAddress;
        ULONG traceHash;
        std::array<void*, 64> frames = { 0 };
        USHORT numFrames = RtlCaptureStackBackTrace(0, (DWORD)frames.size(), &frames[0], &traceHash);

        USHORT startFromIdx = 0;
        for (size_t i = 0; i < numFrames; i++)
        {
            if ((uintptr_t)frames[i] == fromAddr)
            {
                startFromIdx = (USHORT)i;
                break;
            }
        }
        m_StackBackTrace.appendf(
            "Rsp: %llX\n"
            , ContextRecord.Rsp
        );
        m_StackBackTrace.appendf(
            "RtlCaptureStackBackTrace():\n"
            "%hu return addresses found:\n"
            , numFrames - startFromIdx
        );
        for (size_t i = startFromIdx; i < numFrames; i++)
        {
            m_StackBackTrace.appendf(
                "%2d. %s\n"
                , i - startFromIdx
                , utf8_and_wide_string_conversion::utf8_encode(MakeAddressReadable(reinterpret_cast<uintptr_t>(frames[i]), m_AllModules)).c_str()
            );
        }
    }
};
void LogTopLevelException(const std::string_view& whereCaught, ::EXCEPTION_RECORD& ExceptionRecord, ::CONTEXT& ContextRecord)
{
    TopLevelExceptionLoggerTool(whereCaught, ExceptionRecord, ContextRecord).DoLog();
}
static std::optional<LPTOP_LEVEL_EXCEPTION_FILTER> g_CrashLog_PreviousTopLevelSEHandler;
LONG __stdcall CrashLogUnhandledExceptionHandler(::EXCEPTION_POINTERS* exception) noexcept
{
    LogTopLevelException("UnhandledExceptionFilter()", *exception->ExceptionRecord, *exception->ContextRecord);
    if (g_CrashLog_PreviousTopLevelSEHandler && *g_CrashLog_PreviousTopLevelSEHandler)
        return (*g_CrashLog_PreviousTopLevelSEHandler)(exception);
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
    if (numParams == 6 || numParams == 3)
    {
        LOG_DEBUG(CrashLog
            , L"    Name    : \"%s\"\n"
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
        , L"[*] Vectored Exception Handler. Code %llX%s: At: %llX\n"
        , exception->ExceptionRecord->ExceptionCode
        , GetReadableExceptionCode(*exception->ExceptionRecord)
        , exception->ExceptionRecord->ExceptionAddress
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
        LogVEHException(*exception);
    }
    // Returns `0x14290BCF4`.
    // I cannot set the SEH _as_soon_as_ the mod is injected,
    // because my handler gets removed by the game.
    // This is why I set it from VEH, which does not get removed and triggers before SEH.
    // SEH doesn't catch _every_ crash though.
    // For example, it can catch an invalid dereference in the game code
    // but not in the VMProtect code, and not in some of the cases
    // where the call stack contains allocated code (but in some cases it can. Idk).
    if (!g_CrashLog_PreviousTopLevelSEHandler)
        g_CrashLog_PreviousTopLevelSEHandler = ::SetUnhandledExceptionFilter(&CrashLogUnhandledExceptionHandler);
    return EXCEPTION_CONTINUE_SEARCH;
}
std::optional<VEHandler> g_CrashLogVEH;
/*
The Vectored Exception Handler is the most successful way of catching exceptions.
The problem is it triggers on all exceptions, including the nonerror ones
like MS_VC_EXCEPTION_SetThreadName (0x406D1388) and the handled ones
like the C++ exceptions that get successfully caught. I can find no way of
distinguishing nonhandled critical exceptions from successfully handled, and bombarding the log
with stacktraces from successfully handled exceptions just doesn't seem like a good idea.
SetUnhandledExceptionFilter() _seems_ like the solution, but doesn't actually succeed
at catching exceptions that e.g. happen within the C++ receiver functions called from
my PresetScript_CCodeInTheMiddle() hooks.
(I think this has something to do with my implementation of
PresetScript_CCodeInTheMiddle() and how the code cave's stack frame
_in_some_cases_ cannot be recovered during a stack walk/unwind. Hmmmm.
Regardless of the reason...)
I'd like to have the ability to catch all crashes, especially
since I already know VMProtect likes to make crashes silent.
This is why I also hook the ZwRaiseException() within the ntdll.dll
as a last resort. Such a hook doesn't produce a good stacktrace
and interacts poorly with an attached debugger by preventing UnhandledExceptionFilter()
from running when it otherwise would.
*/
void InstallCrashLog()
{
    g_CrashLogVEH.emplace(0, CrashLogVectoredExceptionHandler);
}
