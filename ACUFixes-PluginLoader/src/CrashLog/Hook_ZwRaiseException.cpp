#include "pch.h"

#include "Hook_ZwRaiseException.h"
#include "MyLog.h"

#include <winternl.h>

// From ntdll.dll.
// https://doxygen.reactos.org/d0/ddc/ndk_2kefuncs_8h.html#a94695b119d0f316119f670cb1ee80495
extern "C" __declspec(dllimport) NTSTATUS NTAPI ZwRaiseException(
    _In_ PEXCEPTION_RECORD   ExceptionRecord,
    _In_ PCONTEXT            Context,
    _In_ BOOLEAN             SearchFrames
);

void LogTopLevelException(const std::string_view& whereCaught, ::EXCEPTION_RECORD& ExceptionRecord, ::CONTEXT& ContextRecord);
bool g_Hook_ZwRaiseException_IsCanBeSafelyContinued = false;
void ZwRaiseException_PrologueHook(AllRegisters* params)
{
    EXCEPTION_RECORD* ExceptionRecord = (EXCEPTION_RECORD*)params->rcx_;
    CONTEXT* Context = (CONTEXT*)params->rdx_;
    LogTopLevelException("ZwRaiseException()", *ExceptionRecord, *Context);

    if (g_Hook_ZwRaiseException_IsCanBeSafelyContinued)
        return;
    ImGuiTextBuffer buf;
    buf.appendf(
        "Game cannot recover from this error and will be closed now.\n"
        "Last chance to try to debug:\n"
        "ExceptionRecord:   %llX\n"
        "Thread Context:    %llX\n"
        "Rip:               %llX\n"
        "Rsp:               %llX\n"
        "This handler:      %llX\n"
        , ExceptionRecord
        , Context
        , Context->Rip
        , Context->Rsp
        , ZwRaiseException_PrologueHook
    );
    MessageBoxA(NULL, buf.c_str(), "CrashLog: Last chance to debug", MB_OK | MB_ICONERROR);
    /*
    I'd rather not force exit the process, even though there isn't much
    that can be done to recover from exception. When a debugger is present,
    I'd prefer to leave its default behavior - to let the exception get raised
    over and over again until the programmer decides to let it go.
    But without a disassembler I cannot create a clean hook in ntdll, so
    crashing is better than undefined execution.
    */
    const int exitCode = 'ZwRE'; //== 0x5a775245
    std::exit(exitCode);
}


Hook_ZwRaiseException::Hook_ZwRaiseException()
{
    FARPROC pZwRaiseException = (FARPROC)ZwRaiseException;
    if (!pZwRaiseException) return;
    /*
    Without a disassembler I do not know how many bytes can be safely stolen -
    this differs between versions of ntdll.dll.
    And although the ZwRaiseException() is short, I cannot fully reimplement it
    because the syscall differs between versions of Windows.
    I better crash the application if I cannot steal the correct number of bytes.
    Maybe try using the INT3 instruction at the beginning of ZwRaiseException(),
    catch it in a VEH, then run a clean copy of ZwRaiseException? Would that be of some use?
    More like ZwRaiseInception, amirite?
    */


    DEFINE_ADDR(ZwRaiseException_prologue, (uintptr_t)pZwRaiseException);
    auto TryGetStandardSyscallPrologueBytes = [](SymbolWithAnAddress& symbolAddr) -> std::optional<ByteVector>
        {
            ByteVector firstTwoOpcodesHopefully = symbolAddr.CopyCurrentBytes(8);
            std::array<byte, 4> standardSyscallPrologue = { {
                    0x4C, 0x8B, 0xD1,               // - mov r10,rcx
                    0xB8, /* syscall_dword */       // - mov eax,<syscall_dword>
                } };
            for (size_t i = 0; i < standardSyscallPrologue.size(); i++)
            {
                if (standardSyscallPrologue[i] != firstTwoOpcodesHopefully.m_bytes[i])
                    return {};
            }
            return firstTwoOpcodesHopefully;
        };
    std::optional<ByteVector> firstTwoOpcodes = TryGetStandardSyscallPrologueBytes(ZwRaiseException_prologue);
    size_t howManyBytesStolen;
    if (firstTwoOpcodes)
    {
        howManyBytesStolen = firstTwoOpcodes->m_bytes.size();
        g_Hook_ZwRaiseException_IsCanBeSafelyContinued = true;
    }
    else
    {
        howManyBytesStolen = 5;
        g_Hook_ZwRaiseException_IsCanBeSafelyContinued = false;
    }
    PresetScript_CCodeInTheMiddle((uintptr_t)pZwRaiseException, howManyBytesStolen,
        ZwRaiseException_PrologueHook, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}
