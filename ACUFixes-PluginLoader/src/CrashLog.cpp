#include "pch.h"

#include "ACU/basic_types.h"
#include "MyLog.h"

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
    LOG_DEBUG(DefaultLogger, L"[*] All modules:\n");
    for (size_t i = 0; i < modules.size(); i++)
    {
        const Module& mod = modules[i];
        LOG_DEBUG(
            DefaultLogger,
            L"%3d. %016llX - %016llX: %s\n"
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
void LogExceptionCode(const ::EXCEPTION_RECORD& exceptionRecord, const AllModules_t& allModules)
{
    const wchar_t* readableExceptionCode = [&]() {
#define EXCEPTION_CASE(exceptionCode) \
	case exceptionCode:               \
		return L"==\"" L#exceptionCode L"\""
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
        default:
            return L"";
        }
    }();
#undef EXCEPTION_CASE

    LOG_DEBUG(
        DefaultLogger,
        L"[error][X] CRASH? Unhandled exception %llX%s at %s.\n"
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
LONG __stdcall UnhandledExceptionHandler(::EXCEPTION_POINTERS* exception) noexcept
{
    // I fail to trigger this. I think this means that there are more vectored exception handlers
    // and they prevent this code from being reached.
    LOG_DEBUG(DefaultLogger, L"[X] Unhandled exception. ExceptionCode %llX:\n", exception->ExceptionRecord->ExceptionCode);
    LogException(*exception);
    return EXCEPTION_CONTINUE_SEARCH;
}
LONG _stdcall VectoredExceptionHandler(::EXCEPTION_POINTERS* exception) noexcept
{
    LOG_DEBUG(DefaultLogger, L"[*] Vectored Exception Handler. ExceptionCode %llX:\n", exception->ExceptionRecord->ExceptionCode);
    LogException(*exception);
    // Returns `0x14290BCF4`.
    // However, I'm unable to trigger neither that function nor the one I set.
    LPTOP_LEVEL_EXCEPTION_FILTER previousTopLevelSEHandler = ::SetUnhandledExceptionFilter(&UnhandledExceptionHandler);
    return EXCEPTION_CONTINUE_SEARCH;
}
PVOID g_VEHhandle = nullptr;
void InstallCrashLog()
{
    PVOID vehHandle = ::AddVectoredExceptionHandler(0, &VectoredExceptionHandler);
    g_VEHhandle = vehHandle;
}
